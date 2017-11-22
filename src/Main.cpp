
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


using namespace std;

const char* startString = {
"################################################################\n"
"###                   Task AG_extspot-v2 v1.1.3 -               ###"
};

const char* endString = {
"### Task AG_extspot exiting .................................###\n"
"################################################################"
};

const PilDescription paramsDescr[] = {
	{ PilString, 	"outputLogName","Output log file name"},
	{ PilString, 	"imageCtsPath", "Input ctsMap file path"},
	{ PilReal, 	"classificationThreshold", "Classification threshold"},
	{ PilString, 	"imageExpPath", "Input expMap file path"},
	{ PilBool, 	"isExpMapNormalized","If 'yes' (or 'y') you assert that the exp-map is already normalized. Insert 'no' (or 'n') instead"},
	{ PilBool, 	"createExpNormalizedMap", "If 'yes' (or 'y') the normalized exp map will be written on file"},
	{ PilBool, 	"createExpRatioMap", "If 'yes' (or 'y') the exp-ratio map will be written on file"},
	{ PilReal, 	"minThreshold", "The lower bound for the threshold level in exp-ratio evaluation"},
	{ PilReal, 	"maxThreshold", "The upper bound for the threshold level in exp-ratio evaluation"},
	{ PilReal, 	"squareSize", "The degree dimension of the exp ratio evaluation area's edge"},
	{ PilNone, 	"", "" }
};

int main(int argc, char*argv[]){
	
	cout << startString << endl;
	
	PilParams params(paramsDescr);

    if (!params.Load(argc, argv))
        return EXIT_FAILURE;
	    
	
	const char * outputLogName 	= params["outputLogName"];
	const char * imageCtsPath 	= params["imageCtsPath"];
	double classificationThreshold 	= params["classificationThreshold"];
	const char * imageExpPath 	= params["imageExpPath"];
	bool isExpMapNormalized 	= params["isExpMapNormalized"];
	bool createExpNormalizedMap 	= params["createExpNormalizedMap"];
	bool createExpRatioMap 		= params["createExpRatioMap"]; 
	double minTreshold 		= params["minTreshold"];
	double maxTreshold 		= params["maxTreshold"];
	double squareSize 		= params["squareSize"];	
 
	       
    // PRINT INPUT PARAMETERS -------------------------------------
	cout << "\n** Inputs: " 	<< endl;
    cout << "outfile: " 		<< outputLogName << endl;
    cout << "imageCtsPath: " 		<< imageCtsPath << endl;
    cout << "classificationThreshold: " << classificationThreshold << endl;
    cout << "imageExpPath: " 		<< imageExpPath << endl;
    cout << "isExpMapNormalized: " 	<< isExpMapNormalized << endl;
    cout << "createExpNormalizedMap: " 	<< createExpNormalizedMap << endl;
    cout << "createExpRatioMap: " 	<< createExpRatioMap << endl;
    cout << "minTreshold: " 		<< minTreshold << endl;
    cout << "maxThreshold: " 		<< maxTreshold << endl;
    cout << "squareSize: "		<< squareSize << endl;
    
    
	// CORE LOGIC -----------------------------------------------
	GammaRayDetector grd(
							imageCtsPath, 
							outputLogName, 
							classificationThreshold, 
							imageExpPath, 
							isExpMapNormalized,
							createExpNormalizedMap,
							createExpRatioMap, 
							minTreshold, 
							maxTreshold, 
							squareSize
	); 



   	grd.detect();


	cout << endString << endl;


   	return 0;

}

