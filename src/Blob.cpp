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

#include "Blob.h"

Blob::Blob(vector<MapCoords > & _blob_contour_points, vector<pair<MapCoords,int> > & _blob_points, vector<pair<MapCoords,int> > & _photon_points)
		 : feature_class(UNKOWN)
		 , blob_contour_points(_blob_contour_points)
		 , blob_points(_blob_points)
		 , blob_photon_points(_photon_points)
		 , allocator(json_blob.GetAllocator())
		 , json_meta(rapidjson::kObjectType)
		 , json_features(rapidjson::kObjectType)
{

	json_blob.SetObject(); // define the json document as an object rather than an array

	// features
	number_of_pixels = blob_points.size();

	contour_size = blob_contour_points.size();

	number_of_photons = blob_photon_points.size();

	grey_level_mean = compute_grey_levels_mean();

}


// features
float Blob::compute_circularity_ratio()
{
	#ifdef DEBUG
	cout << "[compute_circularity_ratio] \nblob_area_deg = "<<blob_area_deg<<"\ncontour_size = "<<contour_size<<"\ncircularity = "<<blob_area_deg/(contour_size*contour_size)<<endl;
	#endif
	// Area (pixel^2) / perimetro alla seconda (pixel)
	return blob_area_deg/(contour_size*contour_size);
}

float Blob::compute_rectangularity()
{
	return 0;
}

float Blob::compute_eccentricity()
{
	return 0;
}

float Blob::compute_grey_levels_mean()
{
	float grey_level_count = 0;

	for (vector<pair<MapCoords,int> >::iterator it = blob_points.begin(); it != blob_points.end(); ++it)
	{
		pair<MapCoords,int> point = *it;
		grey_level_count += point.second;
	}

	//cout << "["<<centroid.y<<","<<centroid.x<<"]" << greyLevelCount << " / " << numberOfBlobPixels << " = "<< greyLevelCount/numberOfBlobPixels << endl;
	return grey_level_count / (float) number_of_pixels;
}



rapidjson::Value & Blob::get_json_meta()
{
	// Galactic centroid
	rapidjson::Value gal_centr(rapidjson::kObjectType);
	rapidjson::Value gal_centr_l(get_galactic_centroid_l());
	rapidjson::Value gal_centr_b(get_galactic_centroid_b());
	gal_centr.AddMember("l", gal_centr_l, allocator);
	gal_centr.AddMember("b", gal_centr_b, allocator);
	json_meta.AddMember("galactic_centroid", gal_centr, allocator);

	/* Contour !!
	rapidjson::Value cont(rapidjson::kArrayType);
	for(vector<MapCoords>::iterator l = contour.begin(); l < contour.end(); l++){
		MapCoords p = *l;
		rapidjson::Value cont_point(rapidjson::kObjectType);
		rapidjson::Value cont_point_x(p.x);
		rapidjson::Value cont_point_y(p.y);
		cont_point.AddMember("x", cont_point_x, allocator);
		cont_point.AddMember("y", cont_point_y, allocator);

		cont.PushBack(cont_point, allocator);
	}
	meta.AddMember("contour", cont, allocator);
	*/

	return json_meta;
}

rapidjson::Value & Blob::get_json_features()
{
	rapidjson::Value cls(feature_class);
	json_features.AddMember("class", cls, allocator);

	rapidjson::Value a_d(blob_area_deg);
	json_features.AddMember("area_degrees", a_d, allocator);

	rapidjson::Value p_g_l_m(grey_level_mean);
	json_features.AddMember("pixel_grey_levels_mean", p_g_l_m, allocator);

	rapidjson::Value n_o_p(number_of_pixels);
	json_features.AddMember("number_of_pixels", n_o_p, allocator);

	rapidjson::Value n_o_ph(number_of_photons);
	json_features.AddMember("number_of_photons", n_o_ph, allocator);

	rapidjson::Value c_size(contour_size);
	json_features.AddMember("contour_size", c_size, allocator);

	rapidjson::Value circ_r(circulary_ratio);
	json_features.AddMember("circularity", circ_r, allocator);

	rapidjson::Value rect(rectangularity);
	json_features.AddMember("rectangularity", rect, allocator);

	rapidjson::Value ecc(eccentricity);
	json_features.AddMember("eccentricity", ecc, allocator);

	return json_features;
}


string Blob::_to_json_str()
{

	// add meta e features to the json document
	json_blob.AddMember("meta", json_meta, allocator);
	json_blob.AddMember("features", json_features, allocator);


	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	json_blob.Accept(writer);
	const char * json_cstr = strbuf.GetString();
	string json_str = string(json_cstr);
	return json_str;
}
