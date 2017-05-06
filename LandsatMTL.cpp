// Routine to parse a LANDSAT L1T MTL file & pull selected fields
//
// D.Steinwand/USGS, October 2008
// -----------------------------------------
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <iterator>
#include "LandsatMTL.h"

// Open the MTL file, parse and store keys & values
// ------------------------------------------------
metaDataL1T::metaDataL1T(char *fileName)
{
   ifstream inMTL(fileName);
   if (!inMTL) {
      perror(fileName);
      return;
   }

   // Read the file and spit it to stdout
   // -----------------------------------
   string theLine;			// A line of data from the file
   string theLHS;			// Left Hand Side of the "name = value" field
   string theRHS;			// Right Hand Side of the "name = value" field
   string::size_type equalPos;	// Position in string of the "=" sign
   string::size_type startPos;	// Start of data (past the whitespace)

   // Parse the MTL file, placeing key, value pairs in a map<string,string> container
   // -------------------------------------------------------------------------------
   while (getline(inMTL, theLine)) {
      startPos = theLine.find_first_not_of(" ");   // find the start of the data, not whitespace
      equalPos = theLine.find('=');		   // find where the '=' sign is
      if (equalPos == string::npos) {	   // if there's no '=' sign, we're done
         break;
      }
      else {		// Parse the data, not storing the tags "GROUP" and "END_GROUP"
         theLHS = theLine.substr(startPos, equalPos-(startPos+1));		// assumes a space before the '=' sign
         if ((theLHS.compare("GROUP") != 0) && (theLHS.compare("END_GROUP") != 0)) {
            theRHS = theLine.substr(equalPos+2);
            //cout << theLHS << " = " << theRHS << endl;
            theMetaData[theLHS] = theRHS;
         }
      }
   }
   // Close files and exit
   // --------------------
   inMTL.close();
}

// Grab Field and return the value string
// --------------------------------------
string metaDataL1T::get(string keyVal)
{
   map<string,string>::iterator pos;

   pos = theMetaData.find(keyVal);
   if (pos != theMetaData.end()) {
      return pos->second;
   }
   else {
      //cout << "Field not found.  type end to quit...\n";
      return "NOT_FOUND";
   }
}
