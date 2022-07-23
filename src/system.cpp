#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <cmath>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { 
  return cpu_; 
}
 
// This routine gathers all the process ID's from /proc.  It then creates an object for each process, then sorts the list in 
// descending order based on CPU utilization.
vector<Process>& System::Processes() {
  vector<int> processids;
  string userid;


  // Since we are dynamically building the process list, clear the information from the last time this method was run.
  processes_.clear();
  processids = LinuxParser::Pids();
  for (int currentpid : processids) {
    Process newprocess(currentpid);
    processes_.push_back(newprocess);
  }

  // Use the sort routine.  Reverse the outcome since sort defaults to ascending order.
  sort(processes_.begin(), processes_.end());
  reverse(processes_.begin(), processes_.end());
  
  return processes_; 
}

std::string System::Kernel() { 
  return LinuxParser::Kernel(); 
}

float System::MemoryUtilization() { 
  return(LinuxParser::MemoryUtilization()); 
}

std::string System::OperatingSystem() { 
  return LinuxParser::OperatingSystem(); 
}

int System::RunningProcesses() { 
  return LinuxParser::RunningProcesses(); 
}

int System::TotalProcesses() { 
  return(LinuxParser::TotalProcesses());
}

long int System::UpTime() { 
  float floatuptime;
  long int uptime;

  floatuptime = LinuxParser::UpTime(); 
  floatuptime = std::round(floatuptime);
  uptime = static_cast<long int>(floatuptime);
  return(uptime);
}