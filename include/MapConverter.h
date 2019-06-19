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

#ifndef MAPCONVERTER_H
#define MAPCONVERTER_H

#include "FitsUtils.h"

#include <healpix_map.h>
#include <healpix_base.h>
#include <healpix_map_fitsio.h>

#include <iostream>
#include <string>
#include <math.h>

using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ostringstream;

class IntMatrixCustomMap {

	public:
		int ** image;
		int rows;
		int cols;
		string imagePath;

		IntMatrixCustomMap(){
			rows = 0;
			cols = 0;
			imagePath = "";
		}


		~IntMatrixCustomMap(){
			for (  int i = 0; i < rows; i++ ){
				delete [] image[i];
			}
			delete [] image;
			image = 0;
		}


		void initializeImage(string imgPath, int r, int c){
			imagePath = imgPath;
			rows = r;
			cols = c;
			image = new int*[rows];
			for (int y = 0; y < rows; ++y){
				image[y] = new int[cols];
			}
		}

};


class DoubleMatrixCustomMap {

	public:
		double ** image;
		int rows;
		int cols;
		string imagePath;

		DoubleMatrixCustomMap(){
			rows = 0;
			cols = 0;
			imagePath = "";
		}


		~DoubleMatrixCustomMap(){
			for (  int i = 0; i < rows; i++){
				delete [] image[i];
			}
			delete [] image;
			image = 0;
		}


		void initializeImage(string imgPath, int r, int c){
			imagePath = imgPath;
			rows = r;
			cols = c;
			image = new double*[rows];
			for (int y = 0; y < rows; ++y){
				image[y] = new double[cols];
			}
		}

};


class MapConverter
{
	public:


		// Convert an image in a **double matrix
		static DoubleMatrixCustomMap * fitsMapToDoubleMatrix(const char * fitsImagePath);

		// Convert an image in a **int matrix
		static IntMatrixCustomMap * fitsMapToIntMatrix(const char * fitsImagePath);



	private:
		// Constructor
		MapConverter();
};

#endif
