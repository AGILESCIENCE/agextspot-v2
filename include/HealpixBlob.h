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

#ifndef HEALPIX_BLOB_H
#define HEALPIX_BLOB_H

#include <Eval.h> // DEG2RAD

#include "Blob.h"

class HealpixBlob : public Blob {

public:

  HealpixBlob(string filepath, float cdelt1, float cdelt2, int mresRound, vector<MapCoords > & contour_points, vector<pair<MapCoords,int> > & points, vector<pair<MapCoords,int> > & photon_points);

  // implementation of virtual method
  string to_json_str(string filepath);

  // getters
  inline MapCoords get_img_centroid() { return img_centroid; }
  inline float get_photons_closeness() { return photons_closeness; }

private:

  float cdelt1, cdelt2;
  int mresRound;

  // additional features
  float photons_closeness;
  MapCoords img_centroid;

  // implementation of virtual methods
  MapCoords compute_centroid();
  float compute_blobs_area_degrees();
  //
  // // additional methods
  float compute_photons_closeness();
  double get_spherical_distance_from_centroid(MapCoords photon);
  // MapCoords compute_img_centroid();


};

#endif
