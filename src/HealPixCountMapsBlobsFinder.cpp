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

#define MAX_NEIGHBOORS 20

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

/*
double mres = log2( 90 / ( sqrt(2) * cdelt1 ));
int	map_resolution = round(mres);
*/
vector<Blob*> HealPixCountMapsBlobsFinder::find_blobs(string fitsfilename, string fitsfile_folder, bool save_cv_steps, string output_folder)
{

  cout << "[HealPixCountMapsBlobsFinder] Finding blobs.." << endl;

  vector<Blob*> blobs;



  const string fitsfilePath = fitsfile_folder + "/" + fitsfilename;


  Healpix_Map<int> map = read_Healpix_map_from_fits<int>(fitsfilePath);

  int map_resolution = map.Order();
  long int nPix = map.Npix();

  cout << "Map npix: " << nPix << endl;
  cout << "Map order: " << map_resolution << endl;
  cout << "Map schema: " << map.Scheme() << endl;

  // TO TEST !!!!
  if (map.Scheme() == RING)
    map.Set(map_resolution, NEST);


  Healpix_Map<float> convolved_map;
  convolved_map = gassusian_smoothing(map, nPix, map_resolution, psf, cdelt1, cdelt2);

  if(save_cv_steps)
    saveHealpixFLOATImage(output_folder+"/"+fitsfilename+"_"+"convolved_map.fits",convolved_map);


  Healpix_Map<float> thresholded_map;
  thresholded_map = thresholding(convolved_map, nPix, map_resolution);

  if(save_cv_steps)
    saveHealpixFLOATImage(output_folder+"/"+fitsfilename+"_"+"thresholded_map.fits",thresholded_map);


  Healpix_Map <int> labeledMap;
  //vector <pair<int,int > > connectedComponent;
  vector < vector <int> > connected_components;
  labeledMap = find_connected_components(thresholded_map, map_resolution, connected_components);

  if(save_cv_steps)
    saveHealpixINTImage(output_folder+"/"+fitsfilename+"_"+"labelelled_map.fits",labeledMap);


  if(save_cv_steps){
    vector < vector<MapCoords> > contour_image;
    Healpix_Map <int> contourToPrintMap(map_resolution,NEST);
    contourToPrintMap = healpixFindContour(labeledMap, map_resolution, &contour_image);
    saveHealpixINTImage(output_folder+"/"+fitsfilename+"_"+"contour.fits",contourToPrintMap);
  }


  //Compute number of pixels, number of photons, contours
  // vector < IDENTIFIER,
  /*
  vector <
        pair < int, pair <
                            pair < vector < pair < MapCoords, int > >, vector < pair < MapCoords, int > > > , vector<MapCoords > >

        > > allBlobs;

  computeBlobFeatures(map, thresholded_map, labeledMap, map_resolution, allBlobs);

  // Blobs extraction
  vector < pair < int, pair < pair < vector <pair < MapCoords, int > >, vector < pair < MapCoords, int> > > , vector<MapCoords > > > > :: iterator it;
  */


  for(vector < vector <int> >:: iterator it = connected_components.begin(); it < connected_components.end(); ++it){

    vector<pair<MapCoords,int> > points;
    vector<pair<MapCoords,int> > photon_points;
    vector<MapCoords > contour_points;

    computeBlobFeatures( map_resolution, *it, map, thresholded_map, labeledMap, points, photon_points, contour_points);




      // Creating a Blob
      // The following condition (photonsInBlobs.size() >= 2) is needed
      // to avoid the photon closeness problem:
      // a blob made by 1 photon has a photonCloseness = 1 (the minimum).
      // In this context, both a background blob and a flux blob has the
      // same feature. In addition, there are a lot of background blobs
      // made by only 1 photon and this can alter the photon closeness distribution.

    if( photon_points.size() >= 2) {
      // cout << "Il blob con la label "<<currentBlob.first<< ": ";

      Blob* b = new HealpixBlob(fitsfilePath, cdelt1, cdelt2, map_resolution, contour_points, points, photon_points);
      blobs.push_back(b);
    }

    // DEBUG print
    // cout << "Il blob con la label "<<currentBlob.first<<" contiene "<< points.size()<<" pixel e il suo contorno è dato da: "<<endl;
    //
    // for( vector<MapCoords > :: iterator l = contour_points.begin(); l < contour_points.end(); l++)
    // {
    //   MapCoords c = *l;
    //   cout << c.x<< " "<<c.y<<" /";
    // }
    // cout << endl;

  }

  return blobs;

}


Healpix_Map<float> HealPixCountMapsBlobsFinder :: gassusian_smoothing(Healpix_Map<int> map, int nPix, int map_resolution, float psf, float cdelt1, float cdelt2)
{

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
  int kernel_side = round( (2 * psf/cdelt1) + 1);
  cout << "Kernel size: " << kernel_side << endl;

  float ** kernel_generated;
  kernel_generated = filterCreation(kernel_side);

  float kernel_reordered_Array[MAX_NEIGHBOORS][8*MAX_NEIGHBOORS];

  int lim = ( kernel_side / 2.0 ) - 1; //counts how many neighborhoods must be extracted
  cout <<"Neighbours to be extract: " << lim << endl;

	/*Reorder the kernel according to neighborhoods representation */
	int centre = kernel_side / 2.0;
  cout << "Kernel center: " << centre << endl;


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



	fix_arr<int,81> arr[MAX_NEIGHBOORS]; // array for holding the (i+1)-neighborood
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


  return convolved_map;

}

float ** HealPixCountMapsBlobsFinder :: filterCreation(int kernel_side)
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

  float thresh=0.999;
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
   /* calcolate the treshold as 99% of all pixels*/
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


  return thresholded_map;

}


Healpix_Map <int> HealPixCountMapsBlobsFinder :: find_connected_components(Healpix_Map<float> thresholded_map, int map_resolution, vector < vector <int> > & connected_component_indexes)
{

  long int nPix = thresholded_map.Npix();

  Healpix_Map<int> labeledMap(map_resolution,NEST);

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
    //cout << "Labeling pixel = " << i << endl;
    // check if the pixel i is not background and is not yet labelled
    if(thresholded_map[i] > 0 && labeledMap[i] == 0)
    {

      // extract the 8-neighborood of the i-th pixel
      thresholded_map.neighbors(i,neighbors);

      // searching for a label in the neighbourhood
      vector<int> neighbor_labels;
      vector<int> current_neighbors;

      for(int j=0; j < (int)neighbors.size(); j++)
      {
        int neighbor = neighbors[j];
        if(labeledMap[neighbor] > 0)
        {
          neighbor_labels.push_back(labeledMap[neighbor]);
          current_neighbors.push_back(neighbor);
        }
      }


      /*
      cout << "Neighbours: ";
      for(const auto& i : current_neighbors)
        cout << i << " ";
      cout << endl;

      cout << "Neighbours labels: ";
      for(const auto& i : neighbor_labels)
        cout << i << " ";
      cout << endl;
      */

      // if there is at least one labelled neighbor
      if(neighbor_labels.size() > 0)
      {
        // search for minimum label
        int min_l = 100000;
        for(const auto& i : neighbor_labels)
          if( i < min_l )
              min_l = i;

        // assign label to pixel i
        labeledMap[i] = min_l;

        // remove min element
        neighbor_labels.erase(std::find(neighbor_labels.begin(),neighbor_labels.end(),min_l));

        /*
        cout << "Neighbours labels without minimum: ";
        for(const auto& i : neighbor_labels)
          cout << i << " ";
        cout << endl;


        if ( std::adjacent_find( neighbor_labels.begin(), neighbor_labels.end(), std::not_equal_to<int>() ) == neighbor_labels.end() )
        {
        //    std::cout << "All elements are equal each other" << std::endl;
        }
        else{
          //getchar();
        }


        int ccount = 0;
        cout << "Equivalence class vector:" <<endl;
        for(const auto& ecv : equivalence_class_vector)
        {
          cout << "vector["<<ccount<<"]="<<ecv<<endl;
          ccount++;
        }
        cout << endl;
        */

        // gestire equivalenze => class 'c' must become 'min_l'
        // optimization => dont compare labels that are equals
        for(const auto& nl : neighbor_labels)
        {
          equivalence_class_vector[nl] = min_l;

          for( vector <int> :: iterator it = equivalence_class_vector.begin(); it < equivalence_class_vector.end(); ++it)
          {
            if( *it == nl ){
              *it = equivalence_class_vector[min_l];
            }
          }
        }


        /*
        ccount = 0;
        cout << "Equivalence class vector:" <<endl;
        for(const auto& ecv : equivalence_class_vector)
        {
          cout << "vector["<<ccount<<"]="<<ecv<<endl;
          ccount++;
        }
        cout << endl;

        if ( std::adjacent_find( neighbor_labels.begin(), neighbor_labels.end(), std::not_equal_to<int>() ) == neighbor_labels.end() )
        {
        //    std::cout << "All elements are equal each other" << std::endl;
        }
        else{
          //getchar();
        }
        */

      }
      else
      {
        // Creating a new label
        label++;
        equivalence_class_vector.push_back(label);
        labeledMap[i]=label;
        cout << "No labels exist in the neighborhood, new label = " << label << endl;
          //getchar();

      }
    }
  }

  /*
  int count = 0;
  for( vector <int> :: iterator it = equivalence_class_vector.begin(); it < equivalence_class_vector.end(); ++it)
  {
    cout << "class["<<count<<"] = "<<*it << endl;
    count ++;
  }
  */
  int count = 0;
  vector<int> no_duplicates;
  for( vector <int> :: iterator it = equivalence_class_vector.begin(); it < equivalence_class_vector.end(); ++it)
  {
    int label = *it;
    if(std::find(no_duplicates.begin(), no_duplicates.end(), label) != no_duplicates.end()) {
      /* v contains x */
    } else {
      count ++ ;
      no_duplicates.push_back(label);
    }
  }
  cout << "Count: " << count - 1 << endl;




  // Resolution of the equivalances
  for( int i = 0; i < nPix; i ++ )
  {
    if( labeledMap[i] > 0 )
    {
      // cout << "labeledMap[i]: " << labeledMap[i] << endl;
      labeledMap[i] = equivalence_class_vector[ labeledMap[i] ];
      // cout << "Il pixel " << i << " viene labellizzato come: " << equivalence_class_vector[labeledMap[i]]<< endl;
    }
  }




  int c = 0;
  for(const auto& ecv : equivalence_class_vector)
  {
    cout << "equivalence_class_vector["<<c<<"]= "<<ecv << endl;
    c++;
  }

  std::sort(equivalence_class_vector.begin(), equivalence_class_vector.end());
  auto last = std::unique(equivalence_class_vector.begin(), equivalence_class_vector.end());
  equivalence_class_vector.erase(last, equivalence_class_vector.end());
  equivalence_class_vector.erase(equivalence_class_vector.begin());

  c = 0;
  for(const auto& ecv : equivalence_class_vector)
  {
    cout << "(erased) equivalence_class_vector["<<c<<"]= "<<ecv << endl;
    c++;
  }


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

  c = 0;
  for(const auto& ecv : equivalence_classes_incremental_mapping)
  {
    cout << "equivalence_classes_incremental_mapping["<<c<<"]= "<<ecv << endl;
    c++;
  }





  connected_component_indexes.resize(equivalence_class_vector.size());

  index = 0;
  for( int i = 0; i < nPix; i ++ )
  {
    if (labeledMap[i] > 0)
      connected_component_indexes[equivalence_classes_incremental_mapping[labeledMap[i]]].push_back(i);
  }


  /*
  c = 0;
  for(const auto& cci : connected_component_indexes)
  {
    cout << "Blob "<<c << endl;
    for(const auto& ccii : cci)
      cout << ccii << " ";
    cout << endl;
    c++;
  }
  */
  cout << labeledMap[21326] << endl;



  return labeledMap;
}


int HealPixCountMapsBlobsFinder :: saveHealpixINTImage( string imageName, Healpix_Map<int> map)
{

  if( remove( imageName.c_str() ) == 0 )
		cout << "Deleted old file: " << imageName << endl;

   fitshandle handleC = fitshandle() ;
   handleC.create(imageName.c_str());
   write_Healpix_map_to_fits(handleC,map,PLANCK_INT32);
   handleC.set_key("COORDSYS",string("G"),"Ecliptic, Galactic or Celestial (equatorial) ");
   // cout << "Write new file: " << imageName << endl;
}



int HealPixCountMapsBlobsFinder :: saveHealpixFLOATImage( string imageName, Healpix_Map<float> map)
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

int HealPixCountMapsBlobsFinder :: computeBlobFeatures(
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


  pair < int,  // id
         pair < pair < vector <pair < MapCoords, int > >,       // points
                       vector < pair < MapCoords, int> > > ,    // photons
                vector<MapCoords > > // contour
  > labelNpixelAndContourBlob_tmp;



  pair < pair < vector <pair < MapCoords, int > >, vector < pair < MapCoords, int> > > , vector<MapCoords > > pixelPhotonsAndContour;

  pair < vector < pair < MapCoords, int > >, vector < pair < MapCoords, int> > >  pixelAndPhotons;

  double l, b;

  fix_arr<int,8> neighbors; //holds temporary neighbors
  bool foundPixelContour;

  for( int i = 0; i < labeledWorkingMap.Npix(); i++ )
  {
    if( labeledWorkingMap[i] > 0)
    {
      foundPixelContour = false;
      pixelAndPhotons.first.clear();
      pixelAndPhotons.second.clear();
      pixelPhotonsAndContour.second.clear();
      labelNpixelAndContourBlob_tmp.first = labeledWorkingMap[i]; // la label del blob diventa l'ID del blob

      for( int j = 0; j < labeledWorkingMap.Npix(); j++ )
      {
        if(labeledWorkingMap[j] == labelNpixelAndContourBlob_tmp.first )
        {

          pointing point = labeledWorkingMap.pix2ang(j);
          double l = point.phi*RAD2DEG;
          double b = point.theta*RAD2DEG;


          MapCoords cp( l, 90 - b, "gal");

          pixelAndPhotons.first.push_back(make_pair(cp, (int)thresholded_map[j]));

          /* check if pixel is a photon */
  				int greyLevel = map[j];
  				if ( greyLevel > 0 )
  				{
  					pixelAndPhotons.second.push_back(make_pair(cp,greyLevel));
  				}

          labeledWorkingMap.neighbors(j,neighbors);
          // cout<<"Il pixel "<<j<< " ha come vicini: "<<endl;

          for(int k = 0; k < (int)neighbors.size(); k=k+2)
          {

            if( foundPixelContour == false && labeledWorkingMap[neighbors[k]] == 0)
            {
              foundPixelContour = true;

              pointing pointContour = labeledWorkingMap.pix2ang(j);
              double l = pointContour.phi*RAD2DEG;
              double b = pointContour.theta*RAD2DEG;

              MapCoords cpContour( l, 90-b, "gal");

              pixelPhotonsAndContour.second.push_back(cpContour);

            }
          }
          labeledWorkingMap[j] = -100; // Qui setto a -100 il pixel per non riconsiderarlo negli step successivi
          foundPixelContour = false;
        }
      }
      pixelPhotonsAndContour.first = pixelAndPhotons;


      labelNpixelAndContourBlob_tmp.second = pixelPhotonsAndContour;
      //allBlobs.push_back(labelNpixelAndContourBlob_tmp);

    }

  }
}


Healpix_Map <int> HealPixCountMapsBlobsFinder :: healpixFindContour(Healpix_Map <int> labeledMap, int map_resolution, vector < vector<MapCoords> > * contour_image)
{
  cout << "HealPixCountMapsBlobsFinder"<<endl;
  Healpix_Map <int> contourToPrintMap(map_resolution,NEST);

  Healpix_Map <int> labeledWorkingMap = labeledMap;
  vector<MapCoords>  corrent_contour;

  for(int i= 0; i < contourToPrintMap.Npix(); i++)
  {
    contourToPrintMap[i]=0;
  }

  double l, b;

  fix_arr<int,8> neighbors; //holds temporary neighbors
  bool foundPixelContour;

  for( int i = 0; i < labeledWorkingMap.Npix(); i++ )
  {
    if( labeledWorkingMap[i] > 0)
    {
      foundPixelContour = false;

      corrent_contour.clear();
      int idLabel = labeledWorkingMap[i]; // la label del blob diventa l'ID del blob

      for( int j = 0; j < labeledWorkingMap.Npix(); j++ )
      {
        if(labeledWorkingMap[j] == idLabel )
        {
          labeledWorkingMap.neighbors(j,neighbors);
          // cout<<"Il pixel "<<j<< " ha come vicini: "<<endl;

          for(int k = 0; k < (int)neighbors.size(); k=k+2)
          {

            if( foundPixelContour == false && labeledWorkingMap[neighbors[k]] == 0)
            {
              foundPixelContour = true;
              contourToPrintMap[j]=1;

              pointing pointContour = labeledWorkingMap.pix2ang(j);
              double l = pointContour.phi*RAD2DEG;
              double b = pointContour.theta*RAD2DEG;
              MapCoords cpContour( l, b, "gal");
              corrent_contour.push_back(cpContour);
            }
          }
          labeledWorkingMap[j] = -100; // Qui setto a -100 il pixel per non riconsiderarlo negli step successivi
          foundPixelContour = false;
        }
      }

      contour_image->push_back(corrent_contour);

    }

  }

  return contourToPrintMap;

}
