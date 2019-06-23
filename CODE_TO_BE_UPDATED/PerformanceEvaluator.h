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

#ifndef PERFORMANCEEVAULATOR_H
#define PERFORMANCEEVAULATOR_H

#include <map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <sstream>

#include "Blob.h"
#include "FolderManager.h"
#include "AgileCountMapsBlobsFinder.h"
#include "BayesianClassifierForBlobs.h"
#include "FileWriter.h"

#include "AgileMap.h"


class PerformanceEvaluator{

	public:
		PerformanceEvaluator(string testSetPath, double min_threshold, double max_threshold, double thresholdStep, double CDELT1, double CDELT2, double PSF);
		void evaluate();


	private:

		string testSetPath;

		double minThreshold;
		double maxThreshold;
		double thresholdStep;

		double CDELT1;
		double CDELT2;
		double PSF;

		time_t timer;

		BayesianClassifierForBlobs reverendBayes;

		string outfilename_log_file;
		string outfilename_log_for_plot;

		// Le immagini
		vector<string> filenames;

		// Il test set ->  <identificativo dell'istanza, pair<PuntatoreAlBlob,Etichetta di classificazione> >
		map< string, pair < Blob * , char > > testSet;

		// Prende la lista dei blobs e gli filtra scegliendo gli elementi che andranno a formare il test set
		void createTestSetMap();

		// Effettua la classificazione sul Blob * di ogni elemento di allBlobs. Ritorna una mappa <identificativo, coppia (centroide, etichetta di classificazione predetta)
		map< string, pair < Blob * , char > > createPredictionsMap(double currentThreshold);

		// Confronta ogni elemento di testSet con il corrispondente elemento di predictions.
		void computePerformance(map< string, pair < Blob * , char > >& predictions,double currentThreshold);

		BlobsFinder * blobs_finder;
};

#endif
