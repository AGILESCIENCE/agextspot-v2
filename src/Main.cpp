
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
#include "ExpRatioEvaluator.h"

using namespace std;

const char* startString = {
"################################################################\n"
"###                   Task AG_extspot v1.0.1 -               ###"
};

const char* endString = {
"### Task AG_extspot exiting .................................###\n"
"################################################################"
};

int main(int argc, char*argv[]){
	
	cout << startString << endl;
		
	if( argc < 5 || argc > 7) {
		cout << "\nAt least four arguments expected (+ 2 optional) .\n   -the .cts image path \n   -the .exp image path \n   -the name of output log file\n   -the classification threshold\n\n   (Optional)\n   -minThreshold (default value 0)\n   -maxThreshold (default value 100)\n\nExample: ./agextspot ../images/map.cts ../images/map.ext results  95 (optional) 15 75" << endl;
		cout << 	endString << endl;    
		exit (EXIT_FAILURE);	
	}


	float minTreshold = 0;
	float maxTreshold = 100;

    string imageCtsPath = (string)argv[1];
    const char *imageExpPath = argv[2];
    string outputLogName = (string)argv[3];
    float classificationThreshold = atof(argv[4]);
    
		    
    if(argc >6 && argc <8){
        
		minTreshold = atof(argv[5]);
        cout << "minTreshold: " << minTreshold << endl;
        maxTreshold = atof(argv[6]);
        cout << "maxTreshold: " << maxTreshold << endl; 
        
     }
    

	GammaRayDetector grd(imageCtsPath,outputLogName,classificationThreshold,imageExpPath,minTreshold,maxTreshold); 
    grd.detect();


	cout << 	endString << endl;
    return 0;

}

