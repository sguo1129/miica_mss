//  parseMTL
// 
//  Parses an MTL file to get Landfire/MTBS/MRLC specific information
//
//  Dan Steinwand, USGS/EROS, January 8, 2009
//  Song Guo, USGS/EROS, May 18, 2017, modified the code to work for Landsat MSS data
// -----------------------------------------------------------------------------
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include "LandsatMTL.h"
#include "utils.h"

#define MIN_DELTA 1e-6

main(int argc, char *argv[]) {

   cout << "\nparseMTL v1.02, April 27, 2017\n";

   // Parse input parameters, open files, and allocate buffers
   // --------------------------------------------------------
   if (argc != 2) {
      cout << "  Use:  parseMTL MTLfile\n";
      cout << "          where:\n";
      cout << "              MTLfile is the full name of the L1T MTL file\n";
      return EXIT_FAILURE;
   }

   // Grab parameters and set/calculate constants
   // -------------------------------------------
   metaDataL1T *myMTL = new metaDataL1T(argv[1]);

    string input(argv[1]);
    string sceneName = argv[1];
    string path;
    path = input.substr(0, input.find_last_of("\\/"));

    const size_t last_slash_idx = sceneName.find_last_of("\\/");
    if (string::npos != last_slash_idx)
    {
        sceneName.erase(0, last_slash_idx + 1);
    }

   // Get the scene date
   // ------------------
   string aqDate = myMTL->get("DATE_ACQUIRED");	
   int year = getYear(aqDate);						
   int month = getMonth(aqDate);
   int day = getDay(aqDate);

   // Get the scene time
   // ------------------
   string aqTime = myMTL->get("SCENE_CENTER_TIME");

   // Get the satellite & sensor
   // --------------------------
   string satID = stripQ(myMTL->get("SPACECRAFT_ID"));
   string sensor = stripQ(myMTL->get("SENSOR_ID"));

   // Get the UL and LR projection corner coordinates
   // -----------------------------------------------
   double ULx = atof(myMTL->get("CORNER_UL_PROJECTION_X_PRODUCT").c_str());
   double ULy = atof(myMTL->get("CORNER_UL_PROJECTION_Y_PRODUCT").c_str());
   double LRx = atof(myMTL->get("CORNER_LR_PROJECTION_X_PRODUCT").c_str());
   double LRy = atof(myMTL->get("CORNER_LR_PROJECTION_Y_PRODUCT").c_str());

   // Get LMin and LMax values for each band
   // --------------------------------------
   vector<float> LMax;
   vector<float> LMin;
   string theKey;
   for (int i=1; i<=4; ++i) {
       if (sceneName.at(2) == '1' || sceneName.at(2) == '2' || sceneName.at(2) == '3')
       {
           theKey = "RADIANCE_MAXIMUM_BAND_" + itos(i+3);
           LMax.push_back(atof((myMTL->get(theKey).c_str())));
           theKey = "RADIANCE_MINIMUM_BAND_" + itos(i+3);
           LMin.push_back(atof((myMTL->get(theKey).c_str())));
       }
       else if (sceneName.at(2) == '4' || sceneName.at(2) == '5')
       {
           theKey = "RADIANCE_MAXIMUM_BAND_" + itos(i);
           LMax.push_back(atof((myMTL->get(theKey).c_str())));
           theKey = "RADIANCE_MINIMUM_BAND_" + itos(i);
           LMin.push_back(atof((myMTL->get(theKey).c_str())));
       }
       else 
       {
           cout << "Unsupport sensor number for MSS data" << endl;
           return EXIT_FAILURE;
       }
   }

   // Get QCalMin and QCalMax values for each band
   // --------------------------------------------
   vector<float> QCalMax;
   vector<float> QCalMin;
   for (int i=1; i<=4; ++i) {
       if (sceneName.at(2) == '1' || sceneName.at(2) == '2' || sceneName.at(2) == '3')
       {
           theKey = "QUANTIZE_CAL_MAX_BAND_" + itos(i + 3);
           QCalMax.push_back(atof((myMTL->get(theKey).c_str())));
           theKey = "QCALMIN_CAL_MIN_BAND_" + itos(i + 3);
           QCalMin.push_back(atof((myMTL->get(theKey).c_str())));
       }
       else if (sceneName.at(2) == '4' || sceneName.at(2) == '5')
       {
           theKey = "QUANTIZE_CAL_MAX_BAND_" + itos(i);
           QCalMax.push_back(atof((myMTL->get(theKey).c_str())));
           theKey = "QCALMIN_CAL_MIN_BAND_" + itos(i);
           QCalMin.push_back(atof((myMTL->get(theKey).c_str())));
       }
       else 
       {
           cout << "Unsupport sensor number for MSS data" << endl;
           return EXIT_FAILURE;
       }
   }

   // Get the filenames for each band
   // -------------------------------
   vector<string> fName;
   for (int i=1; i<=4; ++i) {
       if (sceneName.at(2) == '1' || sceneName.at(2) == '2' || sceneName.at(2) == '3')
       {
           theKey = "FILE_NAME_BAND_" + itos(i + 3);
           fName.push_back(path + '/' + stripQ(myMTL->get(theKey)));
       }
       else if (sceneName.at(2) == '4' || sceneName.at(2) == '5')
       {
           theKey = "FILE_NAME_BAND_" + itos(i);
           fName.push_back(path + '/' + stripQ(myMTL->get(theKey)));
       }
       else 
       {
           cout << "Unsupport sensor number for MSS data" << endl;
           return EXIT_FAILURE;
       }
   }

   // Create output file names for each band
   // --------------------------------------
   vector<string> fNameOut;
   for (int i=0; i<4; ++i) {
           fNameOut.push_back(fName[i].substr(0, fName[i].find('.')) + "_info.txt");
   }

   for (int i=0; i<4; ++i) {
       if (sceneName.at(2) == '1' || sceneName.at(2) == '2' || sceneName.at(2) == '3')
       {
           ofstream to(fNameOut[i].c_str());
           if (!to) {
               cout << "   ERROR:  Cannot open output file " << fNameOut[i] << endl;
               return EXIT_FAILURE;
	   }
           to << "FileName: " << fName[i] << endl;
           to << "Band:     " << i+4 << endl;
           to << "Spacecraft:  " << satID << endl;
           to << "Sensor:      " << sensor << endl;
           to << "Acquisition-Date:    " << aqDate << endl;
           to << "Acquisition-Time:    " << aqTime << endl;
           to << "Sun-Elevation:       " << myMTL->get("SUN_ELEVATION") << endl;
	   if (QCalMax[i] < MIN_DELTA && QCalMin[i] < MIN_DELTA)
	   {
               cout << "Band " << i+4 << "does not have valid data" << endl;
               cout << "Not enough bands (4) for MICCA algorithm working well" << endl;
               cout << "Exiting..." << endl;               
               return EXIT_FAILURE;
	   }
           else
	   {
               float bias = LMin[i] - (((LMax[i] - LMin[i])/(QCalMax[i] - QCalMin[i])) * QCalMin[i]) ;
               float gain = (LMax[i]-LMin[i])/(QCalMax[i]-QCalMin[i]);
               to << "Gain:     " << gain << endl;
               to << "Bias:     " << bias << endl;
               to << "LMin:     " << LMin[i] << endl;
               to << "LMax:     " << LMax[i] << endl;
               to << "QCalMin:  " << QCalMin[i] << endl;
               to << "QCalMax:  " << QCalMax[i] << endl;
               to.setf(ios_base::fixed, ios_base::floatfield);
               to << "ULx:      " << ULx << endl;
               to << "ULy:      " << ULy << endl;
               to << "LRx:      " << LRx << endl;
               to << "LRy:      " << LRy << endl;
               to.close();
	   }
       }
       else if (sceneName.at(2) == '4' || sceneName.at(2) == '5')
       {
           ofstream to(fNameOut[i].c_str());
           if (!to) {
               cout << "   ERROR:  Cannot open output file " << fNameOut[i] << endl;
               return EXIT_FAILURE;
	   }
           to << "FileName: " << fName[i] << endl;
           if (sceneName.at(2) == '1' || sceneName.at(2) == '2' || sceneName.at(2) == '3')
               to << "Band:     " << i+4 << endl;
           else if (sceneName.at(2) == '4' || sceneName.at(2) == '5')
               to << "Band:     " << i+1 << endl;
           else 
           {
               cout << "Unsupport sensor number for MSS data" << endl;
               return EXIT_FAILURE;
           }	   
           to << "Spacecraft:  " << satID << endl;
           to << "Sensor:      " << sensor << endl;
           to << "Acquisition-Date:    " << aqDate << endl;
           to << "Acquisition-Time:    " << aqTime << endl;
           to << "Sun-Elevation:       " << myMTL->get("SUN_ELEVATION") << endl;
	   if (QCalMax[i] < MIN_DELTA && QCalMin[i] < MIN_DELTA)
	   {
               cout << "Band " << i+4 << "does not have valid data" << endl;
               cout << "Not enough bands (4) for MICCA algorithm working well" << endl;
               cout << "Exiting..." << endl;               
               return EXIT_FAILURE;
	   }
           else
	   {
               float bias = LMin[i] - (((LMax[i] - LMin[i])/(QCalMax[i] - QCalMin[i])) * QCalMin[i]) ;
               float gain = (LMax[i]-LMin[i])/(QCalMax[i]-QCalMin[i]);
               to << "Gain:     " << gain << endl;
               to << "Bias:     " << bias << endl;
               to << "LMin:     " << LMin[i] << endl;
               to << "LMax:     " << LMax[i] << endl;
               to << "QCalMin:  " << QCalMin[i] << endl;
               to << "QCalMax:  " << QCalMax[i] << endl;
               to.setf(ios_base::fixed, ios_base::floatfield);
               to << "ULx:      " << ULx << endl;
               to << "ULy:      " << ULy << endl;
               to << "LRx:      " << LRx << endl;
               to << "LRy:      " << LRy << endl;
               to.close();
	   }
       }
       else 
       {
           cout << "Unsupport sensor number for MSS data" << endl;
           return EXIT_FAILURE;
       }
   }
   delete myMTL;
   myMTL = NULL;
}

