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
// #include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <healpix_map.h>
#include <healpix_base.h>
#include <healpix_map_fitsio.h>

#include "Blob.h"
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

    vector<Blob*> findBlobs(string fitsfilePath, bool debug);

    string get_format();

  private:

        //NEW NEW version
    Healpix_Map<float> gassusianSmoothing(Healpix_Map<int> map, int nPix, int mresRound, float psf, float cdelt1, float cdelt2, bool debug);

    float ** filterCreation(int kernel_side);

    //new VERSION
    Healpix_Map<float> thresholding(Healpix_Map<float> convolved_map, long int nPix, int mresRound);

    //new VERSION
    Healpix_Map <int> findConnectedComponent(Healpix_Map<float> thresholded_map, int mresRound);

    int computePixelsAndPhotonsOfBlob(Healpix_Map <int> labeledMap);

    int saveHealpixINTImage( string imageName, Healpix_Map<int> map);

    int saveHealpixFLOATImage( string imageName, Healpix_Map<float> map);

    // pair <int, pair < int, vector<int> > > computePixelsAndCountourBlob(Healpix_Map <int> labeledMap, int mresRound, vector < pair <int, pair < int, vector<int> > > > allBlobs);
    int computePixelsAndCountourBlob(Healpix_Map <int> labeledMap, int mresRound, vector < pair <int, pair < int, vector<int> > > > * allBlobs);


};

#endif
