#include "FileWriter.h"

FileWriter::FileWriter()
{
    //ctor
}



void FileWriter::write2FileHeader(string input, string outputFile,float classificationThreshold) {
    ofstream resultOfAnalysis;


    //time_t rawtime;
    //struct tm * timeinfo;
    //char date[80];

    //time (&rawtime);
    //timeinfo = localtime(&rawtime);

  //  strftime(date,sizeof(date),"%d-%m-%Y %I:%M:%S",timeinfo);
  //  std::string str(date);

    resultOfAnalysis.open (outputFile, std::fstream::app);
    resultOfAnalysis << "\n------------------------------------------------------\n";
    resultOfAnalysis << "DETECTION OF: " << input << " -T: "<<classificationThreshold*100<<"\n";
    resultOfAnalysis.close();
}


void FileWriter::write2FileBody(string input, string outputFile) {
    ofstream resultOfAnalysis;

    resultOfAnalysis.open (outputFile, std::fstream::app);
    resultOfAnalysis << "\n" + input;
    resultOfAnalysis.close();
}

void FileWriter::write2SourceFile(string pathToFile, string input, string outputFile){
    ofstream resultOfAnalysisSources;

    string outputSourcesFile = outputFile+"_sources";

    resultOfAnalysisSources.open (outputSourcesFile, std::fstream::app);
    resultOfAnalysisSources << "\n"+ pathToFile +", "+input;
    resultOfAnalysisSources.close();
}


