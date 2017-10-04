#include <iostream>
#include <stdlib.h> 
#include "fitsio.h" 
#include "AgileMap.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
 
using namespace std;

class ExpRatioEvaluator
{
	public: 

	ExpRatioEvaluator(const char * expPath);


	const char* expPath;
	double normalizationFactor;
	double tStart;
	double tStop;
	double timeFactor;
	double spatialFactor;
	
	
	// If pixel value is not inside [minThreshold, maxThreshold], increments nBad.
	double minThreshold;
	double maxThreshold;
	
	AgileMap* agileMap;

	// The size of the rectangle (x-size , x+size, y-size, y+size)
	float size;

	// Check if the  rectangle is completely inside the image
	bool isRectangleInside(int x, int y);

	// We convert fits data into a matrix of double
	int rows;
	int cols;
	double ** image;
	double ** normalizedImage;

	bool convertFitsDataToMatrix();
	
	// The output array  [ exp-ratio, nBad, nTot, greyLevelMean ]	
	double output[4];



	/*
		Computes and returns the output array. 
		If the rectangle is not entirely inside the image, it returns -1 -1 -1 -1.
	*/	
	double* computeExpRatioValues(double l, double b, bool onNormalizeMap, double minThreshold, double maxThreshold);	

	double ** getNormalizedImage();

	int getRows();
	int getCols();
	
	
};
