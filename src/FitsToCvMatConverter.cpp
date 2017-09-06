#include "FitsToCvMatConverter.h"


FitsToCvMatConverter::FitsToCvMatConverter()
{
}

int FitsToCvMatConverter::getObservationTimeFromFits(string fitsPath){

    char * image_path = new char[fitsPath.length() + 1];
	strcpy(image_path, fitsPath.c_str());

	int tstart;
	int tstop;

    fitsfile *fptr;
    char card[FLEN_CARD];
    int status = 0,  nkeys, ii;  /* MUST initialize status */
    fits_open_file(&fptr,  image_path, READONLY, &status);
    fits_get_hdrspace(fptr, &nkeys, NULL, &status);

    for (ii = 1; ii <= nkeys; ii++)  {
        fits_read_record(fptr, ii, card, &status); /* read keyword */
        //printf("%s\n", card[0]);
        if( card[0]=='T' && card[1]=='S' && card[2]=='T' && card[3]=='A'){
            tstart = extractObservationTime(card);
        }
        if( card[0]=='T' && card[1]=='S' && card[2]=='T' && card[3]=='O'){
            tstop = extractObservationTime(card);
        }
    }
    fits_close_file(fptr, &status);
    if (status)          /* print any error messages */
        fits_report_error(stderr, status);


    cout << tstop-tstart <<" seconds" <<endl;
    return tstop-tstart;
}

int FitsToCvMatConverter::extractObservationTime(char * card){
    //cout << card << endl;
    string str_card(card);

    int last_index;
    int start_index;

    // cerco la fine del numero
    int index = 0;
    while(str_card[index] != '.'){
        index++;
        //cout << str_card[index] << endl;
    }
    last_index = index-1;

    // cerco l'inizio del numero
    while(str_card[index] != ' '){
        index--;
        //cout << str_card[index] << endl;
    }
    start_index = index+1;

    // costruisco il numero
    int size = last_index - start_index + 1;
    //cout << size << endl;
    char num_array[size];
    for(int i = start_index; i<=last_index; i++){
        num_array[i-start_index] = str_card[i];
    }

 //   printf("Number is %d ", atoi(num_array));
    return atoi(num_array);
}

Mat FitsToCvMatConverter::convertFitsToCvMat(string fitsPath)
{

	char * image_path = new char[fitsPath.length() + 1];
	strcpy(image_path, fitsPath.c_str());

	//CFITSIO
	fitsfile *fptr;   /* FITS file pointer, defined in fitsio.h */
	int status = 0;   /* CFITSIO status value MUST be initialized to zero! */
	int bitpix, naxis, ii, anynul;
	long naxes[2] = { 1, 1 }, fpixel[2] = { 1, 1 };
	double *pixels;
	char format[20], hdformat[20];

	//OPENCV
	Mat* image;

	if (!fits_open_file(&fptr, image_path, READONLY, &status))
	{									// 16   , 2     , {166,166}
		if (!fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status))
		{
			if (naxis > 2 || naxis == 0)
				printf("Error: only 1D or 2D images are supported\n");
			else
			{	//OPENCV
				image = new Mat(naxes[0], naxes[1], CV_8UC1, Scalar(0));

				/* get memory for 1 row */
				pixels = (double *)malloc(naxes[0] * sizeof(double));

				if (pixels == NULL)
				{
					printf("Memory allocation error - Press any key to exit\n");
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

						for (ii = 0; ii < naxes[0]; ii++){
							image->at<uchar>(row_index, col_index) = (int)pixels[ii];
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
	if (status>0){
		cout << "Error: can't open fits file." << endl;
	}

	return *image;
}



FitsData FitsToCvMatConverter::extractImageAndObsDateFromFitsFile(string fitsFilePath){

    char * image_path = new char[fitsFilePath.length() + 1];
	strcpy(image_path, fitsFilePath.c_str());


	/// FITS DATA


	Mat * image = nullptr;
	DateObsEnd dateObsEnd;
    dateObsEnd.dateObs=" ";
    dateObsEnd.dateEnd=" ";

  	FitsData fitsData;
    fitsData.observationDates = dateObsEnd;

	//CFITSIO
	fitsfile *fptr;   /* FITS file pointer, defined in fitsio.h */
	int status = 0;   /* CFITSIO status value MUST be initialized to zero! */
	int bitpix, naxis, ii, anynul;
	long naxes[2] = { 1, 1 }, fpixel[2] = { 1, 1 };
	double *pixels;
	char format[20], hdformat[20];


    char card[FLEN_CARD];
    int nkeys;  /* MUST initialize status */

	if (!fits_open_file(&fptr, image_path, READONLY, &status))
	{

        /// TO EXTRACT DATES
	    if(! fits_get_hdrspace(fptr, &nkeys, NULL, &status)){
            for (ii = 1; ii <= nkeys; ii++)  {
                if(! fits_read_record(fptr, ii, card, &status)){/* read keyword */
                    if( card[0]=='D' && card[1]=='A' && card[2]=='T' && card[3]=='E' && card[4]=='-' && card[5]=='O'){
                         fitsData.observationDates.dateObs = extractDateObsEnd(card);
                    }
                    if( card[0]=='D' && card[1]=='A' && card[2]=='T' && card[3]=='E' && card[4]=='-' && card[5]=='E'){
                        fitsData.observationDates.dateEnd = extractDateObsEnd(card);
                    }
                }
            }
	    }


        /// TO EXTRACT IMAGE             // 16   , 2     , {166,166}
		if (!fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status))
		{
			if (naxis > 2 || naxis == 0)
				printf("Error: only 1D or 2D images are supported\n");
			else
			{	//OPENCV
                image = new Mat(naxes[0], naxes[1], CV_8UC1, Scalar(0));

				/* get memory for 1 row */
				pixels = (double *)malloc(naxes[0] * sizeof(double));

				if (pixels == NULL)
				{
					printf("Memory allocation error - Press any key to exit\n");
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

						for (ii = 0; ii < naxes[0]; ii++){
                            image->at<uchar>(row_index, col_index) = (int)pixels[ii];
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


    fitsData.image = *image;

	return 	fitsData;


}
