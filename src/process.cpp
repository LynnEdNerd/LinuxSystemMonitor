#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { 
  return (pid_); 
}

float Process::CpuUtilization() const { 
  string  utime;
  string  stime;
  string  cutime;
  string  cstime;
  string  starttime;
  float   elapsedtime;
  long    uptime;
  float   processtime;
  float   hz;
  float   return_value;
  


  LinuxParser::GetProcessAttributeValues(pid_, utime, stime, cutime, cstime, starttime);
  uptime = LinuxParser::UpTime();

  hz = static_cast<float>(sysconf (_SC_CLK_TCK));
  processtime = stof(utime) + stof(stime) + stof(cutime) + stof (cstime);
  processtime /= hz;
  elapsedtime = static_cast<float>(uptime) - static_cast<float>(stol(starttime) / hz);
  
  if ( elapsedtime != 0 ) {
    return_value = processtime / elapsedtime;
  } else {
    return_value = (0.0 );
  }
  return(return_value);
}

string Process::Command() { 
  string command;
  command = LinuxParser::Command(pid_);
  return(command) ; 
}

string Process::Ram() { 
  return LinuxParser::Ram(pid_); 
}

// Obtain the user id associated with the process.  Take the user id and obtain the user name with it.
string Process::User() {
  string user;
  string username;
  user = LinuxParser::Uid(pid_);
  username = LinuxParser::User(user);
  return (username); 
}

// Calculate the amount of time a process has been alive.  This is done by taking the time the Linux system has been running and subtracting 
// the time the process began running.
long int Process::UpTime() { 
  string processbegintime;
  // Most of the attributes from GetProcessAttributeValues are not needed for this calculation.
  string discardattribute; 

  long returnvalue;
  LinuxParser::GetProcessAttributeValues(pid_, discardattribute, discardattribute, discardattribute, discardattribute, processbegintime);
  returnvalue = LinuxParser::UpTime( ) - stol(processbegintime) / sysconf(_SC_CLK_TCK);

  return (returnvalue); 
}

// Compare 2 process objects by comparing the cpu utilization.
bool Process::operator<(Process const& a) const { 

  if (this->CpuUtilization() < a.CpuUtilization()) {
    return true; 
  } 
  return false;
}