#include "FileWriter.h"

FileWriter::FileWriter()
{
    //ctor
}



void FileWriter::write2FileHeader(string input, string outputFile,float classificationThreshold) {
    ofstream resultOfAnalysis;

    time_t rawtime;
    struct tm * timeinfo;
    char date[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(date,sizeof(date),"%d-%m-%Y %I:%M:%S",timeinfo);
    std::string str(date);

    resultOfAnalysis.open (outputFile, std::fstream::app);
    resultOfAnalysis << "\n------------------------------------------------------\n";
    resultOfAnalysis << "Analysis of: " << input << " -D: " << date <<" -T: "<<classificationThreshold*100<<"\n";
    resultOfAnalysis.close();


}

void FileWriter::write2SourcesFile(string filename, string input, string outputFile) {
    ofstream resultOfAnalysisOnlySources;
    string outputFileOnlySources = outputFile+"_onlySources";


    resultOfAnalysisOnlySources.open (outputFileOnlySources, std::fstream::app);
    resultOfAnalysisOnlySources << "\n------------------------------------------------------\n";
    resultOfAnalysisOnlySources << "Analysis of: " << filename <<"\n";
    resultOfAnalysisOnlySources << input;
    resultOfAnalysisOnlySources.close();
}



void FileWriter::write2FileBody(string input, string outputFile) {
    ofstream resultOfAnalysis;

    ofstream resultOfAnalysisOnlySources;
    string outputFileOnlySources = outputFile+"_onlySources";

    resultOfAnalysis.open (outputFile, std::fstream::app);
    resultOfAnalysis << "\n" + input;
    resultOfAnalysis.close();

}
