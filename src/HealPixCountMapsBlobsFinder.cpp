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
#include <algorithm>

#include "HealPixCountMapsBlobsFinder.h"

// #define MAX_NEIGHBOORS 60

HealPixCountMapsBlobsFinder::HealPixCountMapsBlobsFinder(float _cdelt1, float _cdelt2, float _psf)
   : BlobsFinder(_cdelt1, _cdelt2, _psf)
{

  // cout << "HealPixCountMapsBlobsFinder" << endl;
  file_format = "HEALPIX COUNT MAP";

}

string HealPixCountMapsBlobsFinder::get_format()
{
		return file_format;
}

///////////////////////////////////////////////////////////////////////////////
//
//		Extraction of blobs from FITS HEALPix images

vector<Blob*> HealPixCountMapsBlobsFinder::find_blobs(string fitsfilename, string fitsfile_folder, bool save_cv_steps, string output_folder)
{

  cout << "[HealPixCountMapsBlobsFinder] Finding blobs.." << endl;

  vector<Blob*> blobs;

  const string fitsfilePath = fitsfile_folder + "/" + fitsfilename;

  Healpix_Map<int> map = read_Healpix_map_from_fits<int>(fitsfilePath);

  int countPix=0;
  for( int i = 0; i <map.Npix(); i ++)
  {
    if(map[i]==0)
      countPix++;
  }

  if(countPix==map.Npix())
  {
    cout<<"ERROR: Input map empty!"<<endl;
    exit(-1);
  }

  int map_resolution = map.Order();
  long int nPix = map.Npix();

  cout << "Map nPix: " << nPix << endl;
  cout << "Map order: " << map_resolution << endl;
  cout << "Map schema: " << map.Scheme() << endl;

  if (map.Scheme() == RING){

    Healpix_Map<int> mapTemp(map_resolution, NEST);
    int index;

    for( int i = 0; i <map.Npix(); i ++)
    {
      index = map.ring2nest(i);
      mapTemp[index] = map[i];
    }
    map.Set(map_resolution, NEST);
    map = mapTemp;
    // if(save_cv_steps)
    //   save_healpix_INT_image(output_folder+"/"+fitsfilename+"_"+"converted_map.fits",map);
    // cout << "HealpiMap RING converted in HealpixMap NEST"<<endl;

  }


  Healpix_Map<float> convolved_map = gassusian_smoothing(map, nPix, map_resolution, psf, cdelt1, cdelt2);

  if(save_cv_steps)
    save_healpix_FLOAT_image(output_folder+"/"+fitsfilename+"_"+"convolved_map.fits",convolved_map);


  Healpix_Map<float> thresholded_map = thresholding(convolved_map, nPix, map_resolution);

  if(save_cv_steps)
    save_healpix_FLOAT_image(output_folder+"/"+fitsfilename+"_"+"thresholded_map.fits",thresholded_map);


  vector < vector <int> > connected_components;
  Healpix_Map <int> labeledMap = find_connected_components(thresholded_map, map_resolution, connected_components);

  // cout << "Trovati "<< connected_components.size() << " connected components."<< endl;

  if(save_cv_steps)
    save_healpix_INT_image(output_folder+"/"+fitsfilename+"_"+"labelelled_map.fits",labeledMap);



  //Compute number of pixels, number of photons, contour
  for(vector < vector <int> >:: iterator it = connected_components.begin(); it < connected_components.end(); ++it)
  {
    vector <int> pixels = *it;

    vector<pair<MapCoords,int> > points;
    vector<pair<MapCoords,int> > photon_points;
    vector<MapCoords > contour_points;

    compute_blob_features( map_resolution, pixels, map, thresholded_map, labeledMap, points, photon_points, contour_points);


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
    Healpix_Map <int> blobs_map = compute_blobs_map(map_resolution, blobs);
    save_healpix_INT_image(output_folder+"/"+fitsfilename+"_"+"blobs.fits",blobs_map);
  }



  return blobs;

}



Healpix_Map <int> HealPixCountMapsBlobsFinder :: compute_blobs_map(int map_resolution, vector<Blob *> blobs)
{
  Healpix_Map<int> blobs_map(map_resolution,NEST);
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




Healpix_Map<float> HealPixCountMapsBlobsFinder :: gassusian_smoothing(Healpix_Map<int> map, int nPix, int map_resolution, float psf, float cdelt1, float cdelt2)
{
  auto start = std::chrono::system_clock::now();

  // cout <<"Npix: " <<nPix<<endl;

  float convolved_data[nPix];
  int max = -1;


  for( int i = 0; i < nPix; i++ )
  {
		if( map[i] >= max )
    {
			max = (int)map[i];
		}
	}


  float data[nPix];
	for( int i = 0; i < nPix; i++ )
  {
		data[i] = ( (float)map[i] * 255.0) / (float)max;
	}

	for( int i = 0; i < nPix; i++ )
  {
			convolved_data[i]=0;
	}

	// float kernel_side = 19;
  // cout <<"Map resolution: "<<map_resolution<<endl;
  float cdelt = 90 / ( pow(2,map_resolution) * pow(2,0.5) );
  // cout<<"Computed cdelt :"<<cdelt<<endl;
  int kernel_side = round( (2 * psf/cdelt) + 1);
  cout << "Kernel size: " << kernel_side << endl;

  float ** kernel_generated;
  kernel_generated = filter_creation(kernel_side);


  int lim = ( kernel_side / 2.0 ) - 1; //counts how many neighborhoods must be extracted

  // float kernel_reordered_Array[MAX_NEIGHBOORS][8*MAX_NEIGHBOORS];
  float kernel_reordered_Array[lim][8*lim];
  // cout <<"Neighbours to be extract: " << lim << endl;

	/*Reorder the kernel according to neighborhoods representation */
	int centre = kernel_side / 2.0;
  // cout << "Kernel center: " << centre << endl;


	float k_1 = kernel_generated[centre][centre]; // central element

  for( int times = 0; times < centre; times++ )
  {
		int i=0;
		int count=0;
		int cursor_row = centre+(1*times)+2;
		int cursor_column = centre-(times+1);

    while( count < 3 + ( 2 * times ) )   // left side
    {
			cursor_row--;
			kernel_reordered_Array[times][i] = kernel_generated[cursor_row][cursor_column];
			i++;
			count++;
		}
		count=0;

    while( count < 2 + ( 2 * times ) )//up side
    {
			cursor_column++;
			count++;
			kernel_reordered_Array[times][i] = kernel_generated[cursor_row][cursor_column];
			i++;
		}
		count=0;

		while( count < 2 + ( 2 * times ) )   // right side
    {
			cursor_row++;
			count++;
			kernel_reordered_Array[times][i] = kernel_generated[cursor_row][cursor_column];
			i++;
		}
		count=0;

		while( count < 1 + ( 2 * times ) )   // down side
    {
			cursor_column--;
			count++;
			kernel_reordered_Array[times][i] = kernel_generated[cursor_row][cursor_column];
			i++;
		}

	}



	// fix_arr<int,81> arr[MAX_NEIGHBOORS]; // array for holding the (i+1)-neighborood
  fix_arr<int,81> arr[2*lim]; // array for holding the (i+1)-neighborood
	fix_arr<int,8> tempA; //holds temporary neighbors
	float temp; // accumulator for convolution sums
	for( int i = 0; i < nPix; i++ )
  {
		temp=0;
		temp +=data[i]*k_1; // product of central elements

		map.neighbors(i,tempA);//extract the 8-neighborood of the i-th pixel


		for( int j = 0; j < 8; j++ )
    {

			arr[0][j]=tempA[j];
      // cout << i << ", " << tempA[j] << endl;

		}

		int z = 0; //used to select neighbors
		int separator; //indexing dividing arr[times][s] to reflect sides of neighborhood matrix

		/* if necessary extract the other neighbors */
		for( int times = 0; times < lim; times++)  //if kernel side is 3 there is no need to extract other neighbors
    {
			separator = 2*(times+1);
			for( int s = 0; s < 8 + ( 8 * times ); s++)
      {
				if( s == 0 )  //bottom-left corner
        {
					if(arr[times][s]!= -1){
						map.neighbors(arr[times][s],tempA); // 8-neighborhood of arr[s]}
						arr[times+1][s+z]= tempA[z];
						z++;
						arr[times+1][s+z]= tempA[z];
						arr[times+1][s+(15+(8*times))]= tempA[7]; // last neighbor pixel
					}else{ //if arr[times][s] does not exit also his neighbor does not exist
						arr[times+1][s+z]= -1;
						z++;
						arr[times+1][s+z]= -1;
						arr[times+1][s+(15+(8*times))]= -1;
					}
				}else if((s!=0) && (s<separator)){//left side
					if(arr[times][s]!=-1){
						map.neighbors(arr[times][s],tempA); // 8-neighborood of arr[s]
						arr[times+1][s+1]=tempA[z];
					}else{//if arr[times][s] does not exit also his neighbor does not exist
						arr[times+1][s+1]=-1;
					}
				}else if(s==separator){ // top-left corner
					if(arr[times][s]!=-1){
						map.neighbors(arr[times][s],tempA); // 8-neighborood of arr[s]
						arr[times+1][s+z]= tempA[z];
						z++;
						arr[times+1][s+z]= tempA[z];
						z++;
						arr[times+1][s+z]= tempA[z];
					}else{//if arr[times][s] does not exit also his neighbor does not exist
						arr[times+1][s+z]= -1;
						z++;
						arr[times+1][s+z]= -1;
						z++;
						arr[times+1][s+z]= -1;
					}
				}else if((s>separator)&& (s<separator*2)){//up-side
					if(arr[times][s]!=0-1){
						map.neighbors(arr[times][s],tempA); // 8-neighborood of arr[s]
						arr[times+1][s+3]=tempA[z];
					}else{//if arr[times][s] does not exit also his neighbor does not exist
						arr[times+1][s+(times+1)]=-1;
					}
				}else if(s==separator*2){ // top-right corner
					if(arr[times][s]!=-1){
						map.neighbors(arr[times][s],tempA); // 8-neighborood of arr[s]
						arr[times+1][s+z]= tempA[z];
						z++;
						arr[times+1][s+z]= tempA[z];
						z++;
						arr[times+1][s+z]= tempA[z];
					}else{//if arr[times][s] does not exit also his neighbor does not exist
						arr[times+1][s+z]= -1;
						z++;
						arr[times+1][s+z]= -1;
						z++;
						arr[times+1][s+z]= -1;
					}
				}else if((s>separator*2)&& (s<separator*3)){// right side
					if(arr[times][s]!=-1){
						map.neighbors(arr[times][s],tempA); // 8-neighborood of arr[s]
						arr[times+1][s+5]=tempA[z];
					}else{//if arr[times][s] does not exit also his neighbor does not exist
						arr[times+1][s+5]=-1;
					}
				}else if(s==separator*3){ //bottom-right corner
					if(arr[times][s]!=-1){
						map.neighbors(arr[times][s],tempA); // 8-neighborood of arr[s]
						arr[times+1][s+z]= tempA[z];
						z++;
						arr[times+1][s+z]= tempA[z];
						z++;//z=7
						arr[times+1][s+z]= tempA[z];
					}else{//if arr[times][s] does not exit also his neighbor does not exist
						map.neighbors(arr[times][s],tempA); // 8-neighborood of arr[s]
						arr[times+1][s+z]= -1;
						z++;
						arr[times+1][s+z]= -1;
						z++;//z=7
						arr[times+1][s+z]= -1;
					}
				}else if((s>separator*3)&& (s<separator*4)){ // bottom side
					if(arr[times][s]!=-1){
						map.neighbors(arr[times][s],tempA); // 8-neighborood of arr[s]
						arr[times+1][s+7]=tempA[z];
					}else{//if arr[times][s] does not exit also his neighbor does not exist
						arr[times+1][s+7]=-1;
					}
				}
			}
			z=0;
		}



		/*Finally calculates the convolution sum */
		for( int q = 0; q < lim+1 ; q++)
    {
			for( int k = 0; k < 8 *( q+1 ); k++)
      {
				if( arr[q][k] != -1 )
        {
					temp += data[arr[q][k]] * kernel_reordered_Array[q][(k+4*(q+1))%(8*(q+1))];
				}
			}
		}
		convolved_data[i] = temp;
	}


  Healpix_Map<float> convolved_map(map_resolution,NEST); /* copies the data into the Healpix map */


  for(int i=0;i<nPix;i++){
    // cout << "i: " << i;
    convolved_map[i]= convolved_data[i];
  }

  auto stop = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = stop-start;
  cout << "Smoothing execution time: " << diff.count() << " s" << endl;

  return convolved_map;
}

float ** HealPixCountMapsBlobsFinder :: filter_creation(int kernel_side)
{

  // float GKernel[kernel_side][kernel_side];
  // float ** GKernel = new

  float ** GKernel = new float*[kernel_side];

  for(int i = 0; i < kernel_side; ++i){

    GKernel[i] = new float[kernel_side];

  }

  // intialising standard deviation to 1.0
    double sigma = 3.0;
    double r, s = 2.0 * sigma * sigma;

    // sum is for normalization
    double sum = 0.0;

    int x , y;


    x = y = (kernel_side/2);

    // cout << "X and Y value is: " << x << endl;

    // generating kernel_side x kernel_side kernel
    for (int i = -x; i <= x; i++) {

        for (int j = -y; j <= y; j++) {

            r = sqrt(i * i + j * j);

            GKernel[i + x][j + y] = (exp(-(r * r) / s)) / (M_PI * s);

            // cout << GKernel[i + x][j + y] << " ";

            sum += GKernel[i + x][j + y];
        }
        // cout << "\n";
    }

    // normalising the Kernel
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 5; ++j)
            GKernel[i][j] /= sum;

    return GKernel;

}


Healpix_Map<float> HealPixCountMapsBlobsFinder :: thresholding(Healpix_Map<float> convolved_map, long int nPix, int map_resolution)
{

  auto start = std::chrono::system_clock::now();

  float thresh=0.995;
  float convolved_data[nPix];

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

   /* calcolate the treshold as thresh% of all pixels*/
  for(int i=0;i<256;i++){
    mass_prob= (float)histogram[i]/(float)nPix;

    percent_acc+=mass_prob;
    if(percent_acc>=thresh){
      treshold= i;
      break;
    }
  }



  for(int i=0;i<nPix;i++){ /* Suppress the pixel how's intensities are under the threshold  */
    if(convolved_data[i]<=treshold){
      convolved_data[i]=0;
    }
  }

  Healpix_Map<float> thresholded_map(map_resolution,NEST); /* copies the data into the Healpix map */

  for(int i=0;i<nPix;i++){
    // cout << "i: " << i;
    thresholded_map[i] = convolved_data[i] ;
  }

  auto stop = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = stop-start;
  cout << "Thresholding execution time: " << diff.count() << " s" << endl;
  return thresholded_map;

}


Healpix_Map <int> HealPixCountMapsBlobsFinder :: find_connected_components(Healpix_Map<float> thresholded_map, int map_resolution, vector < vector <int> > & connected_component_indexes)
{
  auto start = std::chrono::system_clock::now();

  Healpix_Map<int> labeledMap(map_resolution, NEST);

  long int nPix = labeledMap.Npix();
  for( int i = 0; i < nPix; i++ )
  {
    labeledMap[i] = 0;
  }

  int label = 0;
  fix_arr<int,8> neighbors; //holds temporary neighbors
  vector<int> equivalence_class_vector;
  equivalence_class_vector.push_back(0);
  for(int i=0;i<nPix;i++)
  {
    // check if the pixel i is not background and is not yet labelled
    if(thresholded_map[i] > 0 && labeledMap[i] == 0)
    {

      // extract the 8-neighborood of the i-th pixel
      thresholded_map.neighbors(i,neighbors);

      // searching for a label in the neighbourhood
      vector<int> neighbor_labels;
      // vector<int> current_neighbors;

      for(int j=0; j < (int)neighbors.size(); j++)  //  8-connecctivity: j++ ( DON'T WORK WITH 4-connecctivity )
      {
        int neighbor = neighbors[j];
        if(labeledMap[neighbor] > 0)
        {
          neighbor_labels.push_back(labeledMap[neighbor]);
          // current_neighbors.push_back(neighbor);
        }
      }

      vector<int> neighbor_labels_no_dulpicates;
      int count = 0;
      for( vector <int> :: iterator it = neighbor_labels.begin(); it < neighbor_labels.end(); ++it)
      {
        int label = *it;
        if(std::find(neighbor_labels_no_dulpicates.begin(), neighbor_labels_no_dulpicates.end(), label) != neighbor_labels_no_dulpicates.end())
        {
          /* v contains x */
        } else {
          count ++ ;
          neighbor_labels_no_dulpicates.push_back(label);
        }
      }


      // if there is at least one labelled neighbor
      if(neighbor_labels_no_dulpicates.size() > 0)
      {
        // search for minimum label
        int min_l = 999999;
        for(const auto& i : neighbor_labels_no_dulpicates)
          if( i < min_l )
              min_l = i;

        // assign label to pixel i
        labeledMap[i] = min_l;

        // remove min element
        neighbor_labels_no_dulpicates.erase(std::find(neighbor_labels_no_dulpicates.begin(),neighbor_labels_no_dulpicates.end(),min_l));


        // equivalence classes management
        for(const auto& nl : neighbor_labels_no_dulpicates)
        {
          equivalence_class_vector[nl] = min_l;

          // transitive property implementation
          for( vector <int> :: iterator it = equivalence_class_vector.begin(); it < equivalence_class_vector.end(); ++it)
          {
            if( *it == nl )
            {
              *it = equivalence_class_vector[min_l];
            }
          }
        }
      }
      else
      {
        // Creating a new label
        label++;
        equivalence_class_vector.push_back(label);
        labeledMap[i]=label;
      }
    }
  }

  // Resolution of the equivalances
  for( int i = 0; i < nPix; i ++ )
  {
    if( labeledMap[i] > 0 )
    {
      labeledMap[i] = equivalence_class_vector[ labeledMap[i] ];
      // cout << "Il pixel " << i << " viene labellizzato come: " << equivalence_class_vector[labeledMap[i]]<< endl;
    }
  }

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
    if (labeledMap[i] > 0)
      connected_component_indexes[equivalence_classes_incremental_mapping[labeledMap[i]]].push_back(i);
  }


  auto stop = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = stop-start;
  cout << "Connected components execution time: " << diff.count() << " s" << endl;

  return labeledMap;
}


int HealPixCountMapsBlobsFinder :: save_healpix_INT_image( string imageName, Healpix_Map<int> map)
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




/// NEW ALGORITHMS


int HealPixCountMapsBlobsFinder :: compute_blob_features(
                                                        int map_resolution,
                                                        vector <int> & connected_component_indexes,
                                                        Healpix_Map<int>& map,
                                                        Healpix_Map<float>& thresholded_map,
                                                        Healpix_Map <int>& labeledMap,
                                                        vector<pair<MapCoords,int> > & points,
                                                        vector<pair<MapCoords,int> > & photon_points,
                                                        vector<MapCoords > & contour_points
                                                       )
{
  Healpix_Map <int> labeledWorkingMap = labeledMap;
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
    // foundPixelContour = false;
  }
}
