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


#ifndef GAMMARAYDETECTOR_H
#define GAMMARAYDETECTOR_H

#include <ExpRatioEvaluator.h>
#include <AgileMap.h>

#include "BayesianClassifierForBlobs.h"
#include "AgileCountMapsBlobsFinder.h"
#include "FileWriter.h"


class GammaRayDetector
{
public:

  /**
      User has to specify the path to the FITS file, the name of the output file, the classification threshold as an interger or a floating point number
  */
	GammaRayDetector(double PSF, const char * imageCtsPath, const char * outputLogName, double classificationThreshold, const char * imageExpPath, bool isExpMapNormalizedBool, bool createExpNormalizedMap,bool createExpRatioMap, double minTreshold, double maxTreshold, double squareSize, bool visualizationMode);

	~GammaRayDetector();
	/**
			Open the fits file, convert it to Mat image, extract blobs, classify them with bayesian classifier.
	*/
	void detect();


private:


    /**
        Given a blob call Reverend Bayes to predict the probabilities.
    */
	double classifyBlob(Blob* b);
	string extractFileNameFromImagePath(string imagePath);
	string computeOutputLogName(string filename, string outputLogName, double minThreshold, double maxThreshold, double squareSize);



	double psf;
	string imagePath;
	string outputLogName;
	string fileName;
	float classificationThreshold;
	bool evaluateExpRatio;
	bool visualizationMode;

	const char *imageExpPath;
	int** ctsMap;
	int rows;
	int cols;

	vector<Blob*> blobs;

	BayesianClassifierForBlobs reverendBayes;
	AgileMap agileMapTool;
	ExpRatioEvaluator* exp;
	BlobsFinder * blobs_finder;
 };

#endif
