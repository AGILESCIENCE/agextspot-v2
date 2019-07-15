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

#ifndef HEALPIX_COUNT_MAPS_BLOBS_FINDER_H
#define HEALPIX_COUNT_MAPS_BLOBS_FINDER_H

#include <map>
#include <iostream>
#include <math.h>  // ceil ( )
#include <iomanip>
#include <iostream>
#include <vector>
#include <Eval.h> // DEG2RAD
#include <chrono>



#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <healpix_map.h>
#include <healpix_base.h>
#include <healpix_map_fitsio.h>

#include "HealpixBlob.h"
#include "BlobsFinder.h"
#include "MapConverter.h"

using std::string;
using std::cout;
using std::endl;
using std::vector;
using namespace cv;


class HealPixCountMapsBlobsFinder : public BlobsFinder{

  public:

    HealPixCountMapsBlobsFinder(float cdelt1, float cdelt2, float psf);

    vector<Blob*> find_blobs(string fitsfilename, string fitsfile_folder, bool save_cv_steps, string output_folder);

    string get_format();

  private:

    Healpix_Map<float> gassusian_smoothing(Healpix_Map<int> map, int nPix, int map_resolution, float psf, float cdelt1, float cdelt2);

    float ** filter_creation(int kernel_side);

    Healpix_Map<float> thresholding(Healpix_Map<float> convolved_map, long int nPix, int map_resolution);

    Healpix_Map <int> find_connected_components(Healpix_Map<float> thresholded_map, int map_resolution, vector < vector <int> > & connected_component_indexes);

    int save_healpix_INT_image( string imageName, Healpix_Map<int> map);

    int save_healpix_FLOAT_image( string imageName, Healpix_Map<float> map);

    //new
    int compute_blob_features(int map_resolution,
                              vector <int> & connected_component_indexes,
                              Healpix_Map<int> & map,
                              Healpix_Map<float> & thresholded_map,
                              Healpix_Map <int> & labeledMap,
                              vector<pair<MapCoords,int> > & points,
                              vector<pair<MapCoords,int> > & photon_points,
                              vector<MapCoords > & contour_points
                            );
    Healpix_Map <int> compute_blobs_map(int map_resolution, vector<Blob *> blobs);

    void print_vector_int(string name, vector<int>& v_int, bool print_vertical);


};

#endif
