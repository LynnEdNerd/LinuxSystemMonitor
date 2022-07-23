#include <string>
#include <sstream>
#include <cstring>

#include "format.h"


using std::string;
const int seconds_per_hour {360};
const int seconds_per_minute {60};

string Format::ElapsedTime(long seconds) { 
    int formatseconds;
    int formatminutes;
    int formathours;
    string returnstring;
    char OutputBuffer[std::strlen("00:00:00") + 1];

    formathours = seconds / seconds_per_hour ;
    seconds = seconds % seconds_per_hour ;
    formatminutes = seconds / seconds_per_minute;
    formatseconds = seconds % seconds_per_minute;
    
    if (formathours <= 99) {
      std::sprintf(OutputBuffer,"%02d:%02d:%02d",formathours, formatminutes,formatseconds);
    } else {
      // Remain at 1 second from overflow.
      strcpy(OutputBuffer,"99:59:59");
    }
    

    returnstring = string(OutputBuffer);
    return returnstring;
}