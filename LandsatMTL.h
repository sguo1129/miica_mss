#include <string>
#include <map>

using namespace std;

   class metaDataL1T
   {
   public:
      metaDataL1T(char *fileName);
      string get(string keyVal);

   private:
      map<string,string> theMetaData;
   };
