#include <iostream>
#include <string>
#include <vector>
#include "gdal_priv.h"
#include "ogr_spatialref.h"

// Processing modes
#define  INPUT_FULLIMAGE	1101	// Keep entire input image in memory
#define  OUTPUT_FULLIMAGE	1102	// Keep entire output image in memory
#define  TEMP_FULLIMAGE		1103	// Keep temp image in memory 

using namespace std;

struct imageProps
{
   int nl;		// Number of lines this image
   int ns;		// Number of samples this image
   int nb;		// Number of bands this image
   int dtype;		// Data type:  1 = byte; 2 = 16-bit; 3 = unsigned 16-bit; 
			//             4 = 32-bit; 5 = unsigned 32-bit; 6 = float
};
typedef imageProps imageProperties;

class GeoImage
{
   public:
      GeoImage(string inFile);
      GeoImage(string outFile, imageProperties *outProps, double *frame, string proj);
      GeoImage(imageProperties *outProps);
      ~GeoImage();
      imageProperties getImageProperties(void);
      void calcStats(int maskVal);
      void filter3x3(int filterVal, int numVal, int fillVal);
      void linearFilter(int theVal, int v1, int v2, int dim);
      void edgeFilter(int nibbleSize);
      
      vector<double> globalMean;
      vector<double> stdDev;
      vector<double> min;
      vector<double> max;

      vector <void *> theData; 

      double projFrame[6];
      string projInfo;

   protected:
      GDALDataset *theImage;
      GDALRasterBand *theBand;

      int nl;
      int ns;
      int nb;
      int ioType;

      GDALDataType gdalType;
      GDALDataset *imgPtr;	// Ptr to the GDAL data set 

   private:
      void openInputFile(string inFile);
      void openOutputFile(string outFile, imageProperties *outProps);
      void readEntireFile();
      void writeEntireFile();
      void allocateDataBuffers();
      void fatalError(string message);
      void debugMess(string message);
};
