#include "readInfo.h"

using namespace std;

int readInfoFile(char *theInfoFile, bandInfo *theInfo) {
   FILE *fptr;
   int status;
   char c_key[128];
   char c_value[128];

   fptr = fopen(theInfoFile, "r");
   if (!fptr) { cout << "ERROR:  cannot open file " << theInfoFile << endl; return EXIT_FAILURE; }
   
   // Get the name of the band file
   // -----------------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"FileName:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->fileName = c_value;

   // Get the band number
   // -------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"Band:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->bandNum = atoi(c_value);

   // Get the SpaceCraft Name
   // -----------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"Spacecraft:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->satID = c_value;

   // Get the Sensor
   // --------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"Sensor:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->sensor = c_value;

   // Get the Aquisition Date
   // -----------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"Acquisition-Date:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->aqDate = c_value;

   // Get the Aquisition Time -- this might not be present!
   // -----------------------------------------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"Acquisition-Time:") != 0) { cout << "Error reading info file! (non-fatal)\n"; }
   theInfo->aqTime = c_value;

   // Get the Sun Elevation
   // ---------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"Sun-Elevation:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->sunElev = atof(c_value);

   // Get the Gain value
   // ------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"Gain:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->Gain = atof(c_value);

   // Get the Bias value
   // ------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"Bias:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->Bias = atof(c_value);

   // Get the LMin value
   // ------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"LMin:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->LMin = atof(c_value);

   // Get the LMax value
   // ------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"LMax:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->LMax = atof(c_value);

   // Get the QCalMin value
   // ---------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"QCalMin:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->QCalMin = atoi(c_value);

   // Get the QCalMax value
   // ---------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"QCalMax:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->QCalMax = atoi(c_value);

   // Get the UL & LR projection corner coordinates
   // ---------------------------------------------
   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"ULx:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->ULx = atof(c_value);

   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"ULy:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->ULy = atof(c_value);

   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"LRx:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->LRx = atof(c_value);

   status = fscanf( fptr, "%s %s", c_key, c_value);
   if (status != 2) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   if (strcmp(c_key,"LRy:") != 0) { cout << "Error reading info file!\n"; return EXIT_FAILURE; }
   theInfo->LRy = atof(c_value);

   return EXIT_SUCCESS;
}

