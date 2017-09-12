#pragma once

#include "fitsio.h"
#include <opencv2/core/core.hpp>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

class FitsToCvMatConverter
{
public:
	static Mat convertFitsToCvMat(string fitsPath);
	static string getObservationTimeFromFits(string fitsPath);
    static int extractObservationTime(char * card);

    static string getObservationDateFromFitsFile(string fitsPath);
    static string extractObservationDateFromCard(char * card);


private:
    FitsToCvMatConverter();

};
