#include "GammaRayDetector.h"
#include <time.h>


/// GIT ADDRESS
/// https://github.com/Leofaber/agextspot-v2
/// Authors: Baroncelli Leonardo, Zollino Giancarlo

using namespace std;



int main(int argc, char*argv[]){

    if( argc > 3) {



        string fitsFilePath = (string)argv[1];
        string outputLogName = (string)argv[2];
        float classificationThreshold = atof(argv[3]);


        cout << "\n* AGILE GAMMA RAY BURST DETECTOR *" << endl;

        clock_t tStart = clock();

        GammaRayDetector grd(fitsFilePath,outputLogName,classificationThreshold);

        grd.detect();

        printf("\nTime taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);


    }
    else if( argc > 4 ) {
        cout << "Too many arguments supplied.\n  -the image path \n   -the name of output log file\n   -the classification threshold" << endl;

    }
    else {
        cout << "Three arguments expected.\n   -the image path \n   -the name of output log file\n   -the classification threshold\n" << endl;
    }
    return 0;

}

