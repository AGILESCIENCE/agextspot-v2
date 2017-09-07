#include "GammaRayDetector.h"
//#include <time.h>


/// GIT ADDRESS
/// https://github.com/Leofaber/agextspot-v2
/// Authors: Baroncelli Leonardo, Zollino Giancarlo

using namespace std;



int main(int argc, char*argv[]){

    if( argc > 3) {



        string imagePath = (string)argv[1];
        string outputLogName = (string)argv[2];
        float classificationThreshold = atof(argv[3]);


        //cout << "\n* AGILE GAMMA RAY BURST DETECTOR *" << endl;

        //clock_t tStart = clock();

        GammaRayDetector grd(imagePath,outputLogName,classificationThreshold);

        grd.detect();

       // printf("Time taken: %.2fs", (double)(clock() - tStart)/CLOCKS_PER_SEC);


    }
    else if( argc > 4 ) {
        cout << "Too many arguments supplied.\n  -the image path \n   -the name of output log file\n   -the classification threshold\nExample: ./agextspot ../images/map.cts  results  95" << endl;

    }
    else {
        cout << "Three arguments expected.\n   -the image path \n   -the name of output log file\n   -the classification threshold\nExample: ./agextspot ../images/map.cts  results  95" << endl;
    }
    return 0;

}

