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
#include <iostream>
#include <string>

#include "AgileMap.h"

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

struct CustomPoint{
	int y;  // righe
	int x;	//colonne
	CustomPoint(){ y = 0; x = 0; }
	CustomPoint(int _y, int _x){ y = _y; x = _x; }
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
        Blob(string _filePath, float _cdelt1, float _cdelt2, float _psf, int _map_rows, int _map_cols, vector<CustomPoint > & _contourPixels, vector<pair<CustomPoint,int> > & _blobPixels, vector<CustomPoint > & _photonsInBlob);


				/**
					getters
				*/
	      string getFilePath();
        CustomPoint getCentroid();
        CustomPoint getGalacticCentroid();
      	double getGalacticCentroidL();
      	double getGalacticCentroidB();
      	float getPixelsMean();
				float getArea();
				float getPhotonsCloseness();
				int getNumberOfPixels();
				int getNumberOfPhotonsInBlob();
        vector<CustomPoint> getContour();
	      vector<CustomPoint> getPhotonsInBlob();

        /**
          Checks if the blob's centroid is included in a range
        */
        bool isCentered();


				/**
					Returns a Json string
				*/
				string toJsonString();


     private:

				// Map meta informations
				string filePath;
				int map_rows;
				int map_cols;
				float cdelt1;
				float cdelt2;
				float psf;
				float pixelArea;


				// Blob informations
        vector<CustomPoint> photonsInBlob;
				CustomPoint centroid;
				CustomPoint galacticCentroid;
				vector<pair<CustomPoint,int> > blobPixels;
				vector<CustomPoint> contour;

				// Blob features
				int numberOfPixels;
      	float blobArea;
      	float pixelMean;
        float photonsCloseness;
				int number_of_photons;
				int contour_size;
				float circulary_ratio; // Area (pixel^2) / perimetro alla seconda (pixel)
				float rectangularity; // Area / area rettangolo che contiene la shape
				float eccentricity; // asse maggiore / asse minore (or of the bounding rectangle)

				// utilities
				AgileMap agileMapTool;

				CustomPoint computeCentroid();

				CustomPoint computeGalacticCentroid();

				double getDistanceFromCentroid(CustomPoint p);

				double getSphericalDistanceFromCentroid(CustomPoint p);

				float computePixelMean(); // Return the sum of all the grey levels of the pixels of the blob divided by the number of those pixels.

				float computePhotonsCloseness(); // Return the sum of the distances of each photons from the centroid divided by the number of photons

				float computeCircularyRatio(); // Area (pixel^2) / perimetro alla seconda (pixel)

				float computeRectangularity(); // Area / area rettangolo che contiene la shape

				float computeEccentricity(); // asse maggiore / asse minore (or of the bounding rectangle)



};
#endif // BLOB_H
