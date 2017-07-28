#include "FolderHandler.h"


FolderHandler::FolderHandler()
{
}


FolderHandler::~FolderHandler()
{
}
/*
vector<string> FolderHandler::readFileNames(string path_to_folder){
DIR *dir;
struct dirent *ent;

vector<string> filenames;



if ((dir = opendir(path_to_folder.c_str())) != NULL) {
// print all the files and directories within directory
while ((ent = readdir(dir)) != NULL) {

string images_path(path_to_folder);
string image_name(ent->d_name);
string fullpath = images_path + "/" + image_name;
//	cout << fullpath << endl;

if (isdigit(image_name.at(0)))
filenames.push_back(fullpath);

}
cout << "Number of files: " << filenames.size() << endl;
closedir(dir);
//	getchar();
return filenames;
}
else {
// could not open directory
perror("");

}

}*/
