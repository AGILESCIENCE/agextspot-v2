/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
 * 
 * https://github.com/Leofaber/MapConverter
*/


#include "FitsUtils.h"
#include <iostream>
#include <string>


class MapConverter
{
	public:

		
		// Convert an image in a **double matrix
		static double ** mapPathToDoublePtr(const char * imagePath);

		// Convert fits image in readble fits image	
		static bool addAgileMandatoryFitsKeys(const char * imagePath); 



	private:
		// Constructor
		MapConverter();
};
