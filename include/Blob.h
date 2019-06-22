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

#ifndef BLOB_H
#define BLOB_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <string>

// Rapidjson
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"

using std::string;
using std::to_string;
using std::vector;
using std::iterator;
using std::pair;
using std::cout;
using std::endl;

enum f_class
{   BG = 0,
    GRB = 1,
		UNKOWN = -1
};



struct MapCoords{
	double y;
	double x;
	string type; // gal , pix
 	MapCoords(){ y = 0; x = 0; }
	MapCoords(double _y, double _x, string _t){ y = _y; x = _x; type = _t; }
};

class Blob
{
	public:

		// constructor
		Blob(vector<MapCoords > & contour_points, vector<pair<MapCoords,int> > & points, vector<pair<MapCoords,int> > & photon_points);


    // virtual
    virtual string to_json_str(string filepath) = 0; // Subclasses must implement this

		// getters
		inline vector<MapCoords > get_contour() { return blob_contour_points; }
		inline vector<pair<MapCoords,int> > get_points() { return blob_points; }
		inline vector<pair<MapCoords,int> > get_photons() { return blob_photon_points; }
		inline MapCoords get_galactic_centroid() { return centroid; }
		inline double get_galactic_centroid_l() { return centroid.y; }
		inline double get_galactic_centroid_b() { return centroid.x; }
		inline int get_number_of_photons() { return number_of_photons; }
		inline float get_blob_area_deg() { return blob_area_deg; }

	protected: //attributes

		// Blob data
		MapCoords centroid; // not computed
		vector<MapCoords> blob_contour_points;
		vector<pair<MapCoords,int> > blob_points;
		vector<pair<MapCoords,int> > blob_photon_points;

		// Blob features
		f_class feature_class;
		int number_of_pixels; // the number of points
		float blob_area_deg;	// the area of the Blobs expressed in degree^2
		float grey_level_mean; // the sum of all the grey levels of the pixels of the blob divided by the number of those pixels.
		int number_of_photons; // the number of gamma photons inside the Blob
		int contour_size; // the number of points that belong to the contour of the Blob
		float circulary_ratio; // Area (pixel^2) / perimetro alla seconda (pixel)
		float rectangularity; // Area / area rettangolo che contiene la shape
		float eccentricity; // asse maggiore / asse minore (or of the bounding rectangle)

		// Json document
		rapidjson::Document json_blob;
		rapidjson::Document::AllocatorType& allocator;
		rapidjson::Value json_meta;
		rapidjson::Value json_features;



	protected: // methods

		// VIRTUAL
		virtual MapCoords compute_centroid() = 0; // Subclasses must implement this
		virtual float compute_blobs_area_degrees() = 0;  // Subclasses must implement this



		float compute_grey_levels_mean();
		float compute_rectangularity();
		float compute_circulary_ratio();
		float compute_eccentricity();


		rapidjson::Value& get_json_meta();
		rapidjson::Value& get_json_features();

    // to json
		string _to_json_str();
};
#endif // BLOB_H
