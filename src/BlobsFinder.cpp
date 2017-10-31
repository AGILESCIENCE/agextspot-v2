/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#include "BlobsFinder.h"

BlobsFinder::BlobsFinder()
{
    //ctor
}

vector<Blob> BlobsFinder::findBlobs(Mat tempImage) {

    Mat photonImage = tempImage.clone();


    /// STRETCHING
 	float r = 0.05;
	tempImage = HistogramStretching::nonLinearStretch(tempImage,r);


    /// GAUSSIAN FILTERING
	GaussianFilterer gaussianFilter(Size(21, 21), 4); // 17x17   2.5    23x23   3
	tempImage = gaussianFilter.filter(tempImage);


	/// FIND THE CONTOUR OF EACH BLOB

    vector<Blob> blobs;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    Mat copyOfImage = tempImage.clone();
    findContours( copyOfImage, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

    int indexx=0;
    for(vector<vector<Point> >::iterator i = contours.begin(); i < contours.end(); i++){

        vector<Point> currentBlob = *i;
        /// CREATING A BLOB (se e solo se non è un blob contenuto in un altro blob..
        if(hierarchy[indexx][3]!=0){
			Blob b(currentBlob,tempImage,photonImage);
			cout << "b: " << &b << endl;
			blobs.push_back(b/*new Blob(currentBlob,tempImage,photonImage)*/);
		}
            
        indexx++;

    }


    return blobs;
}

