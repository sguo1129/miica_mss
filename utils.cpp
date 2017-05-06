#include <iostream>
#include <sstream>
#include <cstdlib>
#include "utils.h"

// Routine to calculate a Julian Date (day of year) from Year, Month, Day for positive years.
//
// Algorithm by David Burki, C User's Journal, Feb 1993.
// ----------------------------------------------------
int jd(int year, int month, int day) {
    int jul_day;

    int a,b;

    if ( month <= 2 ) {
        year--;
        month += 12;
    }
    b = 0;
    if ( year * 10000.0 + month * 100.0 + day >= 15821015.0 ) {
        a = year / 100;
        b = 2 - a + a / 4;
    }
    jul_day = (int)((long) ( 365.25 * year) +
              (long) ( 30.6001 * ( month + 1 ) ) + day + 1720995L + b);

    return jul_day;
}

// Routine to calculate day-of-year
// --------------------------------
int doy(int year, int month, int day) {
   return (jd(year, month, day) - jd(year, 1, 1) + 1);
}

// Routine to return the year from a yyyy-mm-dd string
// --------------------------------------------------------
int getYear(string theDate) {
   string year = theDate.substr(0,4);
   //cout << "***" << year << "***\n";
   return atoi(year.c_str());
}

// Routine to return the month from a yyyy-mm-dd string
// ---------------------------------------------------------
int getMonth(string theDate) {

   string::size_type dashPos = theDate.find("-");
   if (dashPos == string::npos) {
      cout << "Parse for MONTH in date field failed!\n";
      exit(-1);
   }
   string month = theDate.substr(dashPos+1, 2);
   //cout << "***" << month << "***\n";
   return atoi(month.c_str());
}

// Routine to return the month from a yyyy-mm-dd string
// ---------------------------------------------------------
int getMonth2(string theDate) {

   string::size_type dashPos = theDate.find(" ");
   if (dashPos == string::npos) {
      cout << "Parse for MONTH in date field failed!\n";
      exit(-1);
   }
   string month = theDate.substr(dashPos+1, 2);
   //cout << "***" << month << "***\n";
   return atoi(month.c_str());
}

// Routine to return the day from a yyyy-mm-dd string
// -------------------------------------------------------
int getDay(string theDate) {

   string::size_type dashPos = theDate.find_last_of("-");
   if (dashPos == string::npos) {
      cout << "Parse for DAY in date field failed!\n";
      exit(-1);
   }
   string day = theDate.substr(dashPos+1, 2);
   //cout << "***" << day << "***\n";
   return atoi(day.c_str());
}

// Routine to return the day from a yyyy-mm-dd string
// -------------------------------------------------------
int getDay2(string theDate) {

   string::size_type dashPos = theDate.find_last_of(" ");
   if (dashPos == string::npos) {
      cout << "Parse for DAY in date field failed!\n";
      exit(-1);
   }
   string day = theDate.substr(dashPos+1, 2);
   //cout << "***" << day << "***\n";
   return atoi(day.c_str());
}

// Returns text inside a quoted string
// -----------------------------------
string stripQ(string theStr) {
   string::size_type startPos = theStr.find_first_of('"');     // Index of first Quote mark 
   string::size_type endPos = theStr.find_last_of('"');       // Index of last Quote mark 
   return(theStr.substr(startPos+1,endPos-startPos-1));
}

// Convert an integer to a string
// -----------------------------------
string itos(int i)	{
   stringstream s;
   s << i;
   return s.str();
}

// Routine to calculate the Earth <--> Sun distance
// ------------------------------------------------

#define NDSUN (25)

typedef struct {
   int doy;         /* Day of year */
   float dsun;      /* Relative sun-earth distance */
} Dsun_table_t;
 
const Dsun_table_t dsun_table[NDSUN] = {
   {  1, 0.9832}, { 15, 0.9836}, { 32, 0.9853}, { 46, 0.9878}, { 60, 0.9909},
   { 74, 0.9945}, { 91, 0.9993}, {106, 1.0033}, {121, 1.0076}, {135, 1.0109}, 
   {152, 1.0140}, {166, 1.0158}, {182, 1.0167}, {196, 1.0165}, {213, 1.0149},
   {227, 1.0128}, {242, 1.0092}, {258, 1.0057}, {274, 1.0011}, {288, 0.9972},
   {305, 0.9925}, {319, 0.9892}, {335, 0.9860}, {349, 0.9843}, {366, 0.9833}
};
 
// Routine to calculate sun-earth distance  (Ref. L7 handbook)
// -----------------------------------------------------------
float calc_d(int jdoy) {

   int i;
   float dsun;

   for (i = 0; i < (NDSUN - 1); i++) {
     if (jdoy >= dsun_table[i].doy  &&  jdoy <= dsun_table[i+1].doy)
       break;
   }
 
   if (i >= (NDSUN - 1)) {
     cout << "Error finding sun-earth distance:  julian date out of range!\n";
     return(-999.9);
   }
 
   dsun = dsun_table[i].dsun + ((jdoy - dsun_table[i].doy) * 
           ((dsun_table[i + 1].dsun - dsun_table[i].dsun) /
            (dsun_table[i + 1].doy - dsun_table[i].doy)));
   return(dsun);
}
 
