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

vector<Blob*> HealPixCountMapsBlobsFinder::findBlobs(string fitsfilePath, bool debug){

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
  // cout << "nPix value map before read: " << map.Npix() << endl;
	read_Healpix_map_from_fits(fitsfilePath.c_str(), map);
  long int nPix = map.Npix();
	int max = -1;
  // cout << "nPix value map after read: " << nPix << endl;
	for(int i=0;i<nPix;i++){
		if(map[i]>= max){
			max = (int)map[i];
		}
	}


  // Smoothing
  float convolved_data[nPix];
  gassusianSmoothing(map, convolved_data, nPix, mresRound, max, psf, cdelt1, cdelt2, debug);


  // Thresholding
  Healpix_Map<float> thresholded_data;
  thresholded_data = thresholding(convolved_data, nPix, mresRound);

  // Create Thresholed Map
  fitshandle handleC2 = fitshandle() ;
	handleC2.create("./thresholded_map.fits");
	write_Healpix_map_to_fits(handleC2,thresholded_data,PLANCK_FLOAT32);
  handleC2.set_key("COORDSYS",string("G"),"Ecliptic, Galactic or Celestial (equatorial) ");

  // Find ConnectedComponent
  vector <pair<int,int>> connectedComponent;
  connectedComponent = findConnectedComponent(thresholded_data, mresRound);


  // DEBUGGING PRINT
  for(vector <pair<int,int>> :: iterator it=connectedComponent.begin(); it < connectedComponent.end(); it ++ )
  {
    pair<int,int> current = *it;
    cout << "Il blob " << current.first<< " ha una label " << current.second<< endl;

  }

  //Find contours




}

float * HealPixCountMapsBlobsFinder :: gassusianSmoothing(Healpix_Map<int> map, float * convolved_data, int nPix, int mresRound, float max, float psf, float cdelt1, float cdelt2, bool debug){

  // float convolved_data[nPix];

  bool smooth = true;
 	if(smooth){


 	 float data[nPix];
 		for(int i=0;i<nPix;i++){
 			data[i] = ((float)map[i]*255.0)/(float)max;
 		}
 		for(int i=0;i<nPix;i++){
 				convolved_data[i]=0;
 		}

 		float kernel_side = 19;
    // int kernel_side = round((2 * psf/cdelt1) + 1);
    cout << "Kernel size: " << kernel_side << endl;

    float ** kernel_generated;
    kernel_generated = filterCreation(19);

 	  float kernel_reordered_Array[MAX_NEIGHBOORS][8*MAX_NEIGHBOORS];
 		int lim = (kernel_side/2.0)-1; //counts how many neighborhoods must be extracted
    cout <<"Neighbours to be extract: " << lim << endl;

 		/*Reorder the kernel according to neighborhoods representation */
 		int centre = kernel_side/2.0;
    cout << "Kernel center: " << centre << endl;


 		float k_1 = kernel_generated[centre][centre]; // central element
 		for(int times=0;times<centre;times++){
 			int i=0;
 			int count=0;
 			int cursor_row = centre+(1*times)+2;
 			int cursor_column = centre-(times+1);
 			while(count<3+(2*times)){ // left side
 				cursor_row--;
 				kernel_reordered_Array[times][i]= kernel_generated[cursor_row][cursor_column];
 				i++;
 				count++;
 			}
 			count=0;
 			while(count<2+(2*times)){ //up side
 				cursor_column++;
 				count++;
 				kernel_reordered_Array[times][i]= kernel_generated[cursor_row][cursor_column];
 				i++;
 			}
 			count=0;
 			while(count<2+(2*times)){ // right side
 				cursor_row++;
 				count++;
 				kernel_reordered_Array[times][i]= kernel_generated[cursor_row][cursor_column];
 				i++;
 			}
 			count=0;
 			while(count<1+(2*times)){ // down side
 				cursor_column--;
 				count++;
 				kernel_reordered_Array[times][i]= kernel_generated[cursor_row][cursor_column];
 				i++;
 			}

 		}



 		fix_arr<int,81> arr[MAX_NEIGHBOORS]; // array for holding the (i+1)-neighborood
 		fix_arr<int,8> tempA; //holds temporary neighbors
 		float temp; // accumulator for convolution sums
 		for(int i=0;i<nPix;i++){
 			temp=0;
 			temp +=data[i]*k_1; // product of central elements

 			map.neighbors(i,tempA);//extract the 8-neighborood of the i-th pixel


 			for(int j=0;j<8;j++){

 				arr[0][j]=tempA[j];
        // cout << i << ", " << tempA[j] << endl;
        // getchar();

 			}

 			int z = 0; //used to select neighbors
 			int separator; //indexing dividing arr[times][s] to reflect sides of neighborhood matrix

 			/* if necessary extract the other neighbors */
 			for(int times=0;times<lim;times++){ //if kernel side is 3 there is no need to extract other neighbors
 				separator = 2*(times+1);
 				for(int s=0;s<8+(8*times);s++){
 					if(s==0){//bottom-left corner
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
 			for(int q=0;q<lim+1;q++){
 				for(int k=0;k<8*(q+1);k++){
 					if(arr[q][k]!=-1){
 						temp += data[arr[q][k]]*kernel_reordered_Array[q][(k+4*(q+1))%(8*(q+1))];
 					}
 				}
 			}
 			convolved_data[i] = temp;
 		}
 	}

  Healpix_Map<float> convolved_map(mresRound,NEST); /* copies the data into the Healpix map */


  for(int i=0;i<nPix;i++){
    // cout << "i: " << i;
    convolved_map[i]= convolved_data[i] ;
  }

  fitshandle handleC = fitshandle() ;
	handleC.create("./convolved_map.fits");
	write_Healpix_map_to_fits(handleC,convolved_map,PLANCK_FLOAT32);
  handleC.set_key("COORDSYS",string("G"),"Ecliptic, Galactic or Celestial (equatorial) ");

  return 0;

}

Healpix_Map<float> HealPixCountMapsBlobsFinder :: thresholding(float * convolved_data, long int nPix, int mresRound){

  float thresh=0.98;
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

  Healpix_Map<float> convolved_map(mresRound,NEST); /* copies the data into the Healpix map */


  cout << "Npix new map:" << convolved_map.Npix() << endl;
  cout << "Npix old map: " <<nPix << endl;


  for(int i=0;i<nPix;i++){
    // cout << "i: " << i;
    convolved_map[i]= convolved_data[i] ;
  }


  return convolved_map;

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

vector <pair<int,int>> HealPixCountMapsBlobsFinder :: findConnectedComponent(Healpix_Map<float> thresholded_data, int mresRound) {

  long int nPix = thresholded_data.Npix();

  Healpix_Map<int> labeledMap(mresRound,NEST);
  for(int i=0;i<nPix;i++){
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

    if(thresholded_data[i] > 0)
    {

      // cout << "\nIl pixel " << i << " ha valore maggiore di 0" << endl;

      thresholded_data.neighbors(i,neighbors);//extract the 8-neighborood of the i-th pixel

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
        for(int j=0; j<neighbors.size(); j++)
        {

          if(labeledMap[neighbors[j]]>0) {

            // cout << "Pixel: "<<i<<", la label del pixel vicino: "<<neighbors[j]<< " vale : "<<labeledMap[neighbors[j]]<<endl;


            // cout << "Il vicino " << neighbors[j] << " è labellizzato" << endl;
            found_label = true;

            if(equivalenceClassVector[tempLabel]!=equivalenceClassVector[labeledMap[neighbors[j]]] && tempLabel>0)
            {
              // cout <<equivalenceClassVector[tempLabel]<<" != "<<equivalenceClassVector[labeledMap[neighbors[j]]]<<endl;

              // cout << "La label di un vicino è settata a " << tempLabel<< " è diversa dalla label del vicino "<<j<<" del pixel "<<i<<" : "<< labeledMap[neighbors[j]]<< endl;

              for( vector <int> :: iterator it = equivalenceClassVector.begin(); it < equivalenceClassVector.end(); ++it)
              {

                int currentclass = *it;

                if(currentclass == 0)
                {

                  // DO NOTHING

                }
                else if( currentclass == equivalenceClassVector[tempLabel])
                {
                  // cout << currentclass<<" == "<<equivalenceClassVector[tempLabel]<< endl;

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

          // cout << "Nuova label! Al pixel "<<i<<" viene assegnata la label: " << label;

          equivalenceClassVector.push_back(label);  //equivalenceClassVector[label] = label;

          // cout << ", equivalenceClassVector["<<label<<"] = " << label << endl;
          // cout << ", equivalenceClassVector["<<label<<"] = " << equivalenceClassVector[label] << endl;

          labeledMap[i]=label;

        }


      }
      else
      {

        // cout <<"Il pixel " << i << " è labellizzato" << endl;

      }

    }
  }

  // cout << "Trovati " << label << " labels" << endl;

  // cout <<"Dopo prima scansione abbiamo: " << endl;
  // std::map<int,int> cntLabel;
  // vector <std::map<int,int>> connectedComponent;
  //
  //
  //
  // // Iterate over the vector and store the frequency of each element in map
	// for (auto & elem : equivalenceClassVector)
	// {
	// 	auto result = cntLabel.insert(std::pair<int, int>(elem, 1));
	// 	if (result.second == false)
	// 		result.first->second++;
	// }
  //
  // // Iterate over the map
  // for (auto & elem : cntLabel)
  // {
  //   // If frequency count is greater than 1 then its a duplicate element
  //   // if (elem.second > 1)
  //   // std::map<int,int> tempMap;
  //   // tempMap.first = elem.first;
  //   // tempMap.second = elem.second;
  //   std::cout << elem.first << " :: " << elem.second << std::endl;
  //   // connectedComponent.push_back(elem);
  //   // }
  // }


  for(int i=0;i<nPix;i++) {

    if(labeledMap[i]>0) {

      // cout << "labeledMap[i]: " << labeledMap[i] << endl;
      labeledMap[i] = equivalenceClassVector[labeledMap[i]];
      // cout << "Il pixel " << i << " viene labellizzato come: " << equivalenceClassVector[labeledMap[i]]<< endl;

    }

  }

  fitshandle handleC3 = fitshandle() ;
 handleC3.create("./labelelled_map.fits");
 write_Healpix_map_to_fits(handleC3,labeledMap,PLANCK_INT32);
 handleC3.set_key("COORDSYS",string("G"),"Ecliptic, Galactic or Celestial (equatorial) ");


  vector <pair<int,int>> connectedComponent;
  int id = 0;

  std::sort(equivalenceClassVector.begin(), equivalenceClassVector.end());
  auto last = std::unique(equivalenceClassVector.begin(), equivalenceClassVector.end());
  equivalenceClassVector.erase(last, equivalenceClassVector.end());
  equivalenceClassVector.erase(equivalenceClassVector.begin());
  for (const auto& i : equivalenceClassVector)
  {
    id++;
    connectedComponent.push_back(std::make_pair(id,i));
    // std::cout << i << " ";
  }
  std::cout << "\n";

  // for(vector <pair<int,int>> ::iterator it = connectedComponent.begin(); it<connectedComponent.end(); it++)
  // {
  //   pair<int,int> current = *it;
  //   cout << "Il blob " << current.first<< " ha una label " << current.second<< endl;
  // }


  return connectedComponent;


}
