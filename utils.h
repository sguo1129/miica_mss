#include <string>

using namespace std;

#define PI 3.141592653589793238
#define D2R (PI/180.0)

// Solar Spectral Irradiances
//    Units = watts / (meter squared * micrometers)
//    Reference:
//        ETM+ values are from the Landsat 7 User Handbook (see above)
//        TM and MSS values from EOSAT Landsat Technical Notes, August 1986
//
//        New version of L4 and L5 given Solar Irradinaces in (mW/m^2-nm) Jeff Masek/Brian Markham on 11/3/06 :
//              L7: 1969      1840      1551      1044    225.7      82.07     1368
//              L5: 1957      1826      1554      1036    215.0      80.67
//              L4: 1957      1825      1557      1033    214.9      80.72
//  ***********************************************************************************
// Note this routine does not deal with thermal nor pan bands...!
const float esun_etm[8] = { 0.0, 1969.000, 1840.000, 1551.000, 1044.000, 225.700, 0.0, 82.07 };
const float esun_tm_4[8] = { 0.0, 1957.0, 1825.0, 1557.0, 1033.0, 214.9, 0.0, 80.72 };
const float esun_tm_5[8] = { 0.0, 1957.0, 1826.0, 1554.0, 1036.0, 215.0, 0.0, 80.67 };
const float esun_mss_1[8] = {0.0, 0.0, 0.0, 0.0, 1823.0, 1559.0, 1276.0, 880.1};
const float esun_mss_2[8] = {0.0, 0.0, 0.0, 0.0, 1829.0, 1539.0, 1268.0, 886.6};
const float esun_mss_3[8] = {0.0, 0.0, 0.0, 0.0, 1839.0, 1555.0, 1291.0, 887.9};
const float esun_mss_4[5] = {0.0, 1827.0, 1569.0, 1260.0, 866.4};
const float esun_mss_5[5] = {0.0, 1824.0, 1570.0, 1249.0, 853.4};

// Thermal...
const float K1_tm_4=  671.62;
const float K2_tm_4= 1284.30;
const float K1_tm_5=  607.76;
const float K2_tm_5= 1260.56;
const float K1_etm =  666.09;
const float K2_etm = 1282.71;

int doy(int year, int month, int day);		// Calculates day of year
int jd(int year, int month, int day);		// Calculates Julian Day from Year, Month, Day
float calc_d(int jdoy);				// Calculates Earth <--> Sun distance for given Julian Day

int getYear(string theDate);
int getMonth(string theDate);
int getDay(string theDate);
int getMonth2(string theDate);
int getDay2(string theDate);
string stripQ(string theStr);
string itos(int i);
