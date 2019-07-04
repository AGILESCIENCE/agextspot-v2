////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION
//       AGILE Science Tools
//       agextspot-v2
//       First release: 2017
//       Authors: Leonardo Baroncelli leonardo.baroncelli@inaf.it,
//                Giancarlo Zollino giancarlo.zollino@gmail.com
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
////////////////////////////////////////////////////////////////////////////////////*/

#include "AgileBlob.h"

AgileBlob::AgileBlob(string filepath, float _cdelt1, float _cdelt2, vector<MapCoords > & contour_points, vector<pair<MapCoords,int> > & points, vector<pair<MapCoords,int> > & photon_points)
              : Blob(contour_points, points, photon_points)
              , agilemap_tool(filepath.c_str() )
              , cdelt1(_cdelt1)
              , cdelt2(_cdelt2)
{

  centroid = compute_centroid();
  img_centroid = compute_img_centroid();
  photons_closeness = compute_photons_closeness();
  blob_area_deg = compute_blobs_area_degrees();

  compute_morphology_measures();


}


bool AgileBlob::is_centered()
{
  int centerY = agilemap_tool.Rows()/2;
  int centerX = agilemap_tool.Cols()/2;
  int offset = 15;
  if( centroid.x <= centerX+offset && centroid.x >= centerX-offset && centroid.y <= centerY+offset && centroid.y >= centerY-offset ){
    return true;
  }else{
    return false;
  }
}

float AgileBlob::compute_blobs_area_degrees()
{
  return cdelt1*cdelt1*number_of_pixels;
}

MapCoords AgileBlob::compute_img_centroid()
{
  int sumX=0;
  int sumY=0;
  for(vector<MapCoords>::iterator l = blob_contour_points.begin(); l < blob_contour_points.end(); l++){
    MapCoords p = *l;
    sumX+=p.x;
    sumY+=p.y;
  }
  MapCoords cp( sumY/blob_contour_points.size(), sumX/blob_contour_points.size(), "pix" );

  return cp;
}


MapCoords AgileBlob::compute_centroid()
{
  // Changing the reference system
	int y_euclidean_ref_sys = agilemap_tool.Rows() - img_centroid.y;
	double l  = agilemap_tool.l(centroid.x, y_euclidean_ref_sys );
	double b  = agilemap_tool.b(centroid.x, y_euclidean_ref_sys );

	MapCoords cp( l, b, "gal" );
	return cp;
}




float AgileBlob::compute_photons_closeness()
{
	float photons_closeness = 0;
	float count_distances = 0;
  float count_photons = 0;

	for(vector<pair<MapCoords, int> >::iterator i = blob_photon_points.begin(); i != blob_photon_points.end(); i++){
		pair<MapCoords, int> point_greylevel = *i;

		float p_dist = get_spherical_distance_from_centroid(point_greylevel.first);
    count_distances += p_dist * point_greylevel.second;

		count_photons += point_greylevel.second;
	}
	// cout << "countDistances: " << countDistances << endl;
	// cout << "countPhotons: " << countPhotons << endl;

	photons_closeness = count_distances/count_photons;
	return photons_closeness;
}


double AgileBlob::get_spherical_distance_from_centroid(MapCoords photon)
{
	double distance =  0;
	//cout <<"\nfile: "<<agileMapTool.GetFileName()<<endl;
	distance = agilemap_tool.SrcDist(photon.x,agilemap_tool.Rows() - photon.y, get_galactic_centroid_l(), get_galactic_centroid_b());
	//cout << "distance: " << distance << ""<<endl;
	return distance;
}

string AgileBlob::to_json_str(string filepath)
{
  // take these from the parent
  rapidjson::Value& json_meta = get_json_meta();
  rapidjson::Value& json_features = get_json_features();


  // adding custom meta
  rapidjson::Value f_p(rapidjson::StringRef(filepath.c_str()));
	json_meta.AddMember("file_path", f_p, allocator);

	//rapidjson::Value cd_1(cdelt1);
	//json_meta.AddMember("cdelt1", cd_1, allocator);

	//rapidjson::Value cd_2(cdelt2);
	//json_meta.AddMember("cdelt2", cd_2, allocator);

	//rapidjson::Value p(psf);
	//json_meta.AddMember("psf", p, allocator);

	//rapidjson::Value kss(kernel_side_size);
	//json_meta.AddMember("gauss_kernel_side_size", kss, allocator);


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
