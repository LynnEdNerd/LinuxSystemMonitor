#include <vector>
#include <string>
#include "processor.h"
#include "linux_parser.h"

// This method returns the aggregate processor utilization.  It makes the calculation by dividing the 
// amount of time it is active by the total time.  Various processing times returned by LinuxProcessor::
float Processor::Utilization() {
  std::vector<std::string> cpudata;

  long int totalprocessingtime = 0;
  long int idleprocessingtime;
  long int currententrytime;


  cpudata = LinuxParser::CpuUtilization();

  // We start with the index = 1.  The 0th value of the vector is the label "cpu"
  for (unsigned int i = 1; i < cpudata.size(); i++ ) {
    currententrytime = stoi(cpudata[i]);
    totalprocessingtime += currententrytime;
    if (i == LinuxParser::PROC_STAT_CPU_IDLE_INDEX_ENTRY) {
      idleprocessingtime = currententrytime;
    }
  }

  return static_cast<float>(totalprocessingtime - idleprocessingtime)/static_cast<float> (totalprocessingtime);
}