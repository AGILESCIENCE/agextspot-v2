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


HealpixBlob :: HealpixBlob(string filepath, float _cdelt1, float _cdelt2, int _map_resolution, vector<MapCoords > & contour_points, vector<pair<MapCoords,int> > & points, vector<pair<MapCoords,int> > & photon_points)
                          : Blob(contour_points, points, photon_points)
                          , cdelt1(_cdelt1)
                          , cdelt2(_cdelt2)
                          , map_resolution(_map_resolution)
{

  // additional features
  centroid = compute_centroid();
  photons_closeness = compute_photons_closeness();
  blob_area_deg = compute_blobs_area_degrees();

  compute_morphology_measures();

}

string HealpixBlob::to_json_str(string filepath)
{
  // take these from the parent
  rapidjson::Value& json_meta = get_json_meta();
  rapidjson::Value& json_features = get_json_features();

  // adding custom meta
  rapidjson::Value f_p(rapidjson::StringRef(filepath.c_str()));
	json_meta.AddMember("file_path", f_p, allocator);

	rapidjson::Value cd_1(cdelt1);
	json_meta.AddMember("cdelt1", cd_1, allocator);

	rapidjson::Value cd_2(cdelt2);
	json_meta.AddMember("cdelt2", cd_2, allocator);

  rapidjson::Value mres(map_resolution);
	json_meta.AddMember("mres", mres, allocator);

	// rapidjson::Value p(psf);
	// json_meta.AddMember("psf", p, allocator);
  //
	// rapidjson::Value kss(kernel_side_size);
	// json_meta.AddMember("gauss_kernel_side_size", kss, allocator);


  rapidjson::Value pix_centr(rapidjson::kObjectType);
	rapidjson::Value pix_centr_x(img_centroid.x);
	rapidjson::Value pix_centr_y(img_centroid.y);
	pix_centr.AddMember("x", pix_centr_x, allocator);
	pix_centr.AddMember("y", pix_centr_y, allocator);
	json_meta.AddMember("pixel_centroid", pix_centr, allocator);


  // adding custom features
	rapidjson::Value p_clos(photons_closeness);
	json_features.AddMember("photons_closeness", p_clos, allocator);

  return _to_json_str();
}


MapCoords HealpixBlob :: compute_centroid()
{
  double sumX=0;
  double sumY=0;
  // cout << "Contour size: "<<blob_contour_points.size()<< endl;
  for(vector<MapCoords>::iterator l = blob_contour_points.begin(); l < blob_contour_points.end(); l++){
    MapCoords p = *l;
    sumX+=p.x;
    if( p.y > 180 )
      p.y = -( 360 - p.y );
    sumY+=p.y;
  }
  MapCoords cp( sumY/blob_contour_points.size(), sumX/blob_contour_points.size(), "gal" );

  if( cp.y < 0)
    cp.y = 360+cp.y;

  return cp;
}

float HealpixBlob :: compute_blobs_area_degrees(){

  float pixarea = ( ( 4 * M_PI / 12 * pow(map_resolution,2) )*RAD2DEG )*RAD2DEG;  //def nside2pixarea(nside, degrees=False) https://github.com/healpy/healpy/blob/master/healpy/pixelfunc.py
  // float pixarea2 = pow(90 / ( pow(2,map_resolution) * pow(2,0.5) ), 2);
  // cout << "Area pixel con formula: "<<pixarea<<endl;
  // cout << "Area pixel con formula cdelt: "<<pixarea2<<endl;

  // cout << "Area of Blob: "<<pixarea*number_of_pixels<<endl; // number_of_pixels ?????????
  return pixarea*number_of_pixels;
}
//
//
float HealpixBlob :: compute_photons_closeness()
{
  // cout << "compute_photons_closeness" << endl;

  float photons_closeness = 0;
	float count_distances = 0;
  float count_photons = 0;

	for(vector<pair<MapCoords, int> >::iterator i = blob_photon_points.begin(); i != blob_photon_points.end(); i++){
		pair<MapCoords, int> point_greylevel = *i;

		float p_dist = get_spherical_distance_from_centroid(point_greylevel.first);
    count_distances += p_dist * point_greylevel.second;

		count_photons += point_greylevel.second;
	}

	photons_closeness = count_distances/count_photons;
	return photons_closeness;
}


double HealpixBlob::get_spherical_distance_from_centroid(MapCoords photon)
{
  double distance =  0;

  double b1 = photon.x;
  double l1 = photon.y;
  double l2 = get_galactic_centroid_l();
  double b2 = get_galactic_centroid_b();

  // cout << "l_centroid: "<<l2<<", b_centroid: "<<b2<<endl;
  // cout << "l1: "<<l1<<", b1: "<<b1<<endl;

  if ( l1 < 0 || l1 > 360 || l2 < 0 || l2 > 360){
    cout << "Errore nelle coordinate l"<< endl;
    cout << l1<< " or "<< l2 << endl;
    return  -2;
  }

  if ( b1 < -90 || b1 > 90 || b2 < -90 || b2 > 90){
    cout << "Errore nelle coordinate b"<< endl;
    cout << b1<< " or "<< b2 << endl;
    return  -2;
  }

  double d1 = b1 - b2;
  double d2 = l1 - l2;

  double b11 = ( M_PI / 2.0 ) - (b1 * M_PI  / 180.0);
  double b21 = ( M_PI / 2.0 ) - (b2 * M_PI  / 180.0);
  double m4 = cos(b11) * cos(b21)  + sin(b11) * sin(b21) * cos(d2 * M_PI  / 180.0);
  if(m4 > 1)
     m4 = 1;

  distance = acos(m4) *  180.0 / M_PI;
  // cout << "distance: "<<distance<<endl;

	return distance;
}
