#include <iostream>
#include <string>
#include <string>
#include "gdal_priv.h"
#include "ogr_spatialref.h"

using namespace std;

#define  IP_ONE_IN_ONE_OUT	1001	// One image in, one image out
#define  IP_MULTI_IN_ONE_OUT	1002	// Multiple images in, one image out
#define  IP_ONE_IN_ZERO_OUT	1003	// One image in, no image output
#define  IP_MULTI_IN_ZERO_OUT	1004	// One image in, no image output
#define  IP_ZERO_IN_ONE_OUT	1005	// No image input, one image out

// Processing modes
#define  IP_INPUT_INCORE	1101	// Keep the entire input image in core
#define  IP_OUTPUT_INCORE	1102	// Keep the entire output image in core
#define  IP_INCORE		1103	// Keep input & output imagery in core
#define  IP_LINE_BY_LINE	1104	// Process each line; eval() sees current band
#define  IP_LINE_BY_LINE_MB	1105	// eval() sees all bands for current pixel 

   struct IPimagery
   {
      GDALDataset *imgPtr;	// Ptr to the GDAL data set 
      int nl;			// Number of lines this image
      int ns;			// Number of samples this image
      int nb;			// Number of bands this image
      float *data;		// The data buffer for this image
   };
   typedef IPimagery imagery;

   struct imgProp
   {
      int nl;		// Number of lines this image
      int ns;		// Number of samples this image
      int nb;		// Number of bands this image
      int dtype;	// Data type:  1 = byte; 2 = 16-bit; 3 = unsigned 16-bit; 
			//      4 = 32-bit; 5 = unsigned 32-bit; 6 = float
      int processMode;  // Processing mode, from defined types above
   };
   typedef imgProp imgProps;

   class ImageP
   {
   public:
      ImageP(string inFile, string outFile);	
      ImageP(string inFile, string outFile, imgProps *outProps);	
      ImageP(vector<string>inFiles, string outFile);
      ImageP(vector<string>inFiles, string outFile, imgProps *outProps);
      ImageP(string outFile, imgProps *outProps);
      ImageP(string inFile);
      ImageP(vector<string>inFiles);
      ~ImageP();
      imgProps getInProps(void);
      void process(void);
      void processI2(void);
      void processLine(int line);
      virtual unsigned char eval(int k) = 0;
      virtual short evalI2(int k) = 0;
      void xy2ls(double x, double y, double *sample, double *line);

      int currentLine;

   protected:
      GDALDataset *inputImage;
      GDALDataset *outputImage;       
      GDALRasterBand *outputBand;
      GDALRasterBand *inputBand;

      vector<imagery> inputImagery;

      int nl;
      int ns;
      int nb;

      int out_nl;
      int out_ns;
      int out_nb;

      double ULx, ULy;		// Upper Left projection coordinates
      double PxSize;		// Pixel Size

      float *theData;
      unsigned char *theDataOut;
      short *theDataOutI2;
      GDALDataType outType;

      //int getCurrentBand(void);
      int currentBand;

   private:
      void openOutputFile(string outFile, imgProps *outProps); 
      void openInputFiles(vector<string>inFiles);
      void openInputFile(string inFile);
      int processType;
   };
