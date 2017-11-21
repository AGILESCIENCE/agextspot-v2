/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#ifndef BLOBSFINDER_H
#define BLOBSFINDER_H

#include <map>
#include <iostream>
#include <math.h>  // ceil ( )
#include <iomanip>
#include "Blob.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;


class BlobsFinder
{
    public:

        /**
            Return a list of Blobs. In order to find one or more Blob it uses:
                - non linear stretching
                - gaussian filtering
                - find contours procedure
         */
        static vector<Blob*> findBlobs(double PSF, string filePath, int ** image, int rows, int cols, double CDELT1, double CDELT2);

    private:
        BlobsFinder();
	static void nonLinearStretch(Mat* inputImage, double r);
	static void gaussianBlur(Mat* inputImg, Size kernelSize, double sigma);
	
	static bool computePixelsAndPhotonsOfBlob(	vector<Point>& contour, 
							Mat& image, 
							int ** data, 
							vector<pair<CustomPoint,int>>& pixelsOfBlobs, 
							vector<CustomPoint>& photonsOfBlobs
	);

	static void reportError(vector<CustomPoint>& photonsOfBlobs, vector<pair<CustomPoint,int>>& pixelsOfBlobs, vector<CustomPoint>& contour, string filePath, int ** data);
	
	static void printImage(Mat& image,string windowName, string type);

	static void printImageInConsole(Mat& image, string type);


	static void printImageBlobs(int rows,int cols, vector<Blob>& blobs, string windowName);
	static void printImageBlob(Mat& inputImage, Blob& b, string windowName);

	static void drawImageHistogram(Mat& hist, int histSize);
};




/* NEED C++14
template<typename T>
void BlobsFinder::printImageInConsole(Mat& image){
	cout << "\n\n" << endl;
	for(int i = 0; i < image.rows; i++){
		for(int j =0; j < image.cols; j++){
			cout << image.at<T>(i,j);
		}
		cout << "\n";
	}
}*/




#endif // BLOBSFINDER_H
