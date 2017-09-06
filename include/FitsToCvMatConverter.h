#pragma once

#include "fitsio.h"
#include <opencv2/core/core.hpp>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;


struct DateObsEnd{
    string dateObs;
    string dateEnd;
};

struct FitsData{
    Mat image;
    DateObsEnd observationDates;
};

class FitsToCvMatConverter
{
public:
	static Mat convertFitsToCvMat(string fitsPath);
	static int getObservationTimeFromFits(string fitsPath);
    static int extractObservationTime(char * card);

    static FitsData extractImageAndObsDateFromFitsFile(string fitsPath);


private:
    FitsToCvMatConverter();

};
