
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


	// CONTROLLO TROPPO POCHI o TROPPI PARAMETRI
	if( argc < 6 || argc > 9) {
		cout << "\nAt least five arguments expected (+ 3 optional) .\n   -the .cts image path \n   -the .exp image path \n   -the name of output log file\n   -the classification threshold\n    -boolean (true/false): true -> will perform exp-ratio evaluation on the normalized map\n\n   (Optional)\n   -minThreshold (default value 120)\n   -maxThreshold (default value 140)\n   -squareSize (default value 10)\n\nExample: ./agextspot ../images/map.cts ../images/map.ext results 95 true (optional) 125 130 10" << endl;
		cout << 	endString << endl;    
		exit (EXIT_FAILURE);	
	}


	
	// SETTAGGIO PARAMETRI OBBLIGATORI
    string imageCtsPath = argv[1];
    const char *imageExpPath = argv[2];
    string outputLogName = argv[3];
    float classificationThreshold = atof(argv[4]);
    const char *normalize = argv[5];
	
		
	if( strcmp(normalize, "true") == 0 )
		doExpEvaluationOnNormalizedMap = true;
	else
		doExpEvaluationOnNormalizedMap = false;
	
	// SETTAGGIO PARAMETRI OPZIONALI

    	if(argc == 7)
		{	
			if(((string)argv[6])!="d")
				minTreshold = atof(argv[6]);
		}
		else if(argc == 8)
		{
			if(((string)argv[6])!="d")
				minTreshold = atof(argv[6]);
			if(((string)argv[7])!="d")
				maxTreshold = atof(argv[7]);
		}
		else if(argc == 9)
		{
			if(((string)argv[6])!="d")
				minTreshold = atof(argv[6]);
			if(((string)argv[7])!="d")
				maxTreshold = atof(argv[7]);
			if(((string)argv[8])!="d")
				squareSize = atof(argv[8]);
		}

	
	//cout << "paramaters: " << argc << endl;
    //cout << "minTreshold: " << minTreshold << endl;
    //cout << "maxTreshold: " << maxTreshold << endl; 
    //cout << "squareSize: " << squareSize << endl; 
        
     
    

	GammaRayDetector grd(imageCtsPath, outputLogName, classificationThreshold, imageExpPath, doExpEvaluationOnNormalizedMap, minTreshold, maxTreshold, squareSize); 

    grd.detect();


	cout << endString << endl;
    return 0;

}

