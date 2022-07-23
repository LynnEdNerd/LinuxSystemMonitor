#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime( );
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(std::string uid);
void GetProcessAttributeValues(int pid, std::string& utime, std::string& stime, std::string& cutime, std::string& cstime, std::string& starttime);


// Definitions for parsing the system and process files
const std::string ProcsRunningKey = "procs_running";
const std::string ProcsTotalKey = "processes";
const std::string MemTotalKey = "MemTotal:";
const std::string MemFreeKey = "MemFree:";
const std::string UidKey = "Uid:";
const std::string ProcessMemKey = "VmSize:";

// In /proc/stat, the first line has various CPU processing times.  Indexing from 0, the 0th entry is the key "CPU."
// It is then followed by various values specifying the amount of processing time spent on various activities.
// The index of the idle time is defined below, indexing from 0.  All other entries are
// active processing time where the CPU is doing work.
const int PROC_STAT_CPU_IDLE_INDEX_ENTRY = 4;
};  // namespace LinuxParser
#endif