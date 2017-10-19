
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

#include "GammaRayDetector.h"


using namespace std;

const char* startString = {
"################################################################\n"
"###                   Task AG_extspot v1.0.3 -               ###"
};

const char* endString = {
"### Task AG_extspot exiting .................................###\n"
"################################################################"
};

int main(int argc, char*argv[]){
	
	cout << startString << endl;
		

	// PARAMETRI OPZIONALI (VALORI DI DEFAULT)
	float minTreshold = 120;
	float maxTreshold = 140;
	int squareSize = 10;
	bool doExpEvaluationOnNormalizedMap = true;
	bool writeExpRatioMap = true;


	// CONTROLLO TROPPO POCHI o TROPPI PARAMETRI
	if( argc < 7 || argc > 10) {
		cout << "\nAt least 6 arguments expected (+ 3 optional) .\n   - The .cts image path \n   - The .exp image path \n   - The name of output log file\n   - The classification threshold\n   - The Normalize boolean (true/false): true -> will perform exp-ratio evaluation on the normalized map\n   - The createExpRatioMap boolean (true/false): true -> will create expRatioMap\n\n   (Optional)\n   - The minThreshold (default value 120)\n   - The maxThreshold (default value 140)\n   - The squareSize (default value 10)\n\nExample: ./agextspot ../images/map.cts ../images/map.ext results 95 true (optional) 125 130 10" << endl;
		cout << 	endString << endl;    
		exit (EXIT_FAILURE);	
	}


	
	// SETTAGGIO PARAMETRI OBBLIGATORI
    const char * imageCtsPath = argv[1];
    const char * imageExpPath = argv[2];
    const char * outputLogName = argv[3];
    float classificationThreshold = atof(argv[4]);
    const char *normalize = argv[5];
    const char *createExpRatioMap = argv[6];
	
		
	if( strcmp(normalize, "true") == 0 )
		doExpEvaluationOnNormalizedMap = true;
	else
		doExpEvaluationOnNormalizedMap = false;
		
	if(strcmp(createExpRatioMap, "true") == 0)
		writeExpRatioMap = true;
	else
		writeExpRatioMap = false;
	
	// SETTAGGIO PARAMETRI OPZIONALI

    	if(argc == 8)
		{	
			if(((string)argv[7])!="d")
				minTreshold = atof(argv[7]);
		}
		else if(argc == 9)
		{
			if(((string)argv[7])!="d")
				minTreshold = atof(argv[7]);
			if(((string)argv[8])!="d")
				maxTreshold = atof(argv[8]);
		}
		else if(argc == 10)
		{
			if(((string)argv[7])!="d")
				minTreshold = atof(argv[7]);
			if(((string)argv[8])!="d")
				maxTreshold = atof(argv[8]);
			if(((string)argv[9])!="d")
				squareSize = atof(argv[9]);
		}

	
	//cout << "paramaters: " << argc << endl;
    //cout << "minTreshold: " << minTreshold << endl;
    //cout << "maxTreshold: " << maxTreshold << endl; 
    //cout << "squareSize: " << squareSize << endl; 
        
     
    

	GammaRayDetector grd(imageCtsPath, outputLogName, classificationThreshold, imageExpPath, doExpEvaluationOnNormalizedMap,writeExpRatioMap, minTreshold, maxTreshold, squareSize); 

    grd.detect();


	cout << endString << endl;
    return 0;

}

