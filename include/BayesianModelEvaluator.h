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

#include "stdio.h" // getchar
#include <iostream>
#include <vector>

#include "FolderManager.h"
#include "AgileCountMapsBlobsFinder.h"
#include "MapConverter.h"
#include "NormalDistribution.h"


class BayesianModelEvaluator{

	public:
		static void computeModel(string trainingSetPath, double CDELT1, double CDELT2, double PSF);


	private:

		static void computeModelFromTrainingSet(string type,
												vector<string> fileNames,
												string trainingSetPath,
												double CDELT1,
												double CDELT2,
												double PSF
												);

		static pair<vector<Blob *>,vector<Blob *> > getAllBlobsFromTrainingSet(	vector<string> bgFileNames,
																				 vector<string> fluxFileNames,
																				 string trainingSetBackgroundPath,
																				 string trainingSetFluxPath,
																				 double CDELT1,
																				 double CDELT2,
																				 double PSF
																			 );


		static void printMeanAndDeviation(string type, vector<Blob *>& allBlobs);

		static NormalDistribution computeNormalDistribution(vector<double>& attributeValues);

		static void cleanMemory(vector<Blob*>& blobs);

		BayesianModelEvaluator();

};
