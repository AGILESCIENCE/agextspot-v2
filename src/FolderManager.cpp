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

#include "FolderManager.h"

FolderManager::FolderManager()
{
}

vector<string> FolderManager::getFileNamesFromFolder(string folderpath){

    /*OPEN MAP DIRECTORY*/
    vector<string> fileNames;
	DIR *pdir = NULL;
	struct dirent *pent = NULL;
	//cout <<"Folderpath: "<< folderpath << endl;
	pdir = opendir (folderpath.c_str());
    if (pdir == NULL)                   // if pdir wasn't initialised correctly
        {
            cout << "\n[Folder Manager] Error: files path is wrong!";

            exit (3);
        }

     while (pent = readdir (pdir)) // while there is still something in the directory to list
    {
        if (pent == NULL) // if pent has not been initialised correctly
        { // print an error message, and exit the program
            cout << "\n[Folder Manager] Error: folder is empty!";

            exit (3);

        }
        // otherwise, it was initialised correctly. Let's print it on the console:
        if(pent->d_name[0]!='.') {
            fileNames.push_back(pent->d_name);
            //cout << pent->d_name << endl;
        }
    }

    // finally, let's close the directory
    closedir (pdir);

   // cin.get (); // pause for input
    return fileNames; // everything went OK

}
