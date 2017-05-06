#include "GeoImage.h"
#include <math.h>

// Constructor for reading entire input image (existing) into memory
// -----------------------------------------------------------------
GeoImage::GeoImage(string inFile) 
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
GeoImage::GeoImage(string outFile, imageProperties *outProps, double *frame, string proj) 
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
   }
}

// Open a single input image file
// ------------------------------
void GeoImage::openInputFile(string inFile)
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
void GeoImage::openOutputFile(string outFile, imageProperties *outProps)
{
   GDALDriver *poDriver;
   //const char *pszFormat = "GTiff";
   const char *pszFormat = "HFA";
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
   cout << "Number of pixels healed:  " << numPixelsHealed << endl;
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

void GeoImage::fatalError(string message) 
{
   cout << message << endl;
   exit(EXIT_FAILURE);
}

void GeoImage::debugMess(string message) 
{
   cout << message << endl;
}


