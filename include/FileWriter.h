#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <ctime>
#include <fstream>

using namespace std;



class FileWriter
{
    public:
        static void write2FileHeader(string input, string observationDate, string outputFile,float classificationThreshold);
        static void write2FileBody(string input, string outputFile);
        static void write2SourceFile(string pathToFile,string input);
    protected:
    private:

        FileWriter();
};

#endif // FILEWRITER_H
