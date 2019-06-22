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


#include "HealpixBlob.h"


HealpixBlob :: HealpixBlob(string filepath, float _cdelt1, float _cdelt2, vector<MapCoords > & contour_points, vector<pair<MapCoords,int> > & points, vector<pair<MapCoords,int> > & photon_points)
                          : Blob(contour_points, points, photon_points)
                          , cdelt1(_cdelt1)
                          , cdelt2(_cdelt2)
{
  // cout << "HealpixBlob"<<endl;

  // // additional features
  centroid = compute_centroid();
  // img_centroid = compute_img_centroid();
  // photons_closeness = compute_photons_closeness();
  blob_area_deg = compute_blobs_area_degrees();
}

string HealpixBlob::to_json_str(string filepath)
{
  // take these from the parent
  rapidjson::Value& json_meta = get_json_meta();
  rapidjson::Value& json_features = get_json_features();
}


MapCoords HealpixBlob :: compute_centroid()
{
  double sumX=0;
  double sumY=0;
  // cout << "Contour size: "<<blob_contour_points.size()<< endl;
  for(vector<MapCoords>::iterator l = blob_contour_points.begin(); l < blob_contour_points.end(); l++){
    MapCoords p = *l;
    sumX+=p.x;
    sumY+=p.y;
  }
  MapCoords cp( sumY/blob_contour_points.size(), sumX/blob_contour_points.size(), "gal" );
  cout << "Centroid: "<<cp.x<<", "<<cp.y<<endl;

  return cp;
}

float HealpixBlob :: compute_blobs_area_degrees(){


  //// TO CHECK
  // Al posto di 7 passare mresRound

  float pixarea = ( ( 4 * M_PI / 12 * pow(7,2) )*RAD2DEG )*RAD2DEG;  //def nside2pixarea(nside, degrees=False) https://github.com/healpy/healpy/blob/master/healpy/pixelfunc.py

  cout << "Area of Blob: "<<pixarea*number_of_pixels<<endl; // number_of_pixels ?????????
  return pixarea*number_of_pixels;
}
//
//
// float HealpixBlob :: compute_photons_closeness(){
//   cout << "ciao" << endl;
// }
// MapCoords HealpixBlob :: compute_img_centroid(){
//   cout << "ciao" << endl;
// }
