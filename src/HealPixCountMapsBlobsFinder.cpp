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

#define MAX_NEIGHBOORS 20

HealPixCountMapsBlobsFinder::HealPixCountMapsBlobsFinder(float _cdelt1, float _cdelt2, float _psf) : BlobsFinder(_cdelt1, _cdelt2, _psf){

  cout << "HealPixCountMapsBlobsFinder" << endl;
  file_format = "HEALPIX COUNT MAP";

}

string HealPixCountMapsBlobsFinder::get_format()
{
		return file_format;
}

///////////////////////////////////////////////////////////////////////////////
//
//		Extraction of blobs from FITS HEALPix images
//

vector<Blob*> HealPixCountMapsBlobsFinder::find_blobs(string fitsfilePath, bool debug, bool save_cv_steps, string output_folder){

  double mres;
  int mresRound;

  cout << "[HealPixCountMapsBlobsFinder] FIND BLOBS FUNCTION" << endl;

  vector<Blob*> blobs;

  mres = log2( 90 / ( sqrt(2) * cdelt1 ));

	mresRound = round(mres);
  cout << "mres value: " << mres << endl;
  cout << "mres round value: " << mresRound << endl;

  int status = 0;
	/*Create a grey-scale image from the counting just done*/
  Healpix_Map<int> map(mresRound,NEST); // NEST is chosen for seek of efficency

	read_Healpix_map_from_fits(fitsfilePath.c_str(), map);
  long int nPix = map.Npix();

  Healpix_Map<float> convolved_map;
  convolved_map = gassusianSmoothing(map, nPix, mresRound, psf, cdelt1, cdelt2, debug);

  saveHealpixFLOATImage("./convolved_map.fits",convolved_map);


  Healpix_Map<float> thresholded_map;
  thresholded_map = thresholding(convolved_map, nPix, mresRound);

  saveHealpixFLOATImage("./thresholded_map.fits",thresholded_map);


  Healpix_Map <int> labeledMap;
  vector <pair<int,int>> connectedComponent;
  labeledMap= findConnectedComponent(thresholded_map, mresRound, &connectedComponent);

  saveHealpixINTImage("./labelelled_map.fits",labeledMap);

  vector < vector<MapCoords> > contour_image;
  healpixFindContour(labeledMap, mresRound, &contour_image);


  //Compute number of pixels and contours of Blobs
  vector < pair < int, pair < pair < vector <pair < MapCoords, int > >, vector < pair < MapCoords, int> > > , vector<MapCoords > > > > allBlobs;

  computeBlobFeatures(map, thresholded_map,labeledMap, mresRound, &allBlobs);//pixelsOfBlobs, photonsInBlobs, contour_points,
  //

  // Blobs extraction
  vector < pair < int, pair < pair < vector <pair < MapCoords, int > >, vector < pair < MapCoords, int> > > , vector<MapCoords > > > > :: iterator it;

  for( it = allBlobs.begin(); it < allBlobs.end(); it ++ ){

    pair < int, pair < pair < vector <pair < MapCoords, int > >, vector < pair < MapCoords, int> > > , vector<MapCoords > > >  currentBlob = *it;

    vector<pair<MapCoords,int> > photon_points = currentBlob.second.first.first; // PIXEL

    vector<pair<MapCoords,int> > points = currentBlob.second.first.second; //PHOTONS

    vector<MapCoords > contour_points = currentBlob.second.second; //CONTORNO

    /* Creating a Blob
      The following condition (photonsInBlobs.size() >= 2) is needed
      to avoid the photon closeness problem:
      a blob made by 1 photon has a photonCloseness = 1 (the minimum).
      In this context, both a background blob and a flux blob has the
      same feature. In addition, there are a lot of background blobs
      made by only 1 photon and this can alter the photon closeness distribution.
    */
    if( photon_points.size() >= 2) {
      // cout << "Il blob con la label "<<currentBlob.first<< ": ";

      Blob* b = new HealpixBlob(fitsfilePath, cdelt1, cdelt2, contour_points, points, photon_points);
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

}


Healpix_Map<float> HealPixCountMapsBlobsFinder :: gassusianSmoothing(Healpix_Map<int> map, int nPix, int mresRound, float psf, float cdelt1, float cdelt2, bool debug)
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

	float kernel_side = 19;
  // int kernel_side = round((2 * psf/cdelt1) + 1);
  cout << "Kernel size: " << kernel_side << endl;

  float ** kernel_generated;
  kernel_generated = filterCreation(19);

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


  Healpix_Map<float> convolved_map(mresRound,NEST); /* copies the data into the Healpix map */


  for(int i=0;i<nPix;i++){
    // cout << "i: " << i;
    convolved_map[i]= convolved_data[i];
  }


  return convolved_map;

}


Healpix_Map<float> HealPixCountMapsBlobsFinder :: thresholding(Healpix_Map<float> convolved_map, long int nPix, int mresRound){

  float thresh=0.98;
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

  #ifdef DEBUG
      cout << "Ending evaluation" << endl;
  #endif


  for(int i=0;i<nPix;i++){ /* Suppress the pixel how's intensities are under the threshold  */
    if(convolved_data[i]<=treshold){
      convolved_data[i]=0;
    }
  }

  Healpix_Map<float> thresholded_map(mresRound,NEST); /* copies the data into the Healpix map */

  for(int i=0;i<nPix;i++){
    // cout << "i: " << i;
    thresholded_map[i] = convolved_data[i] ;
  }


  return thresholded_map;

}

float ** HealPixCountMapsBlobsFinder :: filterCreation(int kernel_side) {

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


Healpix_Map <int> HealPixCountMapsBlobsFinder :: findConnectedComponent(Healpix_Map<float> thresholded_map, int mresRound, vector <pair<int,int>> * connectedComponent) {

  long int nPix = thresholded_map.Npix();

  Healpix_Map<int> labeledMap(mresRound,NEST);

  for( int i = 0; i < nPix; i++ )
  {
    labeledMap[i] = 0;
  }


  fix_arr<int,8> neighbors; //holds temporary neighbors

  int label = 0;
  bool found_label = false;


  /*   NEW ALGORITHM */

  vector<int> equivalenceClassVector;
  equivalenceClassVector.push_back(0);
  // cout << "equivalenceClassVector["<<0<<"] = " << equivalenceClassVector[0] << endl;


  for(int i=0;i<nPix;i++)
  {

    if(thresholded_map[i] > 0) // Check if the pixel i is labeled
    {

      thresholded_map.neighbors(i,neighbors);//extract the 8-neighborood of the i-th pixel

      if(labeledMap[i]==0)
      {

        // cout <<"Guardo i suoi " << neighbors.size() << " vicini: ";
        // for(int j=0;j<neighbors.size();j++)
        // {
        //   cout << neighbors[j] << ", ";
        // }
        // cout << endl;

        found_label = false;

        int tempLabel=0;
        for(int j=0; j<(int)neighbors.size(); j++)
        {

          if(labeledMap[neighbors[j]]>0) {

            // cout << "Pixel: "<<i<<", la label del pixel vicino: "<<neighbors[j]<< " vale : "<<labeledMap[neighbors[j]]<<endl;

            found_label = true;

            if(equivalenceClassVector[tempLabel]!=equivalenceClassVector[labeledMap[neighbors[j]]] && tempLabel>0)
            {

              for( vector <int> :: iterator it = equivalenceClassVector.begin(); it < equivalenceClassVector.end(); ++it)
              {

                int currentclass = *it;

                if(currentclass == 0)
                {

                  // DO NOTHING

                }
                else if( currentclass == equivalenceClassVector[tempLabel])
                {

                  int temp = equivalenceClassVector[currentclass];

                  equivalenceClassVector[currentclass] = equivalenceClassVector[labeledMap[neighbors[j]]];

                  // Iterazione utilizzata al fine di implementare proprietà transitiva
                  for( vector <int> :: iterator it = equivalenceClassVector.begin(); it < equivalenceClassVector.end(); ++it)
                  {
                    if(*it==temp){
                      *it=equivalenceClassVector[labeledMap[neighbors[j]]];
                    }
                  }

                  labeledMap[i] = labeledMap[neighbors[j]];
                  tempLabel=labeledMap[neighbors[j]];


                }

              }

            }

            tempLabel=labeledMap[neighbors[j]];
            labeledMap[i] = labeledMap[neighbors[j]];
          }
        }

        if(!found_label)
        {

          label++;

          equivalenceClassVector.push_back(label);

          labeledMap[i]=label;

        }


      }
      else
      {

        // cout <<"Il pixel " << i << " è labellizzato" << endl;

      }

    }
  }


  for( int i = 0; i < nPix; i ++ )
  {

    if( labeledMap[i] > 0)
    {

      // cout << "labeledMap[i]: " << labeledMap[i] << endl;
      labeledMap[i] = equivalenceClassVector[labeledMap[i]];
      // cout << "Il pixel " << i << " viene labellizzato come: " << equivalenceClassVector[labeledMap[i]]<< endl;
      // getchar();

    }

  }

  // ***** FOR DEBUG *****

  // vector <pair<int,int>> connectedComponent;
  int id = 0;

  std::sort(equivalenceClassVector.begin(), equivalenceClassVector.end());
  auto last = std::unique(equivalenceClassVector.begin(), equivalenceClassVector.end());
  equivalenceClassVector.erase(last, equivalenceClassVector.end());
  equivalenceClassVector.erase(equivalenceClassVector.begin());
  for (const auto& i : equivalenceClassVector)
  {
    id++;
    connectedComponent->push_back(std::make_pair(id,i));
    // std::cout << i << " ";
  }

  //
  // for(vector <pair<int,int>> ::iterator it = connectedComponent.begin(); it<connectedComponent.end(); it++)
  // {
  //   pair<int,int> current = *it;
  //   cout << "Il blob " << current.first<< " ha una label " << current.second<< endl;
  // }
  // getchar();

  // ***** ***** ***** *****

  return labeledMap;

}


int HealPixCountMapsBlobsFinder :: saveHealpixINTImage( string imageName, Healpix_Map<int> map){

  if( remove( imageName.c_str() ) == 0 )
		cout << "Deleted old file: " << imageName << endl;

   fitshandle handleC = fitshandle() ;
   handleC.create(imageName.c_str());
   write_Healpix_map_to_fits(handleC,map,PLANCK_INT32);
   handleC.set_key("COORDSYS",string("G"),"Ecliptic, Galactic or Celestial (equatorial) ");
   // cout << "Write new file: " << imageName << endl;
}



int HealPixCountMapsBlobsFinder :: saveHealpixFLOATImage( string imageName, Healpix_Map<float> map){

  if( remove( imageName.c_str() ) == 0 )
		cout << "Deleted old file: " << imageName << endl;

   fitshandle handleC = fitshandle() ;
   handleC.create(imageName.c_str());
   write_Healpix_map_to_fits(handleC,map,PLANCK_FLOAT32);
   handleC.set_key("COORDSYS",string("G"),"Ecliptic, Galactic or Celestial (equatorial) ");
   // cout << "Write new file: " << imageName << endl;
}




/// NEW ALGORITHMS

int HealPixCountMapsBlobsFinder :: computeBlobFeatures(Healpix_Map<int> map, Healpix_Map<float> thresholded_map, Healpix_Map <int> labeledMap, int mresRound, vector < pair < int, pair < pair < vector <pair < MapCoords, int > >, vector < pair < MapCoords, int> > > , vector<MapCoords > > > > * allBlobs){


  Healpix_Map <int> labeledWorkingMap = labeledMap;
  Healpix_Map <int> contourToPrintMap(mresRound,NEST);

  for(int i= 0; i < contourToPrintMap.Npix(); i++)
  {
    contourToPrintMap[i]=0;
  }


  pair < int, pair < pair < vector <pair < MapCoords, int > >, vector < pair < MapCoords, int> > > , vector<MapCoords > > > labelNpixelAndContourBlob;
  pair < pair < vector <pair < MapCoords, int > >, vector < pair < MapCoords, int> > > , vector<MapCoords > > pixelPhotonsAndContour;
  pair < vector < pair < MapCoords, int > >, vector < pair < MapCoords, int> > >  pixelAndPhotons;


  int idLabel = 0;

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
      labelNpixelAndContourBlob.first = labeledWorkingMap[i]; // la label del blob diventa l'ID del blob

      for( int j = 0; j < labeledWorkingMap.Npix(); j++ )
      {
        if(labeledWorkingMap[j] == labelNpixelAndContourBlob.first )
        {

          pointing point = labeledWorkingMap.pix2ang(j);
          double l = point.phi*RAD2DEG;
          double b = point.theta*RAD2DEG;


          MapCoords cp( l, 90-b, "gal");

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
              contourToPrintMap[j]=1;


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


      labelNpixelAndContourBlob.second = pixelPhotonsAndContour;
      allBlobs->push_back(labelNpixelAndContourBlob);

    }

  }
}


int HealPixCountMapsBlobsFinder :: healpixFindContour(Healpix_Map <int> labeledMap, int mresRound, vector < vector<MapCoords> > * contour_image)
{
  cout << "HealPixCountMapsBlobsFinder"<<endl;

  Healpix_Map <int> labeledWorkingMap = labeledMap;
  vector<MapCoords>  corrent_contour;
  Healpix_Map <int> contourToPrintMap(mresRound,NEST);

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

  saveHealpixINTImage("./contour.fits",contourToPrintMap);
  return 0;

}
