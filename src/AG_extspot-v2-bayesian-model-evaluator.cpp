/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
 *
 * https://github.com/Leofaber/AG_extspot-v2-bayesian-model-evaluator
*/

#include <stdlib.h>
#include "BayesianModelEvaluator.h"


using std::cout;
using std::endl;

const char* startString = {
"################################################################\n"
"###  Task AG_extspot-v2-bayesian-model-evaluator  v.1.0.0    ###"
};

const char* endString = {
"### AG_extspot-v2-bayesian-model-evaluator ..................###\n"
"################################################################"
};


int main(int argc, char*argv[]){

	cout << startString << endl;

	string trainingSetPath = argv[1];
	double CDELT1 = atof(argv[2]);
	double CDELT2 = atof(argv[3]);
	double PSF = atof(argv[4]);

	cout << "\nTraining set path: "<< trainingSetPath << endl;
	cout << "CDELT1: "<< CDELT1 << endl;
	cout << "CDELT2: "<< CDELT2 << endl;
	cout << "PSF: "<< PSF << endl;

	int kernelSize = (2 * PSF/CDELT2) + 1;
	cout << "Smoothing with:\n * kernel size: ["<<kernelSize <<"x"<< kernelSize <<"] (formula: 2 * PSF/CDELT2 + 1)\n * sigma (PSF): "<< PSF << endl;

	/*
		It will print on console the MEAN and DEVIATIONS of all blob's attributes.
	*/
	BayesianModelEvaluator::computeModel(trainingSetPath, CDELT1, CDELT2, PSF );



	cout << endString << endl;

	return 0;
}
