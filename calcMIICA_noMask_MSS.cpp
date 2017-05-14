#include <iostream>
#include <sstream>
#include <string>
#include "GeoImage.h"

#define MIICA_NO_DATA_VALUE	10
#define NDVI_NO_DATA_VALUE	255
#define NBR_NO_DATA_VALUE	255

unsigned char maxVal(unsigned char a, unsigned char b) { return ((a>b)?a:b); }

main(int argc, char *argv[]) {

   // Parse input parameters, open files, and allocate buffers
   // --------------------------------------------------------
   if (argc != 4) {
      cout << "calcMIICA v1.5, May 3, 2017 for MSS data\n";
      cout << "  Use:  calcMIICA date1Img date2Img outRoot\n";
      return EXIT_FAILURE;
   }
   string date1GTiffRoot = argv[1];
   string date2GTiffRoot = argv[2];
   string outImgRoot = argv[3];
   
   string ndvi1File = date1GTiffRoot + "_ndvi.img";
   string ndvi2File = date2GTiffRoot + "_ndvi.img";
   string dndviFile = outImgRoot + "_dndvi.img";
   string nbr1File = date1GTiffRoot + "_nbr.img";
   string nbr2File = date2GTiffRoot + "_nbr.img";
   string dnbrFile = outImgRoot + "_dnbr.img";
   string miicaFile = outImgRoot + "_miica.img";

   // Prepare Imagery for processing
   // ------------------------------
   stringstream ss;
   stringstream ss2;
   stringstream ss3;
   stringstream ss4;
   ss << 1;
   string str = ss.str();
   string fileName11 =date1GTiffRoot  + "_B" + str + "_TOAsc.TIF";
   string fileName21 =date2GTiffRoot  + "_B" + str + "_TOAsc.TIF";
   GeoImage *date11 = new GeoImage(fileName11);
   GeoImage *date21 = new GeoImage(fileName21);
   ss2 << 2;
   string str2 = ss2.str();
   string fileName12 =date1GTiffRoot  + "_B" + str2 + "_TOAsc.TIF";
   string fileName22 =date2GTiffRoot  + "_B" + str2 + "_TOAsc.TIF";
   GeoImage *date12 = new GeoImage(fileName12);
   GeoImage *date22 = new GeoImage(fileName22);
   ss3 << 3;
   string str3 = ss3.str();
   string fileName13 =date1GTiffRoot  + "_B" + str3 + "_TOAsc.TIF";
   string fileName23 =date2GTiffRoot  + "_B" + str3 + "_TOAsc.TIF";
   GeoImage *date13 = new GeoImage(fileName13);
   GeoImage *date23 = new GeoImage(fileName23);
   ss4 << 4;
   string str4 = ss4.str();
   string fileName14 =date1GTiffRoot  + "_B" + str4 + "_TOAsc.TIF";
   string fileName24 =date2GTiffRoot  + "_B" + str4 + "_TOAsc.TIF";
   GeoImage *date14 = new GeoImage(fileName14);
   GeoImage *date24 = new GeoImage(fileName24);

   imageProperties outImageProps = date11->getImageProperties();
   outImageProps.nb = 1;	// Single band output

   outImageProps.dtype = 2;	// write signed 16-bit output image
   GeoImage *dNBRImage = new GeoImage(&outImageProps);
   GeoImage *dNDVIImage = new GeoImage(&outImageProps);

   outImageProps.dtype = 3;	// Create unsigned 16-bit temporary image
   GeoImage *rcvmaxImage = new GeoImage(&outImageProps);
   outImageProps.dtype = 3;	// Create unsigned 16-bit temporary image
   GeoImage *cvImage = new GeoImage(&outImageProps);
   outImageProps.dtype = 1;	// write byte output image
   GeoImage *miicaImage = new GeoImage(miicaFile, &outImageProps, date11->projFrame, date11->projInfo);

   outImageProps.dtype = 1;     // write byte dNBR image
   GeoImage *dNBRImageByte = new GeoImage(dnbrFile, &outImageProps, date11->projFrame, date11->projInfo);
   GeoImage *dNDVIImageByte = new GeoImage(dndviFile, &outImageProps, date11->projFrame, date11->projInfo);
   GeoImage *ndvi1Image = new GeoImage(ndvi1File, &outImageProps, date11->projFrame, date11->projInfo);
   GeoImage *ndvi2Image = new GeoImage(ndvi2File, &outImageProps, date11->projFrame, date11->projInfo);
   GeoImage *nbr1Image = new GeoImage(nbr1File, &outImageProps, date11->projFrame, date11->projInfo);
   GeoImage *nbr2Image = new GeoImage(nbr2File, &outImageProps, date11->projFrame, date11->projInfo);

   // Process the image
   // -----------------
   short numer, dNom;
   float fnbr1, fnbr2;
   float fndvi1, fndvi2;
   bool failFlag = false;
   int imgSize = outImageProps.nl * outImageProps.ns;

   short *in1b1 = (short *)date11->theData[0];
   short *in1b2 = (short *)date12->theData[0];
   short *in1b3 = (short *)date13->theData[0];
   short *in1b4 = (short *)date14->theData[0];
   short *in2b1 = (short *)date21->theData[0];
   short *in2b2 = (short *)date22->theData[0];
   short *in2b3 = (short *)date23->theData[0];
   short *in2b4 = (short *)date24->theData[0];
   
   short *dNBR = (short *)dNBRImage->theData[0];			// Output is 16-bit signed
   short *dNDVI = (short *)dNDVIImage->theData[0];			// Output is 16-bit signed
   unsigned short *rcvmax = (unsigned short *)rcvmaxImage->theData[0];	// Output is 16-bit unsigned
   unsigned short *cv = (unsigned short *)cvImage->theData[0];		// Output is 16-bit unsigned
   unsigned char *miica = (unsigned char*)miicaImage->theData[0];	// Output is byte

   unsigned char *dNBRByte = (unsigned char *)dNBRImageByte->theData[0];     // Output is byte
   unsigned char *dNDVIByte = (unsigned char *)dNDVIImageByte->theData[0];   // Output is byte

   unsigned char *ndvi1 = (unsigned char *)ndvi1Image->theData[0];   // Output is byte
   unsigned char *ndvi2 = (unsigned char *)ndvi2Image->theData[0];   // Output is byte
   unsigned char *nbr1 = (unsigned char *)nbr1Image->theData[0];     // Output is byte
   unsigned char *nbr2 = (unsigned char *)nbr2Image->theData[0];     // Output is byte

   cout << "Calculating dNBR, dNDVI, max_cv, and cv" << endl;
   for (int i=0; i<imgSize; ++i) {

     //     cout<<"in1b1[i],in1b2[i],in1b3[i],in1b4[i]="<<in1b1[i]<<","<<in1b2[i]<<","<<in1b3[i]<<","<<in1b4[i]<<endl;
     //     cout<<"in2b1[i],in2b2[i],in2b3[i],in2b4[i]="<<in2b1[i]<<","<<in2b2[i]<<","<<in2b3[i]<<","<<in2b4[i]<<endl;

      // Apply the mask contained with each image 
      // ------------------------------------------------------------------------------
      if ((in1b1[i] <= 1) || (in1b2[i] <= 1) || (in1b3[i] <= 1) || (in1b4[i] <= 1) ||
          (in2b1[i] <= 1) || (in2b2[i] <= 1) || (in2b3[i] <= 1) || (in2b4[i] <= 1) ) {
      //if ((in1b1[i] == 0) || (in1b2[i] == 0) || (in1b3[i] == 0) || (in1b4[i] == 0) ||
          //(mask1[i] == 0) || (mask1[i] > 4) || (mask2[i] == 0) || (mask2[i] > 4)) {
          //(mask1[i] == 0) || (mask1[i] > 3) || (mask2[i] == 0) || (mask2[i] > 3)) {
         dNBR[i] = -999;
         dNDVI[i] = -999;
         rcvmax[i] = 0;
         cv[i] = 0;
         ndvi1[i] = NDVI_NO_DATA_VALUE;
         ndvi2[i] = NDVI_NO_DATA_VALUE;
         nbr1[i] = NBR_NO_DATA_VALUE;
         nbr2[i] = NBR_NO_DATA_VALUE;
      }
      else {

         cout<<"in1b1[i],in1b2[i],in1b3[i],in1b4[i]="<<in1b1[i]<<","<<in1b2[i]<<","<<in1b3[i]<<","<<in1b4[i]<<endl;
         cout<<"in2b1[i],in2b2[i],in2b3[i],in2b4[i]="<<in2b1[i]<<","<<in2b2[i]<<","<<in2b3[i]<<","<<in2b4[i]<<endl;
         // *********** dNBR **********************
         //
         // Calculate date2 NBR
         // -------------------
         failFlag = false;
         numer = in2b3[i] - in2b4[i];
         dNom = in2b3[i] + in2b4[i];
         if (dNom == 0) failFlag = true;
         else fnbr2 = (float)numer/(float)dNom;
         nbr2[i] = (failFlag) ? NBR_NO_DATA_VALUE : (unsigned char) ((fnbr2 * 100.0) + 100.0 + 0.5);

	 cout << "nbr2[i]=" << nbr2[i] << endl;
         // Calculate data1 NBR
         // -------------------
         numer = in1b3[i] - in1b4[i];
         dNom = in1b3[i] + in1b4[i];
         if (dNom == 0) failFlag = true;
         else fnbr1 = (float)numer/(float)dNom;
         nbr1[i] = (failFlag) ? NBR_NO_DATA_VALUE : (unsigned char) ((fnbr1 * 100.0) + 100.0 + 0.5);

         // Calculate dNBR
         // --------------
         if (failFlag) dNBR[i] = -999;
         else dNBR[i] = (short)((fnbr1 - fnbr2) * 100);

         // *********** dNDVI **********************
         //
         // Calculate date2 NDVI
         // --------------------
         failFlag = false;
         numer = in2b3[i] - in2b2[i];
         dNom = in2b3[i] + in2b2[i];
         if (dNom == 0) failFlag = true;
         else fndvi2 = (float)numer/(float)dNom;
         ndvi2[i] = (failFlag) ? NDVI_NO_DATA_VALUE : (unsigned char) ((fndvi2 * 100.0) + 100.0 + 0.5);

         // Calculate data1 NDVI
         // --------------------
         numer = in1b4[i] - in1b3[i];
         dNom = in1b4[i] + in1b3[i];
         if (dNom == 0) failFlag = true;
         else fndvi1 = (float)numer/(float)dNom;
         ndvi1[i] = (failFlag) ? NDVI_NO_DATA_VALUE : (unsigned char) ((fndvi1 * 100.0) + 100.0 + 0.5);

         // Calculate dNDVI
         // ---------------
         if (failFlag) dNDVI[i] = -999;
         else dNDVI[i] = (short)((fndvi1 - fndvi2) * 100);

     
         // ********** DiffImage Calculation  *************************************
         //
         // Calculate the "diffImg" image:: DATE2 - DATE1 w/masking (a check on bands 1 and 4 for zero conditions...
         // --------------------------------------------------------------------------------------------------------
         int diffImg[6];
         if ((in1b1[i] == 0) || (in2b1[i] == 0) || (in1b4[i] == 0) || (in2b4[i] == 0)) {
            diffImg[0] = 0;
            diffImg[1] = 0;
            diffImg[2] = 0;
            diffImg[3] = 0;
         }
         else {
            diffImg[0] = in2b1[i] - in1b1[i];
            diffImg[1] = in2b2[i] - in1b2[i];
            diffImg[2] = in2b3[i] - in1b3[i];
            diffImg[3] = in2b4[i] - in1b4[i];
         }

         // ************* RVC_MAX & CV ********************************
         //
         // Calculate RCV_MAX
         // -----------------
         long tempVar;
         float tempVal;
         float b4Norm;
         if ((in1b4[i] == 0) || (in2b4[i] == 0)) b4Norm = 0;
         else {
            tempVal = (float)diffImg[3] / (float)maxVal(in1b4[i], in2b4[i]) * 100.0;
            b4Norm = tempVal * tempVal;
         }
         float b3Norm;
         if ((in1b3[i] == 0) || (in2b3[i] == 0)) b3Norm = 0;
         else {
            tempVal = (float)diffImg[2] / (float)maxVal(in1b3[i], in2b3[i]) * 100.0;
            b3Norm = tempVal * tempVal;
         }
         float b2Norm;
         if ((in1b2[i] == 0) || (in2b2[i] == 0)) b2Norm = 0;
         else {
            tempVal = (float)diffImg[1] / (float)maxVal(in1b2[i], in2b2[i]) * 100.0;
            b2Norm = tempVal * tempVal;
         }
         float b1Norm;
         if ((in1b1[i] == 0) || (in2b1[i] == 0)) b1Norm = 0;  
         else {
            tempVal = (float)diffImg[0] / (float)maxVal(in1b1[i], in2b1[i]) * 100.0;
            b1Norm = tempVal * tempVal;
         }
         tempVar = (long)(b1Norm + b2Norm + b3Norm + b4Norm);
         if (tempVar > 65535) tempVar = 65535;
         if (tempVar < 0) tempVar = 0;
         rcvmax[i] = (unsigned short)tempVar;

         // Calculate CV 
         // ------------
         tempVar = (diffImg[0] * diffImg[0]) + (diffImg[1] * diffImg[1]) + (diffImg[2] * diffImg[2]) + (diffImg[3] * diffImg[3]);
         if (tempVar > 65535) tempVar = 65535;
         if (tempVar < 0) tempVar = 0;
         cv[i] = tempVar;
      }
   }

   // Calculate GlobalMean & StdDev values
   // ------------------------------------
   //cout << "Calculating image statistics" << endl;
   
   dNBRImage->calcStats(-999);    	// Calculate image stats with ignore value of -999
   dNDVIImage->calcStats(-999);    	// Calculate image stats with ignore value of -999
   rcvmaxImage->calcStats(0);    	// Calculate image stats with ignore value of 0
   cvImage->calcStats(0); 		// Calculate image stats with ignore value of 0

   //cout << "dNBR mean:    "  << dNBRImage->globalMean[0] << endl;
   //cout << "dNBR stdDev:  "  << dNBRImage->stdDev[0] << endl;
   //cout << "dNBR min:     "  << dNBRImage->min[0] << endl;
   //cout << "dNBR max:     "  << dNBRImage->max[0] << endl;

   // *************  MIICA  **************************
   //
   // Calculate MIICA
   // ---------------
   cout << "Calculating miica" << endl;
   int threshold = 100;

   for (int i=0; i<imgSize; ++i) {
      if ((cv[i] == 0 ) || (rcvmax[i] == 0) || (dNBR[i] == -999) || (dNDVI[i] == -999)) miica[i] = MIICA_NO_DATA_VALUE;
      else {
         if (((cv[i] >= cvImage->globalMean[0]) && (rcvmax[i] > (rcvmaxImage->globalMean[0] + 0.75 * rcvmaxImage->stdDev[0])) && 
             (dNDVI[i] <= (dNDVIImage->globalMean[0] - 0.5 * dNDVIImage->stdDev[0]))) ||
            ((cv[i] > threshold) && (dNBR[i] <= (dNBRImage->globalMean[0] - 1.0 * dNBRImage->stdDev[0])) && 
             (dNDVI[i] <= (dNDVIImage->globalMean[0] -1.0*dNDVIImage->stdDev[0]))) || 
            ((cv[i] >= cvImage->globalMean[0]) && (rcvmax[i] > (rcvmaxImage->globalMean[0] + 3.0 * rcvmaxImage->stdDev[0])) && 
             (dNDVI[i] < dNDVIImage->globalMean[0]))) miica[i] = 1;
         else if (((cv[i] >= cvImage->globalMean[0]) && (rcvmax[i] > (rcvmaxImage->globalMean[0] + 0.75 * rcvmaxImage->stdDev[0])) && 
             (dNDVI[i] >= (dNDVIImage->globalMean[0] + 0.5 * dNDVIImage->stdDev[0]))) || 
            ((cv[i] > threshold) && (dNBR[i] >= (dNBRImage->globalMean[0] + 1.5 * dNBRImage->stdDev[0])) && 
             (dNDVI[i] >= (dNDVIImage->globalMean[0] +1.5 * dNDVIImage->stdDev[0]))) ||
            ((cv[i] >= cvImage->globalMean[0]) && (rcvmax[i] > (rcvmaxImage->globalMean[0] + 3.0 * rcvmaxImage->stdDev[0])) && 
             (dNDVI[i] >= dNDVIImage->globalMean[0]))) miica[i] = 2;
         else miica[i] = 0;
      }
   }
  
   // Write Byte, positive-only versions of dNBR and dNDVI.
   // -----------------------------------------------------
   short theVal;
   for (int i=0; i<imgSize; ++i) {
      theVal = dNDVI[i];
      if (theVal == -999) theVal = 0;
      else if (theVal < 1) theVal = 1;
      else if (theVal > 255) theVal = 255;
      dNDVIByte[i] = theVal;

      theVal = dNBR[i];
      if (theVal == -999) theVal = 0;
      else if (theVal < 1) theVal = 0;
      else if (theVal > 255) theVal = 255;
      dNBRByte[i] = theVal;
   }

   // Filter for single-pixel change conditions
   // -----------------------------------------
   miicaImage->filter3x3(1,0,0);
   miicaImage->filter3x3(2,0,0);

   // Cleanup
   // -------
   delete date11;		// Cleans up buffers
   delete date21;		// Cleans up buffers
   delete date12;		// Cleans up buffers
   delete date22;		// Cleans up buffers
   delete date13;		// Cleans up buffers
   delete date23;		// Cleans up buffers
   delete date14;		// Cleans up buffers
   delete date24;		// Cleans up buffers
   delete dNBRImage;		// Writes the Imagery to disk, closes image, and cleans up buffers 
   delete dNBRImageByte;	// Writes the Imagery to disk, closes image, and cleans up buffers 
   delete dNDVIImage;		// Writes the Imagery to disk, closes image, and cleans up buffers 
   delete dNDVIImageByte;	// Writes the Imagery to disk, closes image, and cleans up buffers 
   delete rcvmaxImage;		// Cleans up buffers 
   delete cvImage;		// Cleans up buffers 
   delete miicaImage;		// Writes the Imagery to disk, closes image, and cleans up buffers 
   delete nbr1Image;		// Writes the Imagery to disk, closes image, and cleans up buffers 
   delete nbr2Image;		// Writes the Imagery to disk, closes image, and cleans up buffers 
   delete ndvi1Image;		// Writes the Imagery to disk, closes image, and cleans up buffers 
   delete ndvi2Image;		// Writes the Imagery to disk, closes image, and cleans up buffers 
}

