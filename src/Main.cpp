
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
		
	// DEFINIZIONE PARAMETRI DI DEFAULT ---------------------------------------------
	const char *createExpNormalizedMap = "false";
	const char *createExpRatioMap = "false";
	float minTreshold = 120;
	float maxTreshold = 140;
	double squareSize = 10;
	
	
	// CONTROLLO TROPPO POCHI o TROPPI PARAMETRI
	if( argc < 6 || argc > 11) {
		cout << "\nAt least 5 arguments expected (+ 5 optional) .\n   - The name of output log file\n   - The .cts image path \n   - The classification threshold\n   - The .exp image path \n   -  isExpMapNormalized? : if 'true' we assert that the exp map given in input is already normalized.\n\n(OPTIONAL)\n   - createNormMap? : if 'true' the normalized map will be writed on file (default value = false)\n   - createExpRatioMap? : if 'true' the exp-ratio map will be writed on file. (default value = false)\n   - The minThreshold (default value = 120)\n   - The maxThreshold (default value = 140)\n   - The square size (default value = 10)\n\nExample: ./agextspot log.txt ../images/map.cts 95 ../images/map.exp false 95 true true true 125 130 10" << endl;
		cout << 	endString << endl;    
		exit (EXIT_FAILURE);	
	}


	
	// SETTAGGIO PARAMETRI OBBLIGATORI
	const char * outputLogName = argv[1];
    const char * imageCtsPath = argv[2];
    float classificationThreshold = atof(argv[3]);
    const char * imageExpPath = argv[4];
    const char * isExpMapNormalized = argv[5];
    
    
	
		
	
	
	// SETTAGGIO PARAMETRI OPZIONALI
	if(argc == 7)
	{	
		if(((string)argv[6])!="d")
			createExpNormalizedMap = argv[6];	
	}
	else if(argc == 8)
	{
		if(((string)argv[6])!="d")
			createExpNormalizedMap = argv[6];	
		if(((string)argv[7])!="d")
			createExpRatioMap = argv[7];	
	}
	else if(argc == 9)
	{
		if(((string)argv[6])!="d")
			createExpNormalizedMap = argv[6];
		if(((string)argv[7])!="d")
			createExpRatioMap = argv[7];	
		if(((string)argv[8])!="d")
			minTreshold = atof(argv[8]);	
	}else if(argc == 10) {
		if(((string)argv[6])!="d")
			createExpNormalizedMap = argv[6];
		if(((string)argv[7])!="d")
			createExpRatioMap = argv[7];
		if(((string)argv[8])!="d")
			minTreshold = atof(argv[8]);
		if(((string)argv[9])!="d")
			maxTreshold = atof(argv[9]);
	}
	else if(argc == 11) {
		if(((string)argv[6])!="d")
			createExpNormalizedMap = argv[6];
		if(((string)argv[7])!="d")
			createExpRatioMap = argv[7];
		if(((string)argv[8])!="d")
			minTreshold = atof(argv[8]);
		if(((string)argv[9])!="d")
			maxTreshold = atof(argv[9]);
		if(((string)argv[10])!="d")
			squareSize = atof(argv[10]);
	}

	bool isExpMapNormalizedBool = false;
	if( strcmp(isExpMapNormalized, "true") == 0 )
		isExpMapNormalizedBool = true;
	else
		isExpMapNormalizedBool = false;
	
	bool createExpNormalizedMapBool = false;
	if(strcmp(createExpNormalizedMap, "true") == 0)
		createExpNormalizedMapBool = true;
	else 
		createExpNormalizedMapBool = false;
		
	bool createExpRatioMapBool = false;	
	if(strcmp(createExpRatioMap, "true") == 0)
		createExpRatioMapBool = true;
	else
		createExpRatioMapBool = false;
	
	       
    // PRINT INPUT PARAMETERS -------------------------------------
    cout << "\noutfile: " << outputLogName << endl;
    cout << "imageCtsPath: " << imageCtsPath << endl;
    cout << "classificationThreshold: " << classificationThreshold << endl;
    cout << "imageExpPath: " << imageExpPath << endl;
    cout << "isExpMapNormalized: " << isExpMapNormalized << endl;
    cout << "createExpNormalizedMap: " << createExpNormalizedMap << endl;
    cout << "createExpRatioMap: " << createExpRatioMap << endl;
    cout << "minTreshold: " << minTreshold << endl;
    cout << "maxThreshold: " << maxTreshold << endl;
    cout << "squareSize: " << squareSize << endl;
    
    
	// CORE LOGIC -----------------------------------------------
	GammaRayDetector grd(imageCtsPath, outputLogName, classificationThreshold, imageExpPath, isExpMapNormalizedBool,createExpNormalizedMapBool,createExpRatioMapBool, minTreshold, maxTreshold, squareSize); 

    grd.detect();


	cout << endString << endl;
    return 0;

}

