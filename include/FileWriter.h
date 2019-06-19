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
#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <ctime>
#include <fstream>
#include <sstream>
#include <stdlib.h>

using std::string;
using std::ofstream;
using std::ifstream;

class FileWriter
{
    public:
      static void write2File(string outputFileName, string input, bool append);
      static string convertToString(int number);
      static string convertToString(float number);
      static string convertToString(double number);
      static bool is_empty(string filename);

    private:
      FileWriter();
};

#endif // FILEWRITER_H
