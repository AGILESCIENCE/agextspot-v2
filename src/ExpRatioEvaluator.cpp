/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/

#include "ExpRatioEvaluator.h"


using namespace std; 

ExpRatioEvaluator::ExpRatioEvaluator(const char * _expPath) 
{
	expPath=_expPath;
	
	agileMap=new AgileMap(expPath);
	
	tStart=agileMap->GetTstart();
	tStop=agileMap->GetTstop();
	timeFactor=tStop-tStart;
 
	double cdelt2=agileMap->GetYbin();
	size = 10/cdelt2;
	
	
	spatialFactor = 0.0003046174197867085688996857673060958405*cdelt2*cdelt2;
	normalizationFactor = spatialFactor*timeFactor;
	
	cout << "normalization factor: " << normalizationFactor<<endl;
	if(! convertFitsDataToMatrix() )
	{
		fprintf( stderr, "convertFitsDataToMatrix() Error reading fits file\n");
		exit (EXIT_FAILURE);
	}
} 


bool ExpRatioEvaluator::convertFitsDataToMatrix()
{
	


	//CFITSIO
	fitsfile *fptr;   /* FITS file pointer, defined in fitsio.h */
	int status = 0;   /* CFITSIO status value MUST be initialized to zero! */
	int bitpix, naxis, ii, anynul;
	long naxes[2] = { 1, 1 }, fpixel[2] = { 1, 1 };
	double *pixels;
	char format[20], hdformat[20];
			

	if (!fits_open_file(&fptr, expPath, READONLY, &status))
	{									// 16   , 2     , {166,166}
		if (!fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status))
		{
			if (naxis > 2 || naxis == 0)
			{
				printf("Error: only 1D or 2D images are supported\n");
				return false;
			}			
			else
			{	 
				rows = (int)naxes[0]; 
				cols = (int)naxes[1];
				image = new double*[rows];
 				normalizedImage = new double*[rows];

				for (int i = 0; i < rows; ++i){
					image[i] = new double[cols];
					normalizedImage[i] = new double[cols];
				}

				/* get memory for 1 row */
				pixels = (double *)malloc(naxes[0] * sizeof(double));

				if (pixels == NULL)
				{
					printf("Memory allocation error - Press any key to exit\n");
					return false;
				}
				else
				{
					/* loop over all the rows in the image, top to bottom */

					int col_index = 0;
					int row_index = 0;
					for (fpixel[1] = naxes[1]; fpixel[1] >= 1; fpixel[1]--)
					{
						if (fits_read_pix(fptr, TDOUBLE, fpixel, naxes[0], NULL, pixels, NULL, &status))  /* read row of pixels */
							break;  /* jump out of loop on error */

						for (ii = 0; ii < naxes[0]; ii++)
						{
							
							image[row_index][col_index] = (double)pixels[ii];
							normalizedImage[row_index][col_index] = (double)pixels[ii]/normalizationFactor;
							//cout << normalizedImage[row_index][col_index] << " ";
							col_index++;
						}
						col_index = 0;
						row_index++;
						//cout << "\n";
					}

					free(pixels);
				}
			}

		}

		fits_close_file(fptr, &status);

	}
	if (status>0)
	{
		printf("Can't open fits file - Press any key to exit\n");
		return false;	
	}

	return true;

}


bool ExpRatioEvaluator::isRectangleInside(int x, int y) 
{
	double distSx;
	double distDx;
	double distUp;
	double distDown;

	distSx =  pow(pow(double(0-x),2),0.5);
	distDx =  pow(pow(double(cols-1-x),2),0.5);
	distUp =  pow(pow(double(0-y),2),0.5);
	distDown =  pow(pow(double(rows-1-y),2),0.5);
	if(distSx < size || distDx < size || distUp < size || distDown < size)
		return false;
	else
		return true;

	
}

double* ExpRatioEvaluator::computeExpRatioValues(double l, double b, bool onNormalizeMap, double minThreshold, double maxThreshold) 
{ 
		
	cout << "MinThreshold: "<< minThreshold << endl;
	cout << "MaxThreshold: " << maxThreshold << endl;
	if(onNormalizeMap)
		cout << "ExpRatioEvaluator will compute exp-ratio value of the normalized map" << endl;
	else	
		cout << "ExpRatioEvaluator will compute exp-ratio value of the NO-normalized map" << endl;	

 	int xmin, xmax, ymin,ymax;
	int npixel = 0;
	int nBad = 0;
	double totCount=0;
	double tmp = 0;
	double expRatio = 0;
	double greyLevelSum = 0;
	double mean = 0;
		
	

	int x;
	int y;

	agileMap->GetRowCol(l,b,&x,&y);
	
	xmin = x - size;
	xmax = x + size;
	ymin = y - size;
	ymax = y + size;
	
	
	if(isRectangleInside(x,y)) 
	{		
		for(int i = xmin; i <= xmax; i++) 
		{
			for(int j= ymin; j <= ymax; j++) 
			{	
				totCount+=1;
				if(onNormalizeMap){
 
					tmp=(double)normalizedImage[i][j];
 
				}		
				else
					tmp=(double)image[i][j];
				
				greyLevelSum+=tmp;

				if(tmp < minThreshold || tmp >maxThreshold)
					nBad+=1;
			}
		}
	
		output[0] = (1-(nBad/totCount))*100;
		output[1] = nBad;
		output[2] = totCount;
		output[3] = greyLevelSum/totCount;
		return output;

	}else 
	{
		fprintf( stderr, "Rectangle is not completely inside!\n");
		output[0] =  -1;
		output[1] =  -1;
		output[2] =  -1;
		output[3] =  -1;
		return output;

	}
}

double ** ExpRatioEvaluator::getNormalizedImage(){
	return normalizedImage;
}

int ExpRatioEvaluator::getRows(){
	return rows;
}
int ExpRatioEvaluator::getCols(){
	return cols;
}

