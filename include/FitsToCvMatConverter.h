#pragma once

#include "fitsio.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

class FitsToCvMatConverter
{
public:
	static Mat convertFitsToCvMat(string fitsPath);
	static int getObservationTimeFromFits(string fitsPath);
    static int extractObservationTime(char * card);


private:
    FitsToCvMatConverter();

};
