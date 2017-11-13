/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#include "GammaRayDetector.h"


GammaRayDetector::GammaRayDetector(const char * _imagePath, 
								   const char * _outputLogName, 
								   double _classificationThreshold, 
								   const char * _imageExpPath, 
								   bool isExpMapNormalizedBool, 
								   bool createExpNormalizedMap,
								   bool createExpRatioMap, 
								   double minTreshold, 
								   double maxTreshold, 
								   double squareSize) :	
 
	reverendBayes(),
	agileMapTool(_imagePath)
	
	
{

	imagePath = _imagePath;
	imageExpPath = _imageExpPath;
	string imageExpPathString = _imageExpPath;
	string outPutLogNameString = _outputLogName;
	
   	if(imageExpPathString == "None"){
		evaluateExpRatio = false;
		cout << "*No exp-ratio evaluation will be performed"<<endl;
	}
	else{
		cout << "*Exp-ratio evaluation will be performed"<<endl;
		evaluateExpRatio = true;
		exp = new ExpRatioEvaluator(_imageExpPath, isExpMapNormalizedBool, createExpNormalizedMap, createExpRatioMap, minTreshold, maxTreshold, squareSize);
	}
		

	/*Check AgileMap*/
	if(agileMapTool.Read(_imagePath) == 202){
		cout << "*ERROR: File "<< _imagePath << " has not AgileMap format." << endl;
		exit(EXIT_FAILURE);
	}
		
	rows = agileMapTool.Rows();
	cols = agileMapTool.Cols();

	
	
   	fileName = extractFileNameFromImagePath(imagePath);

	outputLogName = computeOutputLogName(fileName,outPutLogNameString,minTreshold,maxTreshold,squareSize);
		
   	classificationThreshold = _classificationThreshold/100;
	
}


GammaRayDetector::~GammaRayDetector(){
	/*
		CANCELLARE ctsMap
	*/
	for (  int i = 0; i < rows; i++)
	{
		delete [] ctsMap[i];
	}
	delete [] ctsMap;
	ctsMap = 0;

	for(vector<Blob*>::iterator i = blobs.begin(); i != blobs.end(); i++){
		Blob * b = *i;		
		delete b;
	}
	blobs.clear();
}



void GammaRayDetector::detect()
{

	
	const char * observationDateUTCtemp = agileMapTool.GetStartDate();
	string observationDateUTC = observationDateUTCtemp;
	int observationDateTT = (int)agileMapTool.GetTstart();


	/// converte un file fits in un array 2D
	ctsMap = mapPathToIntPtr(imagePath.c_str());

	/// tira fuori una lista con tutti i BLOBS
	blobs = BlobsFinder::findBlobs(imagePath, ctsMap,rows,cols,agileMapTool.GetXbin() ,agileMapTool.GetYbin() );

	//cout << "SIZE: " << blobs.size()<<endl;

	string information2PrintForSources = "";

	string expRatioString = "";

	int index = 1;
	if(blobs.size() > 0)
	{
		for(vector<Blob*>::iterator i = blobs.begin(); i != blobs.end(); i++)
		{
			information2PrintForSources += to_string(index)+" ";
			index++;
			Blob* b = *i;

			// Classification
			double fluxProbability = classifyBlob(b);

			// Conversion of blob centroid in galactic coordinates
			/*
			CHANGING OF THE REFERENCE SYSTEM FROM Mat to AgileMap
			*/
			double gaLong = agileMapTool.l(b->getCentroid().x, (agileMapTool.Rows()-b->getCentroid().y));
			double gaLat  = agileMapTool.b(b->getCentroid().x, (agileMapTool.Rows()-b->getCentroid().y));


			// ExpRatioEvaluation
			expRatioString="-1 ";
			if(evaluateExpRatio){
				expRatioString = to_string(exp->computeExpRatioValues(gaLong,gaLat))+" ";
			}

			// Building of output
			string tempString = to_string(gaLong)+" "+to_string(gaLat)+" "+to_string(fluxProbability*100)+" "+observationDateUTC+" "+to_string(observationDateTT)+" "+to_string(classificationThreshold*100)+" "+fileName+" "+expRatioString+"\n";

			/// Labeling
			if(fluxProbability >= classificationThreshold){
				information2PrintForSources += "SOURCE "+tempString;
			}else{
				information2PrintForSources += "BG "+tempString;

			}



		}
	}else{

		information2PrintForSources += "NO_BLOBS "+observationDateUTC+" "+to_string(observationDateTT)+" "+to_string(classificationThreshold*100)+" "+fileName+"\n";
	}



	FileWriter::write2File(outputLogName,information2PrintForSources);

	cout << "\n*Created Log File: " << outputLogName << "\n" <<endl;

}




double GammaRayDetector::classifyBlob(Blob* b)
{
    vector<pair<string,double> > predicted = reverendBayes.classify(b);
    //double bgProbability   = predicted[0].second;
    double fluxProbability = predicted[1].second;

    return fluxProbability;
}


///   /ANALYSIS3/NGC4993_SHORT/output/20s_428630400.0_431308800.0/SCAN20_428630420.0_428630440.0_1.cts.gz
string GammaRayDetector::extractFileNameFromImagePath(string imagePath){


    int firstIndex=0;
    int secondIndex=imagePath.size();

    /// FINDING .cts.gz  OR .cts in imagePath
    size_t foundCtsGz = imagePath.find(".cts.gz");

    if (foundCtsGz!=string::npos){
        //cout << ".cts.gz foundCtsGz at: " << foundCtsGz <<endl;
        secondIndex = foundCtsGz;
        imagePath = imagePath.substr(firstIndex, secondIndex);
    } else {

        size_t foundCts = imagePath.find(".cts");
        if (foundCts!=string::npos){
            secondIndex = foundCts;
            imagePath = imagePath.substr(firstIndex, secondIndex);
        }
    }

    /// FINDING SLASH in imagePath
    size_t foundSlash;


    char toDelete2 = '/';
    do{
        foundSlash = imagePath.find(toDelete2);
        //cout << "\nSlash foundSlash at: " << foundSlash << " NPOS: "<<string::npos << endl;
        firstIndex = foundSlash;
        imagePath = imagePath.substr(firstIndex+1, secondIndex);
      //  cout << "\nIMAGE PATH SUBSTRING: " << imagePath << endl;
    }
    while (foundSlash!=string::npos);



    return imagePath;
}

string GammaRayDetector::computeOutputLogName(string _filename, string _outputLogName, double minThreshold, double maxThreshold, double squareSize){
	string outputlogname;

	// FIND .txt in outputLogName.
	size_t foundTxt = _outputLogName.find(".txt");
	if(foundTxt != string::npos)
		outputlogname = _outputLogName.substr(0,foundTxt);
	else
		outputlogname = _outputLogName;
	   

   	outputlogname +="_"+_filename+".txt";
	
	return outputlogname;	


}

int ** GammaRayDetector::mapPathToIntPtr(const char * imagePath)
{
	
	int ** image;
	int rows = 0;
	int cols = 0;
	
	//CFITSIO
	fitsfile *fptr;   /* FITS file pointer, defined in fitsio.h */
	int status = 0;   /* CFITSIO status value MUST be initialized to zero! */
	int bitpix, naxis, ii, anynul;
	long naxes[2] = { 1, 1 }, fpixel[2] = { 1, 1 };
	double *pixels;
	char format[20], hdformat[20];
	
	if (!fits_open_file(&fptr, imagePath, READONLY, &status))
	{									// 16   , 2     , {166,166}
		if (!fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status))
		{
			rows = (int)naxes[0];
			cols = (int)naxes[1];
			image = new int*[rows];
			for (int i = 0; i < rows; ++i){
				image[i] = new int[cols];
			}
			if (naxis > 2 || naxis == 0)
			{	
				fprintf( stderr, "*ERROR 1 - only 1D or 2D images are supported");
				exit(EXIT_FAILURE);
			}			
			else
			{	 
				/* get memory for 1 row */
				pixels = (double *)malloc(naxes[0] * sizeof(double));

				if (pixels == NULL)
				{
					fprintf( stderr, "*ERROR 2 - Memory allocation error");
					exit(EXIT_FAILURE);
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
							
							image[row_index][col_index] = (int)pixels[ii];



							col_index++;
 
						}
						col_index = 0;
						row_index++;
					}

					free(pixels);
				}
			}

		}

		fits_close_file(fptr, &status);

	}
	if (status>0)
	{
		fprintf( stderr, "*ERROR %d",status);
		exit (EXIT_FAILURE);
		
	}

 	return image;

}

