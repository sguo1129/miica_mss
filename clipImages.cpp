//  clipImages
// 
//  Clip each band of two images into exact corner coordinates
//
//  Song Guo, USGS/EROS, May 28, 2017, modified the code to work for Landsat MSS data
//
//  Note: This code is only working for Landsat Archive Pre-Collection Level-1 L1-5
//        MSS data, with some modification of filename and metadata, it can work
//        for Landsat Legacy L1-5 MSS data 
// ----------------------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "readInfo.h"

double maxVal(double a, double b) { return (int) ((a>b)?a:b); }
double minVal(double a, double b) { return (int) ((a<b)?a:b); }

main(int argc, char *argv[]) {

   string sceneName1 = argv[1];
   string sceneName2 = argv[2];
   string sceneName11 = argv[1];
   string sceneName22 = argv[2];
   bandInfo theInfo;
   bandInfo theInfo2;

   int minX;
   int maxX;
   int minY;
   int maxY;

   char infoFile1[200];
   char infoFile2[200];
   char command[200];
   string str;
   string str2;
   string str3;
   string str4;
   string fileName11;
   string fileName12;
   string fileName13;
   string fileName14;
   string fileName21;
   string fileName22;
   string fileName23;
   string fileName24;
   string fileNameOut11;
   string fileNameOut12;
   string fileNameOut13;
   string fileNameOut14;
   string fileNameOut21;
   string fileNameOut22;
   string fileNameOut23;
   string fileNameOut24;
   stringstream ss11;
   stringstream ss12;
   stringstream ss13;
   stringstream ss14;
   stringstream ss21;
   stringstream ss22;
   stringstream ss23;
   stringstream ss24;

   const size_t last_slash_idx = sceneName1.find_last_of("\\/");
   if (string::npos != last_slash_idx)
   {
       sceneName1.erase(0, last_slash_idx + 1);
   }
   const size_t last_slash_idx2 = sceneName2.find_last_of("\\/");
   if (string::npos != last_slash_idx2)
   {
       sceneName2.erase(0, last_slash_idx2 + 1);
   }

   if (sceneName1.at(2) == '1' || sceneName1.at(2) == '2' || sceneName1.at(2) == '3')
   {
       sprintf(infoFile1, "%s_B4_info.txt", argv[1]);
       sprintf(infoFile2, "%s_B4_info.txt", argv[2]);

   }
   else if (sceneName1.at(2) == '4' || sceneName1.at(2) == '5')
   {
       sprintf(infoFile1, "%s_B1_info.txt", argv[1]);
       sprintf(infoFile2, "%s_B1_info.txt", argv[2]);
   }
   else 
   {
      cout << "Unsupport sensor number for MSS data" << endl;
      return EXIT_FAILURE;
   }

   cout << "infoFile1 = " << infoFile1 << endl;
   cout << "infoFile2 = " << infoFile2 << endl;

   readInfoFile(infoFile1, &theInfo);
   readInfoFile(infoFile2, &theInfo2);

   cout << "Scene1: ULx: " << theInfo.ULx << endl;
   cout << "Scene1: ULy: " << theInfo.ULy << endl;
   cout << "Scene1: LRx: " << theInfo.LRx << endl;
   cout << "Scene1: LRy: " << theInfo.LRy << endl;
   cout << "Scene2: ULx: " << theInfo2.ULx << endl;
   cout << "Scene2: ULy: " << theInfo2.ULy << endl;
   cout << "Scene2: LRx: " << theInfo2.LRx << endl;
   cout << "Scene2: LRy: " << theInfo2.LRy << endl;

   minX = maxVal(theInfo.ULx, theInfo2.ULx);   
   maxY = minVal(theInfo.ULy, theInfo2.ULy);
   maxX = minVal(theInfo.LRx, theInfo2.LRx);
   minY = maxVal(theInfo.LRy, theInfo2.LRy);

   cout << "minX = " << minX << endl;
   cout << "maxY = " << maxY << endl;
   cout << "maxX = " << maxX << endl;
   cout << "minY = " << minY << endl;

   if (sceneName1.at(2) == '1' || sceneName1.at(2) == '2' || sceneName1.at(2) == '3')
   {
       ss11 << 4;
       str = ss11.str();
       fileName11 = sceneName11 + "_B" + str + "_TOAsc.TIF";
       fileNameOut11 = sceneName11 + "_B" + str + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName11.c_str(), fileNameOut11.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss12 << 5;
       str2 = ss12.str();
       fileName12 = sceneName11 + "_B" + str2 + "_TOAsc.TIF";
       fileNameOut12 = sceneName11 + "_B" + str2 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName12.c_str(), fileNameOut12.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss13 << 6;
       str3 = ss13.str();
       fileName13 = sceneName11 + "_B" + str3 + "_TOAsc.TIF";
       fileNameOut13 = sceneName11 + "_B" + str3 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName13.c_str(), fileNameOut13.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss14 << 7;
       str4 = ss14.str();
       fileName14 = sceneName11 + "_B" + str4 + "_TOAsc.TIF";
       fileNameOut14 = sceneName11 + "_B" + str4 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName14.c_str(), fileNameOut14.c_str());
       cout <<" command = "<< command << endl;
       system(command);
   }
   else if (sceneName1.at(2) == '4' || sceneName1.at(2) == '5')
   {
       ss11 << 1;
       str = ss11.str();
       fileName11 = sceneName11 + "_B" + str + "_TOAsc.TIF";
       fileNameOut11 = sceneName11 + "_B" + str + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName11.c_str(), fileNameOut11.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss12 << 2;
       str2 = ss12.str();
       fileName12 = sceneName11 + "_B" + str2 + "_TOAsc.TIF";
       fileNameOut12 = sceneName11 + "_B" + str2 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName12.c_str(), fileNameOut12.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss13 << 3;
       str3 = ss13.str();
       fileName13 = sceneName11 + "_B" + str3 + "_TOAsc.TIF";
       fileNameOut13 = sceneName11 + "_B" + str3 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName13.c_str(), fileNameOut13.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss14 << 4;
       str4 = ss14.str();
       fileName14 = sceneName11 + "_B" + str4 + "_TOAsc.TIF";
       fileNameOut14 = sceneName11 + "_B" + str4 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName14.c_str(), fileNameOut14.c_str());
       cout <<" command = "<< command << endl;
       system(command);
   }
   else 
   {
      cout << "Unsupport sensor number for MSS data" << endl;
      return EXIT_FAILURE;
   }

   if (sceneName2.at(2) == '1' || sceneName2.at(2) == '2' || sceneName2.at(2) == '3')
   {
       ss21 << 4;
       str = ss21.str();
       fileName21 = sceneName22 + "_B" + str + "_TOAsc.TIF";
       fileNameOut21 = sceneName22 + "_B" + str + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName21.c_str(), fileNameOut21.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss22 << 5;
       str2 = ss22.str();
       fileName22 = sceneName22 + "_B" + str2 + "_TOAsc.TIF";
       fileNameOut22 = sceneName22 + "_B" + str2 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName22.c_str(), fileNameOut22.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss23 << 6;
       str3 = ss23.str();
       fileName23 = sceneName22 + "_B" + str3 + "_TOAsc.TIF";
       fileNameOut23 = sceneName22 + "_B" + str3 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName23.c_str(), fileNameOut23.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss24 << 7;
       str4 = ss24.str();
       fileName24 = sceneName22 + "_B" + str4 + "_TOAsc.TIF";
       fileNameOut24 = sceneName22 + "_B" + str4 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName24.c_str(), fileNameOut24.c_str());
       cout <<" command = "<< command << endl;
       system(command);
   }
   else if (sceneName2.at(2) == '4' || sceneName2.at(2) == '5')
   {
       ss21 << 1;
       str = ss21.str();
       fileName21 = sceneName22 + "_B" + str + "_TOAsc.TIF";
       fileNameOut21 = sceneName22 + "_B" + str + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName21.c_str(), fileNameOut21.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss22 << 2;
       str2 = ss22.str();
       fileName22 = sceneName22 + "_B" + str2 + "_TOAsc.TIF";
       fileNameOut22 = sceneName22 + "_B" + str2 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName22.c_str(), fileNameOut22.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss23 << 3;
       str3 = ss23.str();
       fileName23 = sceneName22 + "_B" + str3 + "_TOAsc.TIF";
       fileNameOut23 = sceneName22 + "_B" + str3 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName23.c_str(), fileNameOut23.c_str());
       cout <<" command = "<< command << endl;
       system(command);

       ss24 << 4;
       str4 = ss24.str();
       fileName24 = sceneName22 + "_B" + str4 + "_TOAsc.TIF";
       fileNameOut24 = sceneName22 + "_B" + str4 + "_TOAsc_clip.TIF";
       sprintf(command, "gdal_translate -projwin %d %d %d %d %s %s", minX, maxY, maxX, minY, 
                        fileName24.c_str(), fileNameOut24.c_str());
       cout <<" command = "<< command << endl;
       system(command);
   }
   else 
   {
      cout << "Unsupport sensor number for MSS data" << endl;
      return EXIT_FAILURE;
   }
}
