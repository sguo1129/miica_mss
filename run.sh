#!/bin/bash

# The script needs 4 input cariables, the first one is data directory name, the second one
# is first processing scene_name name, the third one is second processing name, the fourth one is output name. 
# The input and final output files are in the data directory, and the temporary text files are in the current 
# code directory and will be removed after their usages
# e.g. ./run.sh /shared/users/sguo/mss_data LM50450301985132AAA04 LM50450301986135AAA03 LM5045030_1985_1986

./parseMTL $1/$2"_MTL.txt"
if [ $? -eq 0 ]
then
  echo "Successfully pasred" $1/$2"_MTL.txt" 
else
  echo "Could not parse" $1/$2"_MTL.txt" "as some band data are missing"
  exit
fi

./parseMTL $1/$3"_MTL.txt"
if [ $? -eq 0 ]
then
  echo "Successfully pasred" $1/$2"_MTL.txt"
else
  echo "Could not parse" $1/$2"_MTL.txt" "as some band data are missing" 
  exit
fi

scene=$2
sensor=${scene:2:1}
if ( (("$sensor" == 4)) || (("$sensor" == 5)) ); then
   ./calcTOA -sc $1/$2"_B1_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$2 "TOA reflectance for band1"
    else
     echo "Could not calculated" $1/$2 "TOA reflectance for band1" 
     exit
   fi

   ./calcTOA -sc $1/$2"_B2_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$2 "TOA reflectance for band2"
   else
     echo "Could not calculated" $1/$2 "TOA reflectance for band2" 
     exit
   fi

   ./calcTOA -sc $1/$2"_B3_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$2 "TOA reflectance for band3"
   else
     echo "Could not calculated" $1/$2 "TOA reflectance for band3" 
     exit
   fi

   ./calcTOA -sc $1/$2"_B4_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$2 "TOA reflectance for band4"
   else
     echo "Could not calculated" $1/$2 "TOA reflectance for band4" 
     exit
   fi

   ./calcTOA -sc $1/$3"_B1_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$3 "TOA reflectance for band1"
   else
     echo "Could not calculated" $1/$3 "TOA reflectance for band1" 
     exit
   fi

   ./calcTOA -sc $1/$3"_B2_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$3 "TOA reflectance for band2"
   else
     echo "Could not calculated" $1/$3 "TOA reflectance for band2" 
     exit
   fi

   ./calcTOA -sc $1/$3"_B3_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$3 "TOA reflectance for band3"
   else
     echo "Could not calculated" $1/$3 "TOA reflectance for band3" 
     exit 
  fi

   ./calcTOA -sc $1/$3"_B4_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$3 "TOA reflectance for band4"
   else
     echo "Could not calculated" $1/$3 "TOA reflectance for band4" 
     exit
   fi
elif ( (("$sensor" == 1)) || (("$sensor" == 2)) || (("$sensor" == 3)) ); then
   ./calcTOA -sc $1/$2"_B4_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$2 "TOA reflectance for band4"
   else
     echo "Could not calculated" $1/$2 "TOA reflectance for band4" 
     exit
   fi

   ./calcTOA -sc $1/$2"_B5_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$2 "TOA reflectance for band5"
   else
     echo "Could not calculated" $1/$2 "TOA reflectance for band5" 
     exit
   fi

   ./calcTOA -sc $1/$2"_B6_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$2 "TOA reflectance for band6"
   else
     echo "Could not calculated" $1/$2 "TOA reflectance for band6" 
     exit
   fi

   ./calcTOA -sc $1/$2"_B7_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$2 "TOA reflectance for band7"
   else
     echo "Could not calculated" $1/$2 "TOA reflectance for band7" 
     exit
   fi

   ./calcTOA -sc $1/$3"_B4_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$3 "TOA reflectance for band4"
   else
     echo "Could not calculated" $1/$3 "TOA reflectance for band4" 
     exit
   fi

   ./calcTOA -sc $1/$3"_B5_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$3 "TOA reflectance for band5"
   else
     echo "Could not calculated" $1/$3 "TOA reflectance for band5" 
     exit 
  fi

   ./calcTOA -sc $1/$3"_B6_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$3 "TOA reflectance for band6"
   else
     echo "Could not calculated" $1/$3 "TOA reflectance for band6" 
     exit
   fi

   ./calcTOA -sc $1/$3"_B7_info.txt"
   if [ $? -eq 0 ]
   then
     echo "Successfully calculated" $1/$3 "TOA reflectance for band7"
   else
     echo "Could not calculated" $1/$3 "TOA reflectance for band7" 
     exit
   fi
else
   echo "Invalid MSS sensor number"
   exit
fi
./clipImages $1/$2 $1/$3
if [ $? -eq 0 ]
then
  echo "Successfully clipped images " $1/$2 " and " $1/$3
else
  echo "Could not clipped images " $1/$2 " and " $1/$3
  exit
fi

#rm $1/*_info.txt
./calcMIICA_noMask_MSS $1/$2 $1/$3 $1/$4
if [ $? -eq 0 ]
then
  echo "Successfully done with MIICA algorithm calculation for " $1/$2 " and " $1/$3
else
  echo "Could not done with MIICA algorithm calculation for " $1/$2 " and " $1/$3
  exit
fi

