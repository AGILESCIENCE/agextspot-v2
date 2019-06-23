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

#ifndef AGILECOUNTMAPSBLOBSFINDER_H
#define AGILECOUNTMAPSBLOBSFINDER_H

#include <map>
#include <iostream>
#include <math.h>  // ceil ( )
#include <iomanip>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include "AgileBlob.h"
#include "MapConverter.h"
#include "BlobsFinder.h"

using namespace cv;
using std::make_pair;
using std::fixed;

using std::make_pair;
using std::fixed;


class AgileCountMapsBlobsFinder : public BlobsFinder
{

    public:

      AgileCountMapsBlobsFinder(float cdelt1, float cdelt2, float psf);

      vector<Blob * > find_blobs(string fitsfilename, string fitsfile_folder, bool debug, bool save_cv_steps, string output_folder);

      string get_format();

    private:

        Mat gassusian_smoothing(IntMatrixCustomMap * int_matrix_map, float PSF, float CDELT1, float CDELT2, bool debug);

        Mat thresholding(Mat image, bool debug);

        Mat add_padding_to_image(Mat image8U);

        void compute_pixels_and_photons_of_blob(	IntMatrixCustomMap * int_matrix_map_original,
                                                  Mat& smoothed_and_thresholded_image,
                                                  vector<Point>& contour,
                                    							vector<pair<MapCoords,int> >& pixelsOfBlobs,
                                    							vector<pair<MapCoords,int> >&  photonsOfBlobs
                                                );

        // DEBUGGING
        float sumImagePixels();
        void reportError(vector<MapCoords>& photonsOfBlobs, vector<pair<MapCoords,int> >& pixelsOfBlobs, vector<MapCoords>& contour, string fitsFilePath, IntMatrixCustomMap * int_matrix_map);
        void print01Image(Mat& image,string windowName);
        void printImage(Mat& image,string windowName, string type);
        void printImageInConsole(Mat& image, string type);
        void printImageBlobs(int rows,int cols, vector<Blob>& blobs, string windowName);
        void printImageBlob(Mat& inputImage, Blob& b, string windowName);
        void drawImageHistogram(Mat& hist, int histSize);

};


#endif // AGILECOUNTMAPSBLOBSFINDER_H
