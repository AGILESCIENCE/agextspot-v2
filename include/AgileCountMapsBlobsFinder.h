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

#ifndef AGILECOUNTMAPSBLOBSFINDER_H
#define AGILECOUNTMAPSBLOBSFINDER_H_H

#include <map>
#include <iostream>
#include <math.h>  // ceil ( )
#include <iomanip>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include "Blob.h"
#include "MapConverter.h"
#include "BlobsFinder.h"

using namespace cv;

class AgileCountMapsBlobsFinder : public BlobsFinder
{

    public:

      AgileCountMapsBlobsFinder(float cdelt1, float cdelt2, float psf);

      vector<Blob*> findBlobs(string fitsfilePath, bool debug);

      string get_format();

    private:

        Mat gassusianSmoothing(IntMatrixCustomMap * int_matrix_map, float PSF, float CDELT1, float CDELT2, bool debug);

        Mat thresholding(Mat image, bool debug);

        Mat addPaddingToImage(Mat image8U);

        void computePixelsAndPhotonsOfBlob(	IntMatrixCustomMap * int_matrix_map_original,
                                            Mat& smoothed_and_thresholded_image,
                                            vector<Point>& contour,
                              							vector<pair<CustomPoint,int> >& pixelsOfBlobs,
                              							vector<CustomPoint>& photonsOfBlobs
                                          );

        // DEBUGGING
        float sumImagePixels();
        void reportError(vector<CustomPoint>& photonsOfBlobs, vector<pair<CustomPoint,int> >& pixelsOfBlobs, vector<CustomPoint>& contour, string fitsFilePath, IntMatrixCustomMap * int_matrix_map);
        void print01Image(Mat& image,string windowName);
        void printImage(Mat& image,string windowName, string type);
        void printImageInConsole(Mat& image, string type);
        void printImageBlobs(int rows,int cols, vector<Blob>& blobs, string windowName);
        void printImageBlob(Mat& inputImage, Blob& b, string windowName);
        void drawImageHistogram(Mat& hist, int histSize);

};


#endif
