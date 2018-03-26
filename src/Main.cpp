
/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
 *
 * https://github.com/Leofaber/agextspot-v2
*/

#include <PilParams.h>

#include "GammaRayDetector.h"

using std::cout;
using std::endl;


const char* startString = {
"################################################################\n"
"###             Task AG_extspot-v2 v1.2.1 - (degree-based)    ###"
};

const char* endString = {
"### Task AG_extspot exiting .................................###\n"
"################################################################"
};

const PilDescription paramsDescr[] = {
	{ PilString, "outputLogName","Output log file name"},
	{ PilReal,   "psf","Telescope's Point Spread Function value (degree)"},
	{ PilString, "imageCtsPath", "Input ctsMap file path"},
	{ PilReal, 	 "classificationThreshold", "Classification threshold"},
	{ PilBool,   "visualizationMode","If 'yes' (or 'y') each blob extraction step will be visualized in a graphic window (interactive mode). Insert 'no' (or 'n') instead"},
	{ PilString, "imageExpPath", "Input expMap file path"},
	{ PilBool,   "isExpMapNormalized","If 'yes' (or 'y') you assert that the exp-map is already normalized. Insert 'no' (or 'n') instead"},
	{ PilBool, 	 "createExpNormalizedMap", "If 'yes' (or 'y') the normalized exp map will be written on file"},
	{ PilBool,   "createExpRatioMap", "If 'yes' (or 'y') the exp-ratio map will be written on file"},
	{ PilReal,   "minThreshold", "The lower bound for the threshold level in exp-ratio evaluation"},
	{ PilReal,   "maxThreshold", "The upper bound for the threshold level in exp-ratio evaluation"},
	{ PilReal, 	 "squareSize", "The degree dimension of the exp ratio evaluation area's edge"},
	{ PilNone, 	 "", "" }
};

int main(int argc, char*argv[]){

	cout << startString << "\n" << endl;

	PilParams params(paramsDescr);

    if (!params.Load(argc, argv))
        return EXIT_FAILURE;


	const char * outputLogName 		= params["outputLogName"];
	double PSF 						= params["psf"];
	const char * imageCtsPath 		= params["imageCtsPath"];
	double classificationThreshold 	= params["classificationThreshold"];
	const char * imageExpPath 		= params["imageExpPath"];
	bool isExpMapNormalized 		= params["isExpMapNormalized"];
	bool createExpNormalizedMap 	= params["createExpNormalizedMap"];
	bool createExpRatioMap 			= params["createExpRatioMap"];
	double minTreshold 				= params["minTreshold"];
	double maxTreshold 				= params["maxTreshold"];
	double squareSize 				= params["squareSize"];
	double visualizationMode 		= params["visualizationMode"];

    // PRINT INPUT PARAMETERS -------------------------------------
	params.Print();

	cout << "CDELT=0.5" << endl;
	int kernelSize = (2 * PSF/0.5) + 1;
	cout << "Smoothing with:\n * kernel size: ["<<kernelSize <<"x"<< kernelSize <<"] (formula: 2 * PSF/CDELT2 + 1)\n * sigma (PSF): "<< PSF << endl;


	// CORE LOGIC -----------------------------------------------
	GammaRayDetector grd(
							PSF,
							imageCtsPath,
							outputLogName,
							classificationThreshold,
							imageExpPath,
							isExpMapNormalized,
							createExpNormalizedMap,
							createExpRatioMap,
							minTreshold,
							maxTreshold,
							squareSize,
							visualizationMode
						);



   	grd.detect();


	cout << endString << endl;


   	return 0;

}
