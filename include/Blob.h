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

#ifndef BLOB_H
#define BLOB_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include "AgileMap.h"

using namespace std;

struct CustomPoint{
	int y;  // righe
	int x;	//colonne
	CustomPoint(){
		y = 0;
		x = 0;
	}
	CustomPoint(int _y, int _x){
		y = _y;
		x = _x;
	}
};






class Blob
{
    public:

        /**
          Create a new Blob starting from the contour pixels.
          Computes the centroid of the blob.
          Finds the pixels that are inside the blob.
          Finds the number of photons inside the blob.
          Compute the gray level pixel mean of the blob.
          Compute the photon's closeness
        */
        Blob(string filePath, vector<CustomPoint>& _contourPixels, vector<pair<CustomPoint,int> >& _blobPixels, vector<CustomPoint> photonsInBlob, double CDELT1, double CDELT2);



	       string getFilePath();

        /**
            Return the centroid of the blob
        */
        CustomPoint getCentroid();

 	      /**
            Return the galactic centroid of the blob
        */
        CustomPoint getGalacticCentroid();

      	double getGalacticCentroidL();
      	double getGalacticCentroidB();


        /**
            Return the sum of all the grey levels of the pixels of the blob divided by the number of those pixels.
        */
        double getPixelsMean();

        /**
            Return all the contour points
        */
        vector<CustomPoint> getContour();

        /**
            Return the number of pixels inside the blob
        */
        int getNumberOfPixels();

        /**
        	Return the blob's area (degree)
        */
        double getArea();

        /**
            Return the number of photons inside the blob (before any stretching or smoothing)
        */
        int getNumberOfPhotonsInBlob();


	       vector<CustomPoint> getPhotonsInBlob();


        /**
            Return all the Pixels (a CustomPoint with a grey level) that are inside the blob
        */
        vector<pair<CustomPoint,int> > getBlobPixels();


        /**
            Return the sum of the distances of each photons from the centroid divided by the number of photons
        */
        double getPhotonsCloseness();

        /**
            Check if the blob's centroid is included in a range
        */
        bool isCentered();


     private:

      	//static int ID;

      	//const int blobId;

      	AgileMap agileMapTool;

      	string filePath;

        vector<CustomPoint> contour;

        vector<pair<CustomPoint,int> > blobPixels;

        int numberOfPixels;

      	double pixelArea;

      	double blobArea;

        vector<CustomPoint> photonsInBlob;

        CustomPoint centroid;

      	CustomPoint galacticCentroid;

      	double pixelMean;

        double photonsCloseness;

	       //vector<CustomPoint> computePhotonsBlob();

        CustomPoint computeCentroid();

        CustomPoint computeGalacticCentroid();

        double computePixelMean();

        double computePhotonsCloseness();

        double getDistanceFromCentroid(CustomPoint p);

	      double getSphericalDistanceFromCentroid(CustomPoint p);


};
#endif // BLOB_H
