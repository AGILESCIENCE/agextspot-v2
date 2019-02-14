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

#include "FileWriter.h"

FileWriter::FileWriter()
{
    //ctor
}

string FileWriter::convertToString(int number){
	std::stringstream out;
	out << std::fixed << number;
	return out.str();
}
string FileWriter::convertToString(float number){
	std::stringstream out;
	out << std::fixed << number;
	return out.str();
}
string FileWriter::convertToString(double number){
 	std::stringstream out;
	out << std::fixed << number;
	return out.str();
}

void FileWriter::write2File(string outputFileName, string input, bool append){
	ofstream resultOfAnalysis;
	const char * outputfilenamecstr = outputFileName.c_str();

	if(append)
		resultOfAnalysis.open(outputfilenamecstr, std::ofstream::out | std::ofstream::app);
	else
		resultOfAnalysis.open(outputfilenamecstr, std::ofstream::out | std::ofstream::trunc);


	if(resultOfAnalysis.is_open()){
		resultOfAnalysis << input;
		resultOfAnalysis.close();

	}else{
		printf("[FileWriter] Can't open file %s\n",outputfilenamecstr);
		exit(EXIT_FAILURE);
	}
}
