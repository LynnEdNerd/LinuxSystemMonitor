#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// This routine takes a filename and a key value.  When it locates the key in the file, it extracts the first token
// and returns the value.  This is a function local to linux_parser.cpp. 
string LocateKeyValuePair(string key, string filepath ) {

  string value;
  string keycandidate;
  string returnvalue;
  string line;

 // Return an empty string if the key is not found.
 returnvalue.clear();

 std::ifstream stream(filepath);
 if (stream.is_open()) {
   while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      // Extract the key and value from the current line of the file.
      linestream >> keycandidate >> value;
      if (keycandidate == key) {
        returnvalue = value;
        // Key found.  
        break;
      }
    }
  }
  return(returnvalue);
}

// This structure is used for the LinuxParser::GetProcessAttributeValues() routine below.  Because there are 5 tokens in the first 22
// that are acutally needed, this allows the user to set up a table that tells which tokens are needed and which ones are not.  This is used for 
// The information in /proc/[pid]/stat.
struct ProcPidStatInfo {
  bool TokenNeeded;
  string& Token;
};

// This routine extracts values 14-17 and 22 from /proc/[pid]/stat.
void  LinuxParser::GetProcessAttributeValues(int pid, string& utime, string& stime, string& cutime, string& cstime, string& starttime)
{
  string discardtoken;

  std::vector<struct ProcPidStatInfo> parseinfo {
    {false, discardtoken}, // Token 1
    {false, discardtoken}, // Token 2
    {false, discardtoken}, // Token 3
    {false, discardtoken}, // Token 4
    {false, discardtoken}, // Token 5
    {false, discardtoken}, // Token 6
    {false, discardtoken}, // Token 7
    {false, discardtoken}, // Token 8
    {false, discardtoken}, // Token 9
    {false, discardtoken}, // Token 10
    {false, discardtoken}, // Token 11
    {false, discardtoken}, // Token 12
    {false, discardtoken}, // Token 13
    {true,  utime},        // Token 14
    {true,  stime},        // Token 15
    {true,  cutime},       // Token 16
    {true,  cstime},       // Token 17
    {false, discardtoken}, // Token 18
    {false, discardtoken}, // Token 19
    {false, discardtoken}, // Token 20
    {false, discardtoken}, // Token 21
    {true,  starttime}     // Token 22
  };
  string filepath;
  string line;
  string token;
  std::ostringstream filepathbuilder;

  filepathbuilder << LinuxParser::kProcDirectory << pid << LinuxParser::kStatFilename;
  filepath = filepathbuilder.str();
  std::ifstream stream(filepath);
  if (stream.is_open()) {
   std::getline(stream, line);
   std::istringstream linestream(line);

    for (auto currenttoken : parseinfo ) {
      linestream >> token;
      if (currenttoken.TokenNeeded) {
        currenttoken.Token = token;
      }
    }
  }
  else
  {
    // return empty tokens if unable to open the file.
    utime.clear();
    stime.clear();
    cutime.clear();
    cstime.clear();
    starttime.clear();
  }
  
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// This routine returns a fraction that is (memory used)/(total memeory).  The information is found
// in /proc/meminfo.
float LinuxParser::MemoryUtilization() { 
  string totalmemstr;
  string memfreestr;
  float totalmem = 0;
  float memfree = 0;

  totalmemstr.clear();
  memfreestr.clear();

  totalmemstr = LocateKeyValuePair(MemTotalKey, kProcDirectory + kMeminfoFilename);
  if (!totalmemstr.empty()) {
    totalmem = std::stof(totalmemstr);
  }

  memfreestr = LocateKeyValuePair(MemFreeKey, kProcDirectory + kMeminfoFilename);
  if (!memfreestr.empty()) {
    memfree = std::stof(memfreestr);
  }

  if (totalmem != 0.0) {
    return ( (totalmem - memfree)/totalmem );
  } else {
    return 0.0; 
  }
}

// This routine returns the value in seconds corresponding to the time the system has been up and running.
// It gets this infoprmation from /proc/uptime.
long LinuxParser::UpTime() { 
  string line;
  float uptime;
  string uptimestr;

  uptimestr.clear();

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptimestr;
  }
  if (!uptimestr.empty()) {
    uptime = stof(uptimestr);
  } else {
    uptime = 0.0;
  }
  return (uptime); 
}

// THis routine opens the /proc/stat file and extracts the first line of data (tokenized).
// Each token contains information about the aggregate CPU workload.  This raw information can 
// then be used to calculate the CPU utilization.
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpudata;
  string timetoken;
  string line;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> timetoken) {
      cpudata.push_back(timetoken);
    }
  }
 
  return {cpudata}; 
}

// This routine seeks out the total number of processes.  This iinfrmation is contained in
// /proc/stat
int LinuxParser::TotalProcesses() { 
  string totalprocsstr;
  int totalprocs = 0;

  totalprocsstr.clear();

  totalprocsstr = LocateKeyValuePair(ProcsTotalKey, kProcDirectory + kStatFilename);
  if (!totalprocsstr.empty()) {
    totalprocs = std::stoi(totalprocsstr);
  }

  return totalprocs; 
}

// This routine extracts the number of runnikng processes from /proc/stat.
int LinuxParser::RunningProcesses() { 
  string totalrunningprocsstr;
  int totalrunningprocs = 0;

  totalrunningprocsstr.clear();

  totalrunningprocsstr = LocateKeyValuePair(ProcsRunningKey, kProcDirectory + kStatFilename);
  if (!totalrunningprocsstr.empty()) {
    totalrunningprocs = std::stoi(totalrunningprocsstr);
  }

  return totalrunningprocs; 

}

// THis routine extracts the command line that invoked this process.  This information is found in /proc/[pid]/cmdline.
string LinuxParser::Command(int pid) { 
  std::ostringstream filepathbuilder;
  string filepath;
  string command;

  command.clear();

  filepathbuilder << LinuxParser::kProcDirectory << pid << LinuxParser::kCmdlineFilename;
  filepath = filepathbuilder.str();
  std::ifstream stream(filepath);
  if (stream.is_open()) {
    std::getline(stream, command);
  }

  return(command);
}

string LinuxParser::Ram(int pid) {
  string memoryused;
  int numericmemory;
  std::ostringstream filepathbuilder;
  filepathbuilder << kProcDirectory << pid << kStatusFilename;
  memoryused = LocateKeyValuePair(ProcessMemKey, filepathbuilder.str());
  numericmemory = stoi(memoryused);
  // System maintains Kytes, but the interface wishes to display MBs.
  numericmemory = numericmemory / 1000;
  std::ostringstream memreturnvalue;
  memreturnvalue << numericmemory;
  return(memreturnvalue.str());
}

// This routine extracts the user id of the user that owns the process in question.  The information is found
// in /proc/[pid]/status.
string LinuxParser::Uid(int pid) { 
  string filename;
  string  uid;
  std::ostringstream filebuilder;

  filebuilder << kProcDirectory << pid << kStatusFilename;
  filename = filebuilder.str();
  // Construct file name from Pid and extract Ui
  uid = LocateKeyValuePair(UidKey, filename);

  return uid; 
}

// This routine takes the user id and returns the user name corresponding to that ID. It finds the information
// in /etc/passwd.
string LinuxParser::User(string uid) {
  string username;
  string line;
  string fileuserid;
  string password;

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream parsestream(line);
      parsestream >> username >> password >> fileuserid;
      if (uid == fileuserid) {
        return(username);
      }
    }
  }
  return(username);
}