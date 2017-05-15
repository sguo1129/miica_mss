//  CalcTOA
// 
//  Calculates TOA corrections; stored as scaled result (0.0 - 1000.0)
//
//  Dan Steinwand, USGS/EROS, October 15, 2008
//  Dan Steinwand, USGS/EROS, January 12, 2009 -- made more generic; parser is now external
//  Song Guo, USGS/EROS, May 18, 2017, modified the code to work for Landsat MSS data
// ----------------------------------------------------------------------------------------
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include "ImageP_gdal.h"
#include "readInfo.h"
#include "utils.h"

#define SCENE_CENTER_ANGLE	1001
#define ZENITH_SUN		1002

   class ImageTOA: public ImageP {
   public:
      ImageTOA(string inFile, string outFile, imgProps *outProps) : ImageP(inFile, outFile, outProps) {}
      virtual unsigned char eval(int k) {};
      virtual short evalI2(int k);
      void setParms(float pLMin, float pLMax, float pQCalMin, float pQCalMax, double pD, double pCosSolarZ, int pBand,
				string pSatID);
   private:
      double myConst;
      double gain;
      double bias;
      double DNoffset;
   };


void ImageTOA::setParms(float LMin, float LMax, float QCalMin, float QCalMax, double d, double cosSolarZ, int band,
				string pSatID) {
   DNoffset = QCalMin;
   gain = (LMax-LMin)/(QCalMax-QCalMin);
   bias = LMin + (gain * QCalMin);

   // Calculate the image-constant values
   // -----------------------------------
   if (pSatID.compare("LANDSAT_1") == 0)
	 myConst = (PI * d * d) / (esun_mss_1[band] * cosSolarZ);
   if (pSatID.compare("LANDSAT_2") == 0)
         myConst = (PI * d * d) / (esun_mss_2[band] * cosSolarZ);
   if (pSatID.compare("LANDSAT_3") == 0)
         myConst = (PI * d * d) / (esun_mss_3[band] * cosSolarZ);
   if (pSatID.compare("LANDSAT_4") == 0)
         myConst = (PI * d * d) / (esun_mss_4[band] * cosSolarZ);
   if (pSatID.compare("LANDSAT_5") == 0)
         myConst = (PI * d * d) / (esun_mss_5[band] * cosSolarZ);

   cout << "\n   Band" << band << " Correction:  TOAVal = ((" << gain << " * (DN - " << DNoffset << ")) + " << bias << ") * " << myConst << endl;
   cout << "     then, multiply by 10,000.0, add 0.5, and truncate to the nearest integer...\n\n";
}

short ImageTOA::evalI2(int k) { 
   if (fabs(theData[k] < 0.001)) return -500;				// Fill condition
   double val = (gain * ((double)theData[k] - DNoffset)) + bias;
   val *= myConst;
   val *= 10000.0;
   if (val > 10000.0) val = 10000.0;
   if (val < 0.0) val = 0.0;
   //   cout << "val="<<val<<endl;
   return (short)(val + 0.5); 
}


main(int argc, char *argv[]) {

   cout << "\ncalcTOA v6.05, June 29, 2009\n";

   // Parse input parameters, open files, and allocate buffers
   // --------------------------------------------------------
   if (argc != 3) {
      cout << "  Use:  calcTOA switch infoFile\n";
      cout << "          where:\n";
      cout << "              switch is either -sc (use scene center sun angle) or -zs (use a zenith sun)\n";
      cout << "              infoFile is the output of parseMTL or parseWO\n";
      cout << "              *** NOTE:  This version does not process thermal or Pan bands\n";
      return EXIT_FAILURE;
   }

   bandInfo theInfo;
   int sunCalc;

   // Which switch was thrown?
   // ------------------------
   string theSwitch = argv[1];
   if (theSwitch.compare("-sc") == 0) sunCalc = SCENE_CENTER_ANGLE;
   else if (theSwitch.compare("-zs") == 0) sunCalc = ZENITH_SUN;
   else {
      cout << "  ERROR:  Invalid switch!\n";
      cout << "          ...switch is either -sc (use scene center sun angle) or -zs (use a zenith sun)\n";
      return EXIT_FAILURE;
   }  

   // Grab parameters and set/calculate constants
   // -------------------------------------------
   readInfoFile(argv[2], &theInfo);
   cout << "Image file name:      " << theInfo.fileName << endl;
   cout << "   Band Number:       " << theInfo.bandNum << endl;
   cout << "   SatID:             " << theInfo.satID << endl;
   cout << "   Sensor:            " << theInfo.sensor << endl;
   cout << "   Aquisition Date:   " << theInfo.aqDate << endl;
   cout << "   Sun Elevation:     " << theInfo.sunElev << endl;
   cout << "   Gain:              " << theInfo.Gain << endl;
   cout << "   Bias:              " << theInfo.Bias << endl;
   cout << "   LMin:              " << theInfo.LMin << endl;
   cout << "   LMax:              " << theInfo.LMax << endl;
   cout << "   QCalMin:           " << theInfo.QCalMin << endl;
   cout << "   QCalMax:           " << theInfo.QCalMax << endl;

   // Get the scene date
   // ------------------
   int year = getYear(theInfo.aqDate);						
   int month = getMonth(theInfo.aqDate);
   int day = getDay(theInfo.aqDate);

   // Create output file names for each band
   // --------------------------------------
   string fNameOut;
   if (sunCalc == SCENE_CENTER_ANGLE) fNameOut = theInfo.fileName.substr(0, theInfo.fileName.find('.')) + "_TOAsc.TIF";
   if (sunCalc == ZENITH_SUN) fNameOut = theInfo.fileName.substr(0, theInfo.fileName.find('.')) + "_TOAzs.TIF";

   // Get the sun elevation, convert to solar zenith & take the cosine()
   // ------------------------------------------------------------------
   double cosSolarZ;
   if (sunCalc == SCENE_CENTER_ANGLE) cosSolarZ = cos((90.0 - theInfo.sunElev) * D2R);
   if (sunCalc == ZENITH_SUN) cosSolarZ = 1.0;

   // Calculate the Earth <--> Sun distance value
   // -------------------------------------------
    float dsun[366];

    //   sprintf(full_path,"%s/%s",path,"EarthSunDistance.txt");
    ifstream inFile;

    inFile.open("EarthSunDistance.txt");
    if (!inFile) {
      cout << "Unable to open file";
    }

    for (int i = 0; i < 366; i++)
    {
        inFile >> dsun[i]; 
    }
    inFile.close();
    //   double d = calc_d(doy(year, month, day));

    // Setup the output image properties (same as input, but 2-byte out)
    // -----------------------------------------------------------------
    imgProps outProps;
    outProps.nl = outProps.ns = outProps.nb = -1;      // Default to input size
    outProps.dtype = 2;                                // Signed short out
    outProps.processMode = IP_LINE_BY_LINE;

    // Reset bandnumber to 1 as we are doing 1 band per file
    //    theInfo.bandNum = 1

   // Process each band to TOA
   // ------------------------
   ImageTOA *theImage;
   cout << "Processing "<< theInfo.fileName << endl;
   theImage = new ImageTOA(theInfo.fileName, fNameOut, &outProps);
   theImage->setParms(theInfo.LMin, theInfo.LMax, theInfo.QCalMin, theInfo.QCalMax, dsun[doy(year, month, day)], 
	cosSolarZ, theInfo.bandNum, theInfo.satID);
   theImage->process();
   delete theImage;
}

