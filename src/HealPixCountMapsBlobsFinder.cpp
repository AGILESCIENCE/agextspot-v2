////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION
//       AGILE Science Tools
//       AG_ap5
//       First release: 2017
//       Authors: Leonardo Baroncelli leonardo.baroncelli@inaf.it, Giancarlo Zollino
//
// NOTICE
//       Any information contained in this software
//       is property of the AGILE TEAM and is strictly
//       private and confidential.
//       Copyright (C) 2005-2019 AGILE Team. All rights reserved.
/*

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
////////////////////////////////////////////////////////////////////////////////////
#include "HealPixCountMapsBlobsFinder.h"

HealPixCountMapsBlobsFinder::HealPixCountMapsBlobsFinder(float _cdelt1, float _cdelt2, float _psf, double _classification_threshold)
   : BlobsFinder(_cdelt1, _cdelt2, _psf, _classification_threshold)
{

  // cout << "HealPixCountMapsBlobsFinder" << endl;
  file_format = "HEALPIX COUNT MAP";

}

string HealPixCountMapsBlobsFinder::get_format()
{
		return file_format;
}

void HealPixCountMapsBlobsFinder::print_vector_int(string name, vector<int>& v_int, bool print_vertical)
{
  cout << "Vector " << name << ": [";
  int count = 0;
  for (const auto & i : v_int)
  {
    if(print_vertical)
      cout <<"\nv["<<count<<"] = "<< i << " ";
    else
      cout << i << " ";
    count ++;
  }
  cout << "]" << endl;
}

///////////////////////////////////////////////////////////////////////////////
//
//		Extraction of blobs from FITS HEALPix images

vector<Blob*> HealPixCountMapsBlobsFinder::find_blobs(string fitsfilename, string fitsfile_folder, bool save_cv_steps, string output_folder)
{

  cout << "[HealPixCountMapsBlobsFinder] Finding blobs.." << endl;

  vector<Blob*> blobs;

  const string fitsfilePath = fitsfile_folder + "/" + fitsfilename;

  // Healpix_Map<int> map = read_Healpix_map_from_fits<int>(fitsfilePath);
  Healpix_Map<double> map;
	read_Healpix_map_from_fits (fitsfilePath, map,1,2);

  if (map.Scheme()==NEST) map.swap_scheme();

  int countPix=0;
  for( int i = 0; i <map.Npix(); i ++)
    countPix += map[i];
  cout << "Data quality -> sum of map pixel values = " << countPix << endl;
  if(countPix == 0)
  {
    cout<<"[HealPixCountMapsBlobsFinder] ERROR: Input map empty!"<<endl;
    exit(1);
  }

  int map_resolution = map.Order();
  long int nPix = map.Npix();

  cout << "Map nPix: " << nPix << endl;
  cout << "Map order: " << map_resolution << endl;
  cout << "Map schema: " << map.Scheme() << endl;

  /*
  The Input Healpix CTA count map and input Healpix AGILE count map
  have a very different range of values.
  The following code lines do the linear interpolation to uniform
  the input maps values.
  */
  float maxInRange = -99999999;
  float minInRange = 99999999;
  for( int i = 0; i < nPix; i ++ )
  {
    if(map[i] > maxInRange)
    {
      maxInRange = map[i];
    }
    if(map[i] < minInRange)
    {
      minInRange = map[i];
    }
  }


  float x = 0;
  int minOutRange = 0;
  int maxOutRange = 255;
  for( int i = 0; i < nPix; i ++ )
  {
    x = (map[i] - minInRange) / (maxInRange - minInRange);
    map[i] = minOutRange + (maxOutRange - minOutRange) * x;
  }

  if(save_cv_steps)
      save_healpix_INT_image(output_folder+"/"+fitsfilename+"_"+"original_map.fits",map);


  // fwhm (expressed in arcmin): full width at half maximum in arcmin is equal 2 times psf (expressed in degree)
  double fwhm = (2*psf)*60;
  string outfile = output_folder+"/"+fitsfilename+"_"+"convolved_map.fits";
  Healpix_Map<double> convolved_map;
  convolved_map = gaussian_smoothing<double>(fwhm, map, outfile);


  if(save_cv_steps)
    save_healpix_INT_image(output_folder+"/"+fitsfilename+"_"+"convolved_map.fits",convolved_map);


  Healpix_Map<float> thresholded_map = thresholding(convolved_map, nPix, map_resolution, classification_threshold);

  if(save_cv_steps)
    save_healpix_FLOAT_image(output_folder+"/"+fitsfilename+"_"+"thresholded_map.fits",thresholded_map);

  vector < vector <int> > connected_components;
  Healpix_Map <double> labeled_map = find_connected_components(thresholded_map, map_resolution, connected_components);

  // cout << "Trovati "<< connected_components.size() << " connected components."<< endl;

  if(save_cv_steps)
    save_healpix_INT_image(output_folder+"/"+fitsfilename+"_"+"labelelled_map.fits",labeled_map);



  //Compute number of pixels, number of photons, contour
  for(vector < vector <int> >:: iterator it = connected_components.begin(); it < connected_components.end(); ++it)
  {
    vector <int> pixels = *it;

    vector<pair<MapCoords,int> > points;
    vector<pair<MapCoords,int> > photon_points;
    vector<MapCoords > contour_points;

    compute_blob_features( map_resolution, pixels, map, thresholded_map, labeled_map, points, photon_points, contour_points);


    // DEBUG print
    /*
    Get index of element from iterator
    int index = std::distance(connected_components.begin(), it);
    cout<<"Blob "<< index+1 << endl;
    cout<<"Number of pixel: "<<points.size()<<endl;
    cout<<"Number of photons: "<<photon_points.size()<<endl;
    cout<<"Contour pixels: "<<endl;
    for(vector<MapCoords > ::iterator it = contour_points.begin(); it < contour_points.end(); ++it )
    {
      MapCoords cont_pixel = *it;
      cout <<cont_pixel.y<<", "<<cont_pixel.x<<endl;
    }
    cout <<"\n"<<endl;
    */

    // Creating a Blob
    // The following condition (photonsInBlobs.size() >= 2) is needed
    // to avoid the photon closeness problem:
    // a blob made by 1 photon has a photonCloseness = 1 (the minimum).
    // In this context, both a background blob and a flux blob has the
    // same feature. In addition, there are a lot of background blobs
    // made by only 1 photon and this can alter the photon closeness distribution.

    if( photon_points.size() >= 2)
    {
      Blob* b = new HealpixBlob(fitsfilePath, cdelt1, cdelt2, map_resolution, contour_points, points, photon_points);
      blobs.push_back(b);
    }
  }

  cout << "Number of blobs found: " << blobs.size() << endl;

  if(save_cv_steps)
  {
    Healpix_Map <double> blobs_map = compute_blobs_map(map_resolution, blobs);
    save_healpix_INT_image(output_folder+"/"+fitsfilename+"_"+"blobs.fits",blobs_map);
  }



  return blobs;

}


template<typename T> Healpix_Map<T> HealPixCountMapsBlobsFinder::gaussian_smoothing (double fwhm, Healpix_Map<double> input_map, string outfile )
{

  auto start = std::chrono::system_clock::now();

  int nlmax = 64;
  int num_iter = 0;
  fwhm = arcmin2rad*fwhm;
  if (fwhm<0)
    cout << "NOTE: negative FWHM supplied, doing a deconvolution..." << endl;

  Healpix_Map<T> map = input_map;

  tsize nmod = map.replaceUndefWith0();
  if (nmod!=0)
  cout << "WARNING: replaced " << nmod <<
         " undefined map pixels with a value of 0" << endl;

  double avg=map.average();
  map.Add(T(-avg));

  arr<double> weight;
  weight.alloc(2*map.Nside());
  weight.fill(1);

  Alm<xcomplex<T> > alm(nlmax,nlmax);

  map2alm_iter(map,alm,num_iter,weight);
  smoothWithGauss (alm, fwhm);
  alm2map(alm,map);

  map.Add(T(avg));

  auto stop = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = stop-start;
  cout << "Smoothing execution time: " << diff.count() << " s" << endl;

  return map;

}

Healpix_Map<float> HealPixCountMapsBlobsFinder :: thresholding(Healpix_Map<double> convolved_map, long int nPix, int map_resolution, double classification_threshold)
{
  auto start = std::chrono::system_clock::now();

  float * convolved_data = new float[nPix];

  for( int i = 0; i < nPix; i ++ )
  {
    convolved_data[i] = convolved_map[i];
  }

  /*first create the histogram*/
  int histogram[255];
  for(int i=0;i<256;i++){
    histogram[i]=0;
  }
  for(int i=0;i<nPix;i++){
    histogram[(int)convolved_data[i]]++;
  }


  float mass_prob;
  float percent_acc = 0;
  int index=-1;
  float treshold = -1;

   /* calcolate the treshold as classification_threshold% of all pixels*/
  for(int i=0;i<256;i++){
    mass_prob= (float)histogram[i]/(float)nPix;

    percent_acc+=mass_prob;
    if(percent_acc>=classification_threshold){
      treshold= i;
      break;
    }
  }



  for(int i=0;i<nPix;i++){ /* Suppress the pixel how's intensities are under the threshold  */
    if(convolved_data[i]<=treshold){
      convolved_data[i]=0;
    }
  }

  Healpix_Map<float> thresholded_map(map_resolution,RING); /* copies the data into the Healpix map */

  for(int i=0;i<nPix;i++){
    thresholded_map[i] = convolved_data[i] ;
  }

  delete[] convolved_data;

  auto stop = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = stop-start;
  cout << "Thresholding execution time: " << diff.count() << " s" << endl;
  return thresholded_map;

}


Healpix_Map <double> HealPixCountMapsBlobsFinder :: find_connected_components(Healpix_Map<float> thresholded_map, int map_resolution, vector < vector <int> > & connected_component_indexes)
{
  auto start = std::chrono::system_clock::now();

  /*
  The following code lines suppress infinitesimal values produced
  by gaussian_smoothing and not removed by the threshold process.
  */

  long int thresholded_map_nPix = thresholded_map.Npix();
  for( int i = 0; i < thresholded_map_nPix; i ++ )
  {
    if(thresholded_map[i] < 0.1)
    {
      thresholded_map[i] = 0;
    }
  }


  Healpix_Map<double> labeled_map(map_resolution, RING);

  long int nPix = labeled_map.Npix();
  for( int i = 0; i < nPix; i++ )
  {
    labeled_map[i] = 0;
  }

  int label = 0;
  fix_arr<int,8> neighbors; //holds temporary neighbors
  vector<int> equivalence_class_vector;
  equivalence_class_vector.push_back(0);
  for(int i=0;i<nPix;i++)
  {
    // check if the pixel i is not background and is not yet labelled
    if(thresholded_map[i] > 0 && labeled_map[i] == 0)
    {

      // extract the 8-neighborood of the i-th pixel
      thresholded_map.neighbors(i,neighbors);

      // searching for a label in the neighbourhood
      vector<int> neighbor_labels;


      for(int j=0; j < (int)neighbors.size(); j++)  //  8-connecctivity: j++ ( DON'T WORK WITH 4-connecctivity )
      {
        int neighbor = neighbors[j];
        if(labeled_map[neighbor] > 0)
          neighbor_labels.push_back(labeled_map[neighbor]);
      }
      //print_vector_int("neighbor_labels", neighbor_labels);

      vector<int> neighbor_labels_no_dulpicates;
      for( vector <int> :: iterator it = neighbor_labels.begin(); it < neighbor_labels.end(); ++it)
      {
        int label = *it;
        if(std::find(neighbor_labels_no_dulpicates.begin(), neighbor_labels_no_dulpicates.end(), label) != neighbor_labels_no_dulpicates.end())
        {}else
          neighbor_labels_no_dulpicates.push_back(label);
      }
      //print_vector_int("neighbor_labels_no_dulpicates", neighbor_labels_no_dulpicates);



      // if there is at least one labelled neighbor
      if(neighbor_labels_no_dulpicates.size() > 0)
      {
        // search for minimum label
        int min_l = 999999;
        for(const auto& i : neighbor_labels_no_dulpicates)
          if( i < min_l )
              min_l = i;

        // assign label to pixel i
        labeled_map[i] = min_l;

        // remove min element
        neighbor_labels_no_dulpicates.erase(std::find(neighbor_labels_no_dulpicates.begin(),neighbor_labels_no_dulpicates.end(),min_l));


        // equivalence classes management
        for(const auto& nl : neighbor_labels_no_dulpicates)
        {
          equivalence_class_vector[nl] = equivalence_class_vector[min_l];

          // transitive property implementation
          for( vector <int> :: iterator it = equivalence_class_vector.begin(); it < equivalence_class_vector.end(); ++it)
            if( *it == nl )
              *it = equivalence_class_vector[nl];
        }

      }
      else
      {
        // Creating a new label
        label++;
        equivalence_class_vector.push_back(label);
        labeled_map[i]=label;
      }
    }
  }



  // Resolution of the equivalances

  for( int i = 0; i < nPix; i ++ )
  {
    if( labeled_map[i] > 0 )
    {
      labeled_map[i] = equivalence_class_vector[ labeled_map[i] ];
      // cout << "Il pixel " << i << " di classe "<<labeled_map[i]<<" viene labellizzato come: " << equivalence_class_vector[labeled_map[i]] << endl;
    }
  }

  // Creating vector < vector > > connected_component_indexes -> for each blob , a list of blobs pixels (healpix pixels)

  std::sort(equivalence_class_vector.begin(), equivalence_class_vector.end());
  auto last = std::unique(equivalence_class_vector.begin(), equivalence_class_vector.end());
  equivalence_class_vector.erase(last, equivalence_class_vector.end());
  equivalence_class_vector.erase(equivalence_class_vector.begin());

  // int c = 0;
  // for(const auto& ecv : equivalence_class_vector)
  // {
  //   cout << "equivalence_class_vector["<<c<<"]= "<<ecv << endl;
  //   c++;
  // }


  int max = 0;
  for(const auto& ecv : equivalence_class_vector)
  {
    if(ecv>max)
      max = ecv;
  }
  vector<int> equivalence_classes_incremental_mapping(max, 0);

  int index = 0;
  for(const auto& ecv : equivalence_class_vector)
  {
    equivalence_classes_incremental_mapping[ecv] = index;
    index ++;
  }

  connected_component_indexes.resize(equivalence_class_vector.size());

  for( int i = 0; i < nPix; i ++ )
  {
    if (labeled_map[i] > 0)
      connected_component_indexes[equivalence_classes_incremental_mapping[labeled_map[i]]].push_back(i);
  }


  auto stop = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = stop-start;
  cout << "Connected components execution time: " << diff.count() << " s" << endl;

  return labeled_map;
}


Healpix_Map <double> HealPixCountMapsBlobsFinder :: compute_blobs_map(int map_resolution, vector<Blob *> blobs)
{
  Healpix_Map<double> blobs_map(map_resolution,RING);
  for( int i = 0; i < blobs_map.Npix(); i++ )
  {
    blobs_map[i] = 0;
  }


  for(const auto& b : blobs)
  {
    //cout << "Blob with centroid " << b->get_galactic_centroid_l() << " "<< b->get_galactic_centroid_b() << endl;
    for(const auto& contour_point : b->get_contour())
    {
      //cout << "computing pointing.."<<endl;

      pointing point = pointing((90-contour_point.x)*DEG2RAD, contour_point.y*DEG2RAD);
      int healpix_map_index = blobs_map.ang2pix(point);
      //cout << "y= " << contour_point.y << " x= " << contour_point.x << " index= " << healpix_map_index << endl;
      blobs_map[healpix_map_index] = 1;
    }
    pointing point = pointing((90-b->get_galactic_centroid_b())*DEG2RAD, b->get_galactic_centroid_l()*DEG2RAD);
    int healpix_map_index = blobs_map.ang2pix(point);
    blobs_map[healpix_map_index] = 10;
  }
  return blobs_map;
}


int HealPixCountMapsBlobsFinder :: compute_blob_features(
                                                        int map_resolution,
                                                        vector <int> & connected_component_indexes,
                                                        Healpix_Map<double>& map,
                                                        Healpix_Map<float>& thresholded_map,
                                                        Healpix_Map <double>& labeled_map,
                                                        vector<pair<MapCoords,int> > & points,
                                                        vector<pair<MapCoords,int> > & photon_points,
                                                        vector<MapCoords > & contour_points
                                                       )
{
  Healpix_Map <double> labeledWorkingMap = labeled_map;
  double l, b;

  fix_arr<int,8> neighbors; //holds temporary neighbors
  bool foundPixelContour;

  for(vector < int > ::iterator it = connected_component_indexes.begin(); it < connected_component_indexes.end(); ++it )
  {
    int pixel = *it;
    // cout << "Pixel: "<<pixel<<endl;

    pointing point = labeledWorkingMap.pix2ang(pixel);
    double l = point.phi*RAD2DEG;
    double b = point.theta*RAD2DEG;

    MapCoords cp( l, 90 - b, "gal");

    points.push_back(make_pair(cp, (int)thresholded_map[pixel]));

    /* check if pixel is a photon */
    int greyLevel = map[pixel];
    if ( greyLevel > 0 )
    {
      photon_points.push_back(make_pair(cp,greyLevel));
    }

    /* check if pixel is contour pixel */
    labeledWorkingMap.neighbors(pixel,neighbors);
    foundPixelContour = false;
    // cout<<"Il pixel "<<j<< " ha come vicini: "<<endl;

    for(int k = 0; k < (int)neighbors.size(); k=k+2)
    {

      if( foundPixelContour == false && labeledWorkingMap[neighbors[k]] == 0)
      {
        foundPixelContour = true;

        pointing pointContour = labeledWorkingMap.pix2ang(pixel);
        double l = pointContour.phi*RAD2DEG;
        double b = pointContour.theta*RAD2DEG;

        MapCoords cpContour( l, 90-b, "gal");

        contour_points.push_back(cpContour);

      }
    }
    labeledWorkingMap[pixel] = -100; // Qui setto a -100 il pixel per non riconsiderarlo negli step successivi
  }
}



int HealPixCountMapsBlobsFinder :: save_healpix_INT_image( string imageName, Healpix_Map<double> map)
{

  if( remove( imageName.c_str() ) == 0 )
		cout << "Deleted old file: " << imageName << endl;

   fitshandle handleC = fitshandle() ;
   handleC.create(imageName.c_str());
   write_Healpix_map_to_fits(handleC,map,PLANCK_INT32);
   handleC.set_key("COORDSYS",string("G"),"Ecliptic, Galactic or Celestial (equatorial) ");
   // cout << "Write new file: " << imageName << endl;
}



int HealPixCountMapsBlobsFinder :: save_healpix_FLOAT_image( string imageName, Healpix_Map<float> map)
{

  if( remove( imageName.c_str() ) == 0 )
		cout << "Deleted old file: " << imageName << endl;

   fitshandle handleC = fitshandle() ;
   handleC.create(imageName.c_str());
   write_Healpix_map_to_fits(handleC,map,PLANCK_FLOAT32);
   handleC.set_key("COORDSYS",string("G"),"Ecliptic, Galactic or Celestial (equatorial) ");
   // cout << "Write new file: " << imageName << endl;
}
