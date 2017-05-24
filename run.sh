#!/bin/bash

# The script needs 4 input cariables, the first one is data directory name, the second one
# is first processing scene_name name, the third one is second processing name, the fourth one is output name. 
# The input and final output files are in the data directory, and the temporary text files are in the current 
# code directory and will be removed after their usages
# e.g. ./run.sh /shared/users/sguo/mss_data LM50450301985132AAA04 LM50450301986135AAA03 LM5045030_1985_1986

./parseMTL $1/$2"_MTL.txt"
./parseMTL $1/$3"_MTL.txt"
scene=$2
echo $scene
sensor=${scene:2:1}
echo $sensor
if ( (("$sensor" == 4)) || (("$sensor" == 5)) ); then
   ./calcTOA -sc $1/$2"_B1_info.txt"
   ./calcTOA -sc $1/$2"_B2_info.txt"
   ./calcTOA -sc $1/$2"_B3_info.txt"
   ./calcTOA -sc $1/$2"_B4_info.txt"
   ./calcTOA -sc $1/$3"_B1_info.txt"
   ./calcTOA -sc $1/$3"_B2_info.txt"
   ./calcTOA -sc $1/$3"_B3_info.txt"
   ./calcTOA -sc $1/$3"_B4_info.txt"
elif ( (("$sensor" == 1)) || (("$sensor" == 2)) || (("$sensor" == 3)) ); then
   ./calcTOA -sc $1/$2"_B4_info.txt"
   ./calcTOA -sc $1/$2"_B5_info.txt"
   ./calcTOA -sc $1/$2"_B6_info.txt"
   ./calcTOA -sc $1/$2"_B7_info.txt"
   ./calcTOA -sc $1/$3"_B4_info.txt"
   ./calcTOA -sc $1/$3"_B5_info.txt"
   ./calcTOA -sc $1/$3"_B6_info.txt"
   ./calcTOA -sc $1/$3"_B7_info.txt"
else
   echo "Invalid MSS sensor number"
fi
#rm $1/*_info.txt
./calcMIICA_noMask_MSS $1/$2 $1/$3 $1/$4
