/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


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
