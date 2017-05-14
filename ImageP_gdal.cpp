


#include "ImageP_gdal.h"

// Constructor for IP_ONE_IN_ONE_OUT processing
// --------------------------------------------
ImageP::ImageP(string inFile, string outFile, imgProps *outProps) 
{
   processType = IP_ONE_IN_ONE_OUT;
   GDALAllRegister();
   openInputFile(inFile);
   if (outProps->nl == -1) outProps->nl = nl;
   if (outProps->ns == -1) outProps->ns = ns;
   if (outProps->nb == -1) outProps->nb = nb;
   openOutputFile(outFile, outProps);
}

// Constructor for IP_ONE_IN_ONE_OUT processing--no output size info spec'd
// ------------------------------------------------------------------------
ImageP::ImageP(string inFile, string outFile) 
{
   processType = IP_ONE_IN_ONE_OUT;
   GDALAllRegister();
   openInputFile(inFile);
   imgProps outProps;
   outProps.nl = nl;
   outProps.ns = ns;
   outProps.nb = nb;
   outProps.dtype = 1;
   outProps.processMode = IP_LINE_BY_LINE;
   openOutputFile(outFile, &outProps);
}

// Constructor for IP_MULTI_IN_ONE_OUT processing
// ----------------------------------------------
ImageP::ImageP(vector<string>inFiles, string outFile) 
{
   processType = IP_MULTI_IN_ONE_OUT;
   GDALAllRegister();
   openInputFiles(inFiles);
   imgProps outProps;
   outProps.nl = nl;
   outProps.ns = ns;
   outProps.nb = nb;
   outProps.dtype = 1;
   outProps.processMode = IP_LINE_BY_LINE;
   openOutputFile(outFile, &outProps);
}

// Constructor for IP_MULTI_IN_ONE_OUT processing
// ----------------------------------------------
ImageP::ImageP(vector<string>inFiles, string outFile, imgProps *outProps) 
{
   processType = IP_MULTI_IN_ONE_OUT;
   GDALAllRegister();
   openInputFiles(inFiles);
   if (outProps->nl == -1) outProps->nl = nl;
   if (outProps->ns == -1) outProps->ns = ns;
   if (outProps->nb == -1) outProps->nb = nb;
   openOutputFile(outFile, outProps);
}

// Constructor for IP_ZERO_IN_ONE_OUT processing
// ---------------------------------------------
ImageP::ImageP(string outFile, imgProps *outProps)
{
   processType = IP_ZERO_IN_ONE_OUT;
   GDALAllRegister();
   openOutputFile(outFile, outProps);
}

// Constructor for IP_ONE_IN_ZERO_OUT processing
// ---------------------------------------------
ImageP::ImageP(string inFile)
{
   processType = IP_ONE_IN_ZERO_OUT;
   GDALAllRegister();
   openInputFile(inFile);
}

// Constructor for IP_MULTI_IN_ZERO_OUT processing
// -----------------------------------------------
ImageP::ImageP(vector<string>inFiles)
{
   processType = IP_MULTI_IN_ZERO_OUT;
   GDALAllRegister();
   openInputFiles(inFiles);
}

// Open the Input image files and verify that the imagery is the same size
// -----------------------------------------------------------------------
void ImageP::openInputFiles(vector<string>inFiles) 
{
   nb = 0;
   for (int i=0; i<inFiles.size(); ++i) {
      imagery thisImage;
      thisImage.imgPtr = (GDALDataset *) GDALOpen(inFiles[i].c_str(), GA_ReadOnly );		// Open the input file
      if( thisImage.imgPtr == NULL ) {
         cout << "Error opening input file:  " << inFiles[i].c_str() << endl;
         //return EXIT_FAILURE;
      }
      thisImage.nl = thisImage.imgPtr->GetRasterYSize();
      thisImage.ns = thisImage.imgPtr->GetRasterXSize(); 
      thisImage.nb = thisImage.imgPtr->GetRasterCount();
      if (i == 0) {
         nl = thisImage.nl;
         ns = thisImage.ns;
         inputImage = thisImage.imgPtr;  // <-- save for future header reads...
      }
      else {
         if ((nl != thisImage.nl) || (ns != thisImage.ns)) {
            cout << "ERROR:  Input image dimensions (lines & samples) must match!\n";
            //return EXIT_FAILURE;
         }
      }
      nb += thisImage.nb;
      thisImage.data = new float[thisImage.ns];
      if (!thisImage.data) {
         cout << "Error allocating input data buffer!\n";
         //return EXIT_FAILURE;
      }
      double adfGeoTransform[6];
      thisImage.imgPtr->GetGeoTransform(adfGeoTransform);
      ULx = adfGeoTransform[0]; 
      ULy = adfGeoTransform[3];
      PxSize = adfGeoTransform[1];

      inputImagery.push_back(thisImage);
   }
}

// Open a single input image file
// ------------------------------
void ImageP::openInputFile(string inFile)
{
   inputImage = (GDALDataset *) GDALOpen(inFile.c_str(), GA_ReadOnly ); 
   if( inputImage == NULL ) {
      cout << "Error opening input file:  " << inFile.c_str() << endl;
      //return EXIT_FAILURE;
   }
   nl = inputImage->GetRasterYSize();
   ns = inputImage->GetRasterXSize();
   nb = inputImage->GetRasterCount();

   double adfGeoTransform[6];
   inputImage->GetGeoTransform(adfGeoTransform);
   ULx = adfGeoTransform[0]; 
   ULy = adfGeoTransform[3];
   PxSize = adfGeoTransform[1];

   // Allocate memory for the data buffer
   // -----------------------------------
   theData = new float[ns];
   if (!theData) {
      cout << "Error allocating data buffers!\n";
      //return EXIT_FAILURE;
   }
}

imgProps ImageP::getInProps(void) 
{
   imgProps temp;
   temp.nl = nl;
   temp.ns = ns;
   temp.nb = nb;
   return temp;
}

//int ImageP::getCurrentBand(void) { return currentBand; }
  
// Open a single output image file
// -------------------------------
void ImageP::openOutputFile(string outFile, imgProps *outProps)
{
   GDALDriver *poDriver;
   const char *pszFormat = "GTiff";
   //   const char *pszFormat = "HFA";
   char **papszMetadata;
   double adfGeoTransform[6];    // GDAL geoTransform (framing) info

   //   cout << "GDT_Int16=" << GDT_Int16 << endl;

   out_ns = outProps->ns;
   out_nl = outProps->nl;
   out_nb = outProps->nb;
   if (outProps->dtype == 1) outType = GDT_Byte;
   else outType = GDT_Int16;

   // Allocate memory for the output data buffer
   // ------------------------------------------
   if (outType == GDT_Byte) {
      theDataOut = new unsigned char[outProps->ns];
      if (!theDataOut) {
         cout << "Error allocating output data buffer!\n";
         //return EXIT_FAILURE;
      }
   }
   else {
      theDataOutI2 = new short[outProps->ns];
      if (!theDataOutI2) {
         cout << "Error allocating output data buffer!\n";
         //return EXIT_FAILURE;
      }
   }
 
   // Setup the output file type driver
   // ---------------------------------
   poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
   //if( poDriver == NULL ) return EXIT_FAILURE;

   papszMetadata = poDriver->GetMetadata();
   CSLFetchBoolean(papszMetadata, GDAL_DCAP_CREATE, FALSE);

   // Grab the Projection and GeoTransformation info from the first input file
   // ------------------------------------------------------------------------
   inputImage->GetGeoTransform(adfGeoTransform);

   // *********************************************************************************************
   // This section was added to adjust the pixel-reference to the upper left.   LPGS processes to
   // a center-pixel referenced and labels the GeoTiff file as "Point" referenced.   There is
   // argument about the intention of "Point" vs "Area" in the GeoTiff standard.  GDAL ignores the
   // Point vs Area and assumes everything is Area -- that is, it does not adjust the coordinates
   // to the upper left corner -- so that is done here...    This may (or may not) change in the
   // future releases of GDAL or LPGS.   Tune into this, and make sure it is correct thru time!
   // It doesn't matter who's right (GDAL or LPGS), just that it is tracked correctly!
   // *********************************************************************************************
   papszMetadata = inputImage->GetMetadata();
   /*
   if (CSLCount(papszMetadata) > 0) {
      string nameVal = papszMetadata[0];
      //cout << "   " << nameVal << endl;
      if (nameVal.compare("AREA_OR_POINT=Point") == 0)  {
         cout.setf(ios_base::fixed, ios_base::floatfield);
         cout << "   Adjusting center-pixel located projection coordinates to upper-left:\n";
         cout << "     X: " << adfGeoTransform[0] << " (center-pixel) to ";
         //adfGeoTransform[0] -= 15.0;
         adfGeoTransform[0] -= (adfGeoTransform[1]/2.0);
         cout << adfGeoTransform[0] << " (upper-left pixel)\n";
         cout << "     Y: " << adfGeoTransform[3] << " (center-pixel) to ";
         //adfGeoTransform[3] += 15.0;
         adfGeoTransform[3] -= (adfGeoTransform[5]/2.0);
         cout << adfGeoTransform[3] << " (upper-left pixel)\n";
         cout.setf(ios_base::fmtflags(0), ios_base::floatfield);
      }
      else if (nameVal.compare("AREA_OR_POINT=Area") == 0)  {
         //cout << "   Pixels are upper-left referenced...\n";
      }
   } */

   //   cout << "outType = " << outType << endl;

   // Create the output image handles
   // -------------------------------
   char **papszOptions = NULL;
   outputImage = poDriver->Create(outFile.c_str(), outProps->ns, outProps->nl, 
			outProps->nb, outType, papszOptions);  
   outputImage->SetGeoTransform(adfGeoTransform);
   outputImage->SetProjection(inputImage->GetProjectionRef());
}

ImageP::~ImageP()
{
   if (processType == IP_ONE_IN_ONE_OUT) {
      delete inputImage;
      delete outputImage;
      delete[] theData;
      if (outType == GDT_Byte) delete[] theDataOut;
      else delete[] theDataOutI2;
   }
   else if (processType == IP_MULTI_IN_ONE_OUT) {
      delete outputImage;
      if (outType == GDT_Byte) delete[] theDataOut;
      else delete[] theDataOutI2;
   }
}

void ImageP::xy2ls(double x, double y, double *sample, double *line)
{
   *sample = (x - ULx) / PxSize;	// Zero-relative image coordinates as in GDAL data model
   *line =  (ULy - y) / PxSize;		// Zero-relative image coordinates as in GDAL data model
}

void ImageP::processLine(int line) 
{
   unsigned char dummy;
   short dummyI2;

   if (processType == IP_ONE_IN_ZERO_OUT)
   {
      // Process each band...
      // --------------------
      short dummy;

      for (int i=0; i<nb; ++i) {
         currentBand = i+1;
         inputBand = inputImage->GetRasterBand(i+1);

         // Process each line...
         // --------------------
         currentLine = line;
         int errorNo = inputBand->RasterIO(GF_Read, 0, line, ns, 1, theData, ns, 1, GDT_Float32, 0, 0);
         if (errorNo != 0) {
            cout << "Return Status from RasterIO for image1:  " << errorNo
                        << "; currLine = " << line << endl;
         }

         // Map each pixel
         // ---------------------------------
         for (int k=0; k<ns; ++k) {
            if (outType == GDT_Byte) dummy = eval(k);
            else dummyI2 = evalI2(k);
         }
      }
   }
   else {
      cout << "*** Internal Error:  Process-type not support in the processLine() method; use process()\n";
   }
}
void ImageP::process(void) 
{
   if (processType == IP_ONE_IN_ONE_OUT) 
   {
      // Process each band...
      // --------------------
      for (int i=0; i<nb; ++i) {
	 //cout << "Processing band " << i+1 << endl;
         currentBand = i+1;
         outputBand = outputImage->GetRasterBand(i+1);
         inputBand = inputImage->GetRasterBand(i+1);

         // Process each line...
         // --------------------
         for (int j=0; j< nl; ++j) {
            currentLine = j;
            int errorNo = inputBand->RasterIO
			(GF_Read, 0, j, ns, 1, theData, ns, 1, GDT_Float32, 0, 0);
            if (errorNo != 0) {
               cout << "Return Status from RasterIO for image1:  " << errorNo 
			<< "; currLine = " << j << endl;
               //return EXIT_FAILURE;
            }
    
            // Map each pixel 
            // ---------------------------------
            for (int k=0; k<ns; ++k) {
               if (outType == GDT_Byte) theDataOut[k] = eval(k);
               else theDataOutI2[k] = evalI2(k);
            }
            if (outType == GDT_Byte) outputBand->RasterIO
				       (GF_Write, 0, j, ns, 1, theDataOut, ns, 1, outType, 0, 0);
            else outputBand->RasterIO
		   (GF_Write, 0, j, ns, 1, theDataOutI2, ns, 1, outType, 0, 0);
         }
      } 
   }
   /*else if (processType == IP_ONE_IN_FLATTEN)
   {
      // Process each band...
      // --------------------
      for (int i=0; i<nb; ++i) {
         //cout << "Processing band " << i+1 << endl;
         currentBand = i+1;
         outputBand = outputImage->GetRasterBand(i+1);
         inputBand = inputImage->GetRasterBand(i+1);

         // Process each line...
         // --------------------
         for (int j=0; j< nl; ++j) {
            currentLine = j;
            int errorNo = inputBand->RasterIO(GF_Read, 0, j, ns, 1, theData, ns, 1, GDT_Float32, 0, 0);
            if (errorNo != 0) {
               cout << "Return Status from RasterIO for image1:  " << errorNo << "; currLine = " << j << endl;
               //return EXIT_FAILURE;
            }

            // Map each pixel
            // ---------------------------------
            for (int k=0; k<ns; ++k) {
               if (outType == GDT_Byte) theDataOut[k] = eval(k);
               else theDataOutI2[k] = evalI2(k);
            }
            if (outType == GDT_Byte) outputBand->RasterIO
                        (GF_Write, 0, j, ns, 1, theDataOut, ns, 1, outType, 0, 0);
            else outputBand->RasterIO
                        (GF_Write, 0, j, ns, 1, theDataOutI2, ns, 1, outType, 0, 0);
         }
      }
   } */
   else if (processType == IP_ONE_IN_ZERO_OUT)
   {
      // Process each band...
      // --------------------
      short dummy;

      for (int i=0; i<nb; ++i) {
         //cout << "Processing band " << i+1 << endl;
         currentBand = i+1;
         inputBand = inputImage->GetRasterBand(i+1);

         // Process each line...
         // --------------------
         for (int j=0; j< nl; ++j) {
            currentLine = j;
            int errorNo = inputBand->RasterIO
                        (GF_Read, 0, j, ns, 1, theData, ns, 1, GDT_Float32, 0, 0);
            if (errorNo != 0) {
               cout << "Return Status from RasterIO for image1:  " << errorNo
                        << "; currLine = " << j << endl;
               //return EXIT_FAILURE;
            }

            // Process each pixel...
            // ---------------------
            for (int k=0; k<ns; ++k) {
               dummy = eval(k);
            }
         }
      }
   }
   else if (processType == IP_MULTI_IN_ONE_OUT) 
   {
      int bandCnt = 0;

      // Process each file...
      // --------------------
      for (int fiNum=0; fiNum<inputImagery.size(); ++fiNum, ++bandCnt) 
      {
         // Process each band...
         // --------------------
         imagery thisImage = inputImagery[fiNum];
         for (int i=0; i<thisImage.nb; ++i) {
            theData = thisImage.data;
            //cout << "Processing band " << i+1 << endl;
            outputBand = outputImage->GetRasterBand(bandCnt+1);
            inputBand = thisImage.imgPtr->GetRasterBand(i+1);

            // Process each line...
            // --------------------
            for (int j=0; j< nl; ++j) {
               currentLine = j;
               int errorNo = inputBand->RasterIO
			(GF_Read, 0, j, ns, 1, theData, ns, 1, GDT_Float32, 0, 0);
               if (errorNo != 0) {
                  cout << "Return Status from RasterIO for image1:  " << errorNo 
			<< "; currLine = " << j << endl;
                  //return EXIT_FAILURE;
               }

               // Map each pixel
               // ---------------------------------
               for (int k=0; k<ns; ++k) {
                  if (outType == GDT_Byte) theDataOut[k] = eval(k);
                  else theDataOutI2[k] = evalI2(k);
               }
               if (outType == GDT_Byte) outputBand->RasterIO
					  (GF_Write, 0, j, ns, 1, theDataOut, ns, 1, outType, 0, 0);
               else outputBand->RasterIO
		      (GF_Write, 0, j, ns, 1, theDataOutI2, ns, 1, outType, 0, 0);
            }
         }
      }
   }
}

