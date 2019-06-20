////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION
//       AGILE Science Tools
//       AG_ap5
//       First release: 2017
//       Authors: Leonardo Baroncelli leonardo.baroncelli@inaf.it, Giancarlo Zollino
//
// NOTICE
//       Any information contained in this software
//       is property of the AGILE TEAM and is strictly
//       private and confidential.
//       Copyright (C) 2005-2019 AGILE Team. All rights reserved.
/*

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
////////////////////////////////////////////////////////////////////////////////////

#include "MapConverter.h"


MapConverter::MapConverter() {


}


DoubleMatrixCustomMap * MapConverter::fitsMapToDoubleMatrix(const char * imagePath)
{

	string imagePathString = imagePath;

	DoubleMatrixCustomMap * image = new DoubleMatrixCustomMap();

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

			image->initializeImage(imagePathString, (int)naxes[0], (int)naxes[1]);


			if (naxis > 2 || naxis == 0)
			{
				fprintf( stderr, "[MapConverter] ERROR 1 - only 1D or 2D images are supported");
				exit(EXIT_FAILURE);
			}
			else
			{
				/* get memory for 1 row */
				pixels = (double *)malloc(naxes[0] * sizeof(double));

				if (pixels == NULL)
				{
					fprintf( stderr, "[MapConverter] ERROR 2 - Memory allocation error");
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

							image->image[row_index][col_index] = (double)pixels[ii];



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
		fprintf( stderr, "[MapConverter] ERROR %d",status);
		exit (EXIT_FAILURE);

	}

 	return image;

}



IntMatrixCustomMap* MapConverter::fitsMapToIntMatrix(const char * imagePath){


	string imagePathString = imagePath;

	IntMatrixCustomMap * map = new IntMatrixCustomMap();



	//CFITSIO
	fitsfile *fptr;   // FITS file pointer, defined in fitsio.h
	int status = 0;   // CFITSIO status value MUST be initialized to zero!
	int bitpix, naxis, ii, anynul;
	long naxes[2] = { 1, 1 }, fpixel[2] = { 1, 1 };
	double *pixels;
	char format[20], hdformat[20];

	if (!fits_open_file(&fptr, imagePath, READONLY, &status))
	{									// 16   , 2     , {166,166}
		if (!fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status))
		{


			map->initializeImage(imagePathString, (int)naxes[0], (int)naxes[1]);



			if (naxis > 2 || naxis == 0)
			{
				fprintf( stderr, "[MapConverter] ERROR 1 - only 1D or 2D images are supported");
				exit(EXIT_FAILURE);
			}
			else
			{
				// get memory for 1 row
				pixels = (double *)malloc(naxes[0] * sizeof(double));

				if (pixels == NULL)
				{
					fprintf( stderr, "[MapConverter] ERROR 2 - Memory allocation error");
					exit(EXIT_FAILURE);
				}
				else
				{
					// loop over all the rows in the image, top to bottom

					int col_index = 0;
					int row_index = 0;
					for (fpixel[1] = naxes[1]; fpixel[1] >= 1; fpixel[1]--)
					{
						if (fits_read_pix(fptr, TDOUBLE, fpixel, naxes[0], NULL, pixels, NULL, &status))  // read row of pixels
							break;  // jump out of loop on error

						for (ii = 0; ii < naxes[0]; ii++)
						{

							map->image[row_index][col_index] = (int)pixels[ii];
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
		fprintf( stderr, "[MapConverter] ERROR %d",status);
		exit (EXIT_FAILURE);

	}

 	return map;
}
