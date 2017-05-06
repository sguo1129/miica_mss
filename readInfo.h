#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

struct LSinfo
   {
   int bandNum;
   int QCalMin;
   int QCalMax;
   double LMin;
   double LMax;
   double Gain;
   double Bias;
   double sunElev;
   double ULx, ULy, LRx, LRy;
   string fileName;   
   string satID;
   string sensor;
   string aqDate;
   string aqTime;
   };
typedef LSinfo bandInfo;

int readInfoFile(char *theInfoFile, bandInfo *theInfo);



















