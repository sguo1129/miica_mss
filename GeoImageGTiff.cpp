#include "GeoImage.h"
#include <math.h>

// Constructor for reading entire input image (existing) into memory
// -----------------------------------------------------------------
GeoImage::GeoImage(std::string inFile) 
{
   ioType = INPUT_FULLIMAGE;
   GDALAllRegister();
   openInputFile(inFile);
   theImage->GetGeoTransform(projFrame);	// Grab Image Frame Info
   projInfo = theImage->GetProjectionRef();	// Grab projection info
   allocateDataBuffers();
   readEntireFile();
   GDALClose(theImage);
}

// Constructor for output file processing
// --------------------------------------
GeoImage::GeoImage(std::string outFile, imageProperties *outProps, double *frame, std::string proj) 
{
   ioType = OUTPUT_FULLIMAGE;
   GDALAllRegister();
   projInfo = proj;
   projFrame[0] = frame[0];
   projFrame[1] = frame[1];
   projFrame[2] = frame[2];
   projFrame[3] = frame[3];
   projFrame[4] = frame[4];
   projFrame[5] = frame[5];
   openOutputFile(outFile, outProps);
   allocateDataBuffers();
}

// Constructor for temporary file processing (in-memory only -- no file I/O here)
// ------------------------------------------------------------------------------
GeoImage::GeoImage(imageProperties *imageProperties)
{
   ioType = TEMP_FULLIMAGE;
   ns = imageProperties->ns;
   nl = imageProperties->nl;
   nb = imageProperties->nb;
   if (imageProperties->dtype == 1) gdalType = GDT_Byte;
   else if (imageProperties->dtype == 2) gdalType = GDT_Int16;
   else if (imageProperties->dtype == 3) gdalType = GDT_UInt16;
   else if (imageProperties->dtype == 4) gdalType = GDT_Int32;
   else if (imageProperties->dtype == 5) gdalType = GDT_UInt32;
   else gdalType = GDT_Float32;
   allocateDataBuffers();
}

// Destructor for the GeoImage Object
// ----------------------------------
GeoImage::~GeoImage()
{
   void *ptr = NULL;

   if (ioType == OUTPUT_FULLIMAGE) {
      writeEntireFile();
      GDALClose(theImage);
   }
   for (int i=0; i<theData.size(); ++i) {
      ptr = theData[i];
      //delete[] ptr;
      if (gdalType == GDT_Byte) delete[] (unsigned char *)ptr;
      else if (gdalType == GDT_Int16) delete[] (short *)ptr;
      else if (gdalType == GDT_UInt16) delete[] (unsigned short *)ptr;
      else if (gdalType == GDT_Int32) delete[] (int *)ptr;
      else if (gdalType == GDT_UInt32) delete[] (unsigned int *)ptr;
      else if (gdalType == GDT_Float32) delete[] (float *)ptr;
   }
}

// Open a single input image file
// ------------------------------
void GeoImage::openInputFile(std::string inFile)
{
   theImage = (GDALDataset *) GDALOpen(inFile.c_str(), GA_ReadOnly ); 
   if( theImage == NULL ) fatalError ("Error opening input file");
   nl = theImage->GetRasterYSize();
   ns = theImage->GetRasterXSize();
   nb = theImage->GetRasterCount();
   theBand = theImage->GetRasterBand(1);
   gdalType = theBand->GetRasterDataType();
}

imageProperties GeoImage::getImageProperties(void) 
{
   imageProperties temp;
   temp.nl = nl;
   temp.ns = ns;
   temp.nb = nb;
   return temp;
}

// Open a single output image file
// -------------------------------
void GeoImage::openOutputFile(std::string outFile, imageProperties *outProps)
{
   GDALDriver *poDriver;
   const char *pszFormat = "GTiff";
   //const char *pszFormat = "HFA";
   //const char *pszFormat = "ENVI";
   char **papszMetadata;

   ns = outProps->ns;
   nl = outProps->nl;
   nb = outProps->nb;
   if (outProps->dtype == 1) gdalType = GDT_Byte;
   else if (outProps->dtype == 2) gdalType = GDT_Int16;
   else if (outProps->dtype == 3) gdalType = GDT_UInt16;
   else if (outProps->dtype == 4) gdalType = GDT_Int32;
   else if (outProps->dtype == 5) gdalType = GDT_UInt32;
   else gdalType = GDT_Float32;

   // Setup the output file type driver
   // ---------------------------------
   poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
   if( poDriver == NULL ) fatalError("Output image format not supported!");

   papszMetadata = poDriver->GetMetadata();
   CSLFetchBoolean(papszMetadata, GDAL_DCAP_CREATE, FALSE);

   // Create the output image handles
   // -------------------------------
   char **papszOptions = NULL;
   theImage = poDriver->Create(outFile.c_str(), outProps->ns, outProps->nl, 
			outProps->nb, gdalType, papszOptions);  
   theImage->SetGeoTransform(projFrame);
   theImage->SetProjection(projInfo.c_str());
}

void GeoImage::readEntireFile()
{
   void *ImgPtr = NULL;

   for (int i=0; i<nb; ++i) {
      theBand = theImage->GetRasterBand(i+1);
      ImgPtr = theData[i];
      int errorNo = theBand->RasterIO(GF_Read, 0, 0, ns, nl, ImgPtr, ns, nl, gdalType, 0, 0);
      if (errorNo != 0) fatalError("Error reading file");
   }
}

void GeoImage::writeEntireFile()
{
   void *ImgPtr = NULL;

   for(int i=0; i<nb; ++i) {
      theBand = theImage->GetRasterBand(i+1);
      ImgPtr = theData[i];
      int errorNo = theBand->RasterIO(GF_Write, 0, 0, ns, nl, ImgPtr, ns, nl, gdalType, 0, 0);
      if (errorNo != 0) fatalError("Error writing file");
   }
}

void GeoImage::allocateDataBuffers()
{
   int size = nl * ns;
   void *ptr = NULL;

   for (int i=0; i<nb; ++i) {
      if (gdalType == GDT_Byte) ptr = new unsigned char[size];
      else if (gdalType == GDT_Int16) ptr = new short[size];
      else if (gdalType == GDT_UInt16) ptr = new unsigned short[size];
      else if (gdalType == GDT_Int32) ptr = new int[size];
      else if (gdalType == GDT_UInt32) ptr = new unsigned int[size];
      else if (gdalType == GDT_Float32) ptr = new float[size];
      else fatalError("Bad data type in image properties");
   if (!ptr) fatalError("Memory allocation for data buffers failed");

   theData.push_back(ptr);
   }
}

// Apply a linear filter to a mask...   
// ----------------------------------
void GeoImage::linearFilter(int theVal, int v1, int v2, int dim)
{
   unsigned char *imgPtr = (unsigned char *)theData[0];
   bool healRight = false;
   bool healLeft = false;
   int index;
   int numPixelsHealed = 0;

   for (int i=1; i<(nl-1); ++i) {	// Ignore the first & last lines
      for (int j=1; j<(ns-1); ++j) {	// Ignore the first & last samples
         index = i*ns+j;
         bool healRight = false;
         bool healLeft = false;
         if (imgPtr[index] == theVal) {	// Check for neighbors with v1 or v2 values.   This line only...
            
            // Check to the Right...
            // ---------------------
            for (int x=1; x<dim; ++x) {
               if ((j+x < ns) && (imgPtr[index+x] != theVal)) {
                  if ((imgPtr[index+x] == v1) || (imgPtr[index+x] == v2)) healRight = true;
                  break;
               }
            }

            // Check to the Left...
            // --------------------
            if (healRight) {    // If we're good on the right, look left
               for (int x=1; x<dim; ++x) {
                  if ((j-x > 1) && (imgPtr[index-x] != theVal)) {
                     if ((imgPtr[index-x] == v1) || (imgPtr[index-x] == v2)) healLeft = true; 
                     break;
                  }
               }
            }

            // If we're still good, fix the mask
            // ---------------------------------
            if (healRight && healLeft) {
               imgPtr[index] = v1;
               numPixelsHealed++;
            }
         }
      } 
   } 
   //std::cout << "Number of pixels healed:  " << numPixelsHealed << std::endl;
}

// Filter out lone pixels in a mask.   This is single band, byte-only for now...
// -----------------------------------------------------------------------------
void GeoImage::filter3x3(int filterVal, int numVal, int fillVal)
{
   unsigned char *imgPtr = (unsigned char *)theData[0];

   for (int i=1; i<(nl-1); ++i) {	// Ignore the first & last lines
      for (int j=1; j<(ns-1); ++j) {	// Ignore the first & last samples
         if (imgPtr[i*ns+j] == filterVal) {	// Check for neighbors
            if ((imgPtr[((i-1)*ns) + (j-1)] + imgPtr[((i-1)*ns) + j] + imgPtr[((i-1)*ns) + (j+1)] +		// Row above
                 imgPtr[(i*ns) + (j-1)] + imgPtr[(i*ns) + (j+1)] +						// Row 
                 imgPtr[((i+1)*ns) + (j-1)] + imgPtr[((i+1)*ns) + j] + imgPtr[((i+1)*ns) + (j+1)])		// Row below
                    <= numVal) imgPtr[i*ns+j] = fillVal;
         }
      }
   }
}

// Edge filter -- attempts to reduce the uneven edges caused by bumper mode.
// This works on the mask (single-band, byte) and modifies in-place like the 3x3 filters do...
// -------------------------------------------------------------------------------------------
void GeoImage::edgeFilter(int nibbleSize)
{
   int smNoFillLine_line = 99999;
   int smNoFillLine_samp = 0;
   int smNoFillSamp_line = 0;
   int smNoFillSamp_samp = 99999;
   int lrgNoFillLine_line = -1;
   int lrgNoFillLine_samp = 0;
   int lrgNoFillSamp_line = 0;
   int lrgNoFillSamp_samp = -1;

   unsigned char *imgPtr = (unsigned char *)theData[0];

   for (int i=0; i<nl; ++i) {
      for (int k=0; k<ns; ++k) {
         if (imgPtr[i*ns+k] != 0)
         {
            // Find the scene extents in line/sample, as we process
            // ----------------------------------------------------
            if (i < smNoFillLine_line) {  // Give me the line where the first data point occurs
            smNoFillLine_line = i;
            smNoFillLine_samp = k;
            }
            if (i >= lrgNoFillLine_line) {  // Give me the line where the last data point occurs
            lrgNoFillLine_line = i;
            lrgNoFillLine_samp = k;
            }
            if (k <= smNoFillSamp_samp) { // this one is >= so we can get the last line with max k...
            smNoFillSamp_samp = k;
            smNoFillSamp_line = i;
            }
            if (k > lrgNoFillSamp_samp) {  // Give me the first line w/max sample (k)
            lrgNoFillSamp_samp = k;
            lrgNoFillSamp_line = i;
            }
         }
      }
   }
            
   // Calc slope on left
   // ------------------
   float leftSlope;
   int leftNum = smNoFillSamp_line - smNoFillLine_line;
   int leftDenom = smNoFillSamp_samp - smNoFillLine_samp;
   if (leftNum == 0) leftSlope = 0.0;
   else if (leftDenom == 0) leftSlope = 1.0;
   else leftSlope = (float)leftNum/(float)leftDenom;
   //printf("\n   Slope on Left:  %f\n", leftSlope);

   // Fix (nibble) on the left
   // ------------------------
   int stopSamp;
   if ((leftSlope != 0) && (abs(leftSlope) != 1)) {
      for (int i=0; i<nl; ++i) {
         stopSamp = (i - smNoFillLine_line)/leftSlope + smNoFillLine_samp + nibbleSize;
         if (stopSamp > ns) stopSamp = ns;
         if (stopSamp < 0) stopSamp = 0;
         for (int k=0; k<stopSamp; ++k) imgPtr[i*ns+k] = 0;
      }
   }

   // Calc slope on right
   // ------------------
   float rightSlope;
   int rightNum = lrgNoFillLine_line - lrgNoFillSamp_line;
   int rightDenom = lrgNoFillLine_samp - lrgNoFillSamp_samp;
   if (rightNum == 0) rightSlope = 0.0;
   else if (rightDenom == 0) rightSlope = 1.0;
   else rightSlope = (float)rightNum/(float)rightDenom;
   //printf("\n   Slope on right:  %f\n", rightSlope);
   
   // Fix on the right
   // ----------------
   int startSamp;
   if ((rightSlope !=0) && (abs(rightSlope) != 1)) {
     for (int i=0; i<nl; ++i) {
        startSamp = (i - lrgNoFillSamp_line)/rightSlope + lrgNoFillSamp_samp - nibbleSize;
        if (startSamp > ns) startSamp = ns;
        if (startSamp < 0) startSamp = 0;
        for (int k=startSamp; k<ns; ++k) imgPtr[i*ns+k] = 0;
      }
   }
}

void GeoImage::calcStats(int maskVal)
{
   long size = nl*ns;
   long num = 0;
   double acc = 0.0;
   double tempVal;
   double mean;
   double sd;
   double minVal;
   double maxVal;

   // For unsigned char (byte) data buffers:
   // --------------------------------------
   if (gdalType == GDT_Byte) {
      for (int k=0; k<nb; ++k) {
         acc = 0.0;
         num = 0;
         minVal = 999999999.0;
         maxVal = -999999999.0;
         unsigned char *imgPtr = (unsigned char *)theData[k];
         for (long i=0; i<size; ++i) {
            if (imgPtr[i] != maskVal) {
               acc += (double)imgPtr[i];
               if ((double)imgPtr[i] > maxVal) maxVal = (double)imgPtr[i];
               if ((double)imgPtr[i] < minVal) minVal = (double)imgPtr[i];
               num++;
            }
         }
         mean = acc/(double)num;
         globalMean.push_back(mean);
         min.push_back(minVal);
         max.push_back(maxVal);

         acc = 0.0;
         num = 0;
         for (long i=0; i<size; ++i) {
            if (imgPtr[i] != maskVal) {
               tempVal = ((double)imgPtr[i] - mean);
               acc += (tempVal * tempVal);
               num++;
            }
         }
         sd = sqrt(acc/(double)num);
         stdDev.push_back(sd);
      }
   }
   
   // For signed 16-bit (short) data buffers:
   // ------------------------------------------
   else if (gdalType == GDT_Int16) {
      for (int k=0; k<nb; ++k) {
         acc = 0.0;
         num = 0;
         minVal = 999999999.0;
         maxVal = -999999999.0;
         short *imgPtr = (short *)theData[k];
         for (long i=0; i<size; ++i) {
            if (imgPtr[i] != maskVal) {
               acc += (double)imgPtr[i];
               if ((double)imgPtr[i] > maxVal) maxVal = (double)imgPtr[i];
               if ((double)imgPtr[i] < minVal) minVal = (double)imgPtr[i];
               num++;
            }
         }
         mean = acc/(double)num;
         globalMean.push_back(mean);
         min.push_back(minVal);
         max.push_back(maxVal);

         acc = 0.0;
         num = 0;
         for (long i=0; i<size; ++i) {
            if (imgPtr[i] != maskVal) {
               tempVal = ((double)imgPtr[i] - mean);
               acc += (tempVal * tempVal);
               num++;
            }
         }
         sd = sqrt(acc/(double)num);
         stdDev.push_back(sd);
      }
   }

   // For unsigned 16-bit (unsigned short) data buffers:
   // --------------------------------------------------
   else if (gdalType == GDT_UInt16) {
      for (int k=0; k<nb; ++k) {
         acc = 0.0;
         num = 0;
         minVal = 999999999.0;
         maxVal = -999999999.0;
         unsigned short *imgPtr = (unsigned short *)theData[k];
         for (long i=0; i<size; ++i) {
            if (imgPtr[i] != maskVal) {
               acc += (double)imgPtr[i];
               if ((double)imgPtr[i] > maxVal) maxVal = (double)imgPtr[i];
               if ((double)imgPtr[i] < minVal) minVal = (double)imgPtr[i];
               num++;
            }
         }
         mean = acc/(double)num;
         globalMean.push_back(mean);
         min.push_back(minVal);
         max.push_back(maxVal);

         acc = 0.0;
         num = 0;
         for (long i=0; i<size; ++i) {
            if (imgPtr[i] != maskVal) {
               tempVal = ((double)imgPtr[i] - mean);
               acc += (tempVal * tempVal);
               num++;
            }
         }
         sd = sqrt(acc/(double)num);
         stdDev.push_back(sd);
      }
   }

   else {
      fatalError("Image Statistics not implemented for image data type");
   }
}

void GeoImage::fatalError(std::string message) 
{
   std::cout << message << std::endl;
   exit(EXIT_FAILURE);
}

void GeoImage::debugMess(std::string message) 
{
   std::cout << message << std::endl;
}


