#include <string>
#include "GetSysInfo.h"
//#include "glog/logging.h"
#ifdef WIN32
#include <Shlwapi.h>
#include <windows.h>
#include <psapi.h>
#pragma comment(lib,"pdh.lib")
#pragma comment(lib,"Shlwapi.lib")
#endif

GetSysInfo::GetSysInfo(const std::string& name ) :pidName(name)
{
#ifdef WIN32
	m_CpuQuery = NULL;
	m_CpuTotal = NULL;
	m_ThreadCount = NULL;
#endif

}

GetSysInfo::~GetSysInfo()
{
#ifdef WIN32
	if (m_CpuTotal)
	{
		PdhRemoveCounter(m_CpuTotal);
		m_CpuTotal = NULL;
	}
	if (m_ThreadCount)
	{
		PdhRemoveCounter(m_ThreadCount);
		m_ThreadCount = NULL;
	}
	if (m_CpuQuery) {
		PdhCloseQuery(m_CpuQuery);
		m_CpuQuery = NULL;
	}
#endif
	SystemCpuUnInit();

}

void  GetSysInfo::SystemCpuInit()
{
#ifdef WIN32
	PDH_STATUS status;
	status = PdhOpenQuery(NULL, NULL, &m_CpuQuery);
	if (status != ERROR_SUCCESS) {
		// 处理错误
		LOG(ERROR) << "打开 Pdh 查询句柄失败 错误码 ：" << GetLastError();
		return ;
	}
	status = PdhAddCounter(m_CpuQuery, ("\\Processor(_Total)\\% Processor Time"), NULL, &m_CpuTotal);
	if (status != ERROR_SUCCESS) {
		// 处理错误
		LOG(ERROR) << "累加 Pdh 数值失败 错误码 ：" << GetLastError() ;
		//PdhCloseQuery(query);
		return;
	}
	status = PdhCollectQueryData(m_CpuQuery);
	if (status != ERROR_SUCCESS) {
		// 处理错误
		LOG(ERROR) << "搜集 Pdh 查询信息失败 错误码 ：" << GetLastError();
		//PdhCloseQuery(query);
		return;
	}
#endif
}

void  GetSysInfo::SystemCpuInit(std::string& processName)
{
#ifdef WIN32
	PDH_STATUS status;
	status = PdhOpenQuery(NULL, NULL, &m_CpuQuery);
	if (status != ERROR_SUCCESS) {
		// 处理错误
		LOG(ERROR) << "打开 Pdh 查询句柄失败 错误码 ：" << GetLastError();
		return;
	}
	//std::cout << "process id is :" << processID << std::endl;
	std::string counterPath = "\\Process(" + processName + ")\\% Processor Time";
	//std::cout << "---111---" << std::endl;
	//sprintf(counterPath,TEXT("\\Process(%s)\\ % Processor Time"), processName.c_str());
	//std::cout << "counterPath is :" << counterPath << std::endl;
	status = PdhAddCounter(m_CpuQuery, (counterPath.c_str()), NULL, &m_CpuTotal);
	if (status != ERROR_SUCCESS) {
		// 处理错误
		LOG(ERROR) << "累加 Pdh 数值失败 错误码 ：" << GetLastError();
		//PdhCloseQuery(query);
		return;
	}
	std::string threadCounterPath = "\\Process(" + processName + ")\\Thread Count";
	status = PdhAddCounter(m_CpuQuery, (threadCounterPath.c_str()), NULL, &m_ThreadCount);
	if (status != ERROR_SUCCESS) {
		// 处理错误
		LOG(ERROR) << "累加 Thread Pdh 数值失败 错误码 ：" << GetLastError();
		//PdhCloseQuery(query);
		return;
	}
	status = PdhCollectQueryData(m_CpuQuery);
	if (status != ERROR_SUCCESS) {
		// 处理错误
		LOG(ERROR) << "搜集 Pdh 查询信息失败 错误码 ：" << GetLastError();
		//PdhCloseQuery(query);
		return;
	}
#endif
}

void GetSysInfo::SystemCpuUnInit()
{
#ifdef WIN32
	if (m_CpuTotal) {
		PdhRemoveCounter(m_CpuTotal);
		m_CpuTotal = NULL;
	}
	if (m_ThreadCount) {
		PdhRemoveCounter(m_ThreadCount);
		m_ThreadCount = NULL;
	}
	if (m_CpuQuery) {
		PdhCloseQuery(m_CpuQuery);
		m_CpuQuery = NULL;
	}
#endif
}

double GetSysInfo::GetSystemCpuCurrentUsage()
{
    double UsagePercent;
#ifdef WIN32
	PDH_FMT_COUNTERVALUE counterVal;
	Sleep(1000);
	PdhCollectQueryData(m_CpuQuery);
	PDH_STATUS status = PdhGetFormattedCounterValue(m_CpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
	if (status != ERROR_SUCCESS) {
		std::cerr  << "转换 LONG VALUE 格式失败，status is : " << GetLastError() << std::endl;
		//PdhCloseQuery(query);
		// 处理错误
		return 0;
	}
	UsagePercent = counterVal.doubleValue;
#else
	struct cpuinfo cpuoccupy1;
    struct cpuinfo cpuoccupy2;
    get_cpuoccupy(cpuoccupy1);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    get_cpuoccupy(cpuoccupy2);
    UsagePercent = calc_cpuoccupy(cpuoccupy1,cpuoccupy2);
#endif
	return UsagePercent;
    
}

double GetSysInfo::GetProcCpuCurrentUsage()
{
    double usage = 0;
#ifdef WIN32
#else
    struct cpuinfo cpuoccupy1;
    struct cpuinfo cpuoccupy2;
    struct cpuinfo cpuoccupy3;
    struct cpuinfo cpuoccupy4;

    get_cpuoccupy(cpuoccupy1);
    uint32_t pid = 0;
    get_cpuoccupy(cpuoccupy2,pid);
    std::this_thread::sleep_for(std::chrono::seconds(1));
     get_cpuoccupy(cpuoccupy3);
     get_cpuoccupy(cpuoccupy4,pid);
     uint32_t sumcputime = cpuoccupy3.user +  cpuoccupy3.nice + cpuoccupy3.system + cpuoccupy3.idle + cpuoccupy3.iowait + cpuoccupy3.irq + cpuoccupy3.softirq + cpuoccupy3.steal -  \
                            (cpuoccupy1.user +  cpuoccupy1.nice + cpuoccupy1.system + cpuoccupy1.idle + cpuoccupy1.iowait + cpuoccupy1.irq + cpuoccupy1.softirq + cpuoccupy1.steal);
    uint32_t proccputime = cpuoccupy4.user +  cpuoccupy4.system - cpuoccupy2.user - cpuoccupy2.system;

    usage = 100 * double(proccputime) / double(sumcputime);
#endif
    return usage;
}

inline void GetSysInfo::get_cpuoccupy(cpuinfo& cpuoccupy)
{
    std::ifstream file("/proc/stat");
    std::string line;
   if (std::getline(file, line)) 
   {
        std::istringstream ss(line);
        ss >> cpuoccupy.name >> cpuoccupy.user >> cpuoccupy.nice >> cpuoccupy.system >> cpuoccupy.idle >> cpuoccupy.iowait >> cpuoccupy.irq >> cpuoccupy.softirq >> cpuoccupy.steal;
   }

}

inline void GetSysInfo::get_cpuoccupy(cpuinfo& cpuoccupy,uint32_t& pid)
{
    std::ifstream file("/proc/self/stat");
    std::string line;
   if (std::getline(file, line)) 
   {
        //std::cout << "line = " << line << std::endl;
        std::istringstream ss(line);
        std::string pid, comm, state, ppid, pgrp, session, tty_nr, tpgid, flags;
        uint32_t minflt,cminflt,majflt;
        ss >> cpuoccupy.pid >> cpuoccupy.name >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags ;
        ss >> minflt >> cminflt >> majflt >> majflt >> cpuoccupy.user >> cpuoccupy.system ;
   }
   //std::cout << "proc time = " << cpuoccupy.user + cpuoccupy.system << std::endl;

}

inline double GetSysInfo::calc_cpuoccupy(cpuinfo& cpuoccupyf,cpuinfo& cpuoccupys)
{
    double cpuUsage;
    uint32_t ftotal,stotal;
    ftotal = cpuoccupyf.user +  cpuoccupyf.nice + cpuoccupyf.system + cpuoccupyf.idle + cpuoccupyf.iowait + cpuoccupyf.irq + cpuoccupyf.softirq + cpuoccupyf.steal;
    stotal = cpuoccupys.user +  cpuoccupys.nice + cpuoccupys.system + cpuoccupys.idle + cpuoccupys.iowait + cpuoccupys.irq + cpuoccupys.softirq + cpuoccupys.steal;
    double sumtime = stotal - ftotal;
    double sumidle = cpuoccupys.idle +  cpuoccupys.iowait - cpuoccupyf.idle - cpuoccupyf.iowait;

    cpuUsage = 100.0 * (sumtime - sumidle) / sumtime;

    return cpuUsage;

}

uint32_t GetSysInfo::get_pid()
{
    uint32_t currPid;
#ifdef WIN32
    DWORD pid = GetCurrentProcessId();
    currPid = pid;
#else
    pid_t pid = getpid();
    currPid  = pid;
#endif
    std::cout << "pid = " << currPid << std::endl;
    return currPid;
}

double GetSysInfo::GetTotalPhysicalMemory()
{
    double memUsage;
#ifdef WIN32
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	double physMemUsed = (((memInfo.ullTotalPhys - memInfo.ullAvailPhys) / 1024 / 1024) * 100) / 1024;;
	memUsage = physMemUsed / 100.0;
#else
    std::ifstream file("/proc/meminfo");
    std::string line;
    uint32_t totalMem = 0;
    uint32_t availMem = 0;
    if(file.is_open())
    {
        while(std::getline(file,line))
        {
            std::istringstream ss(line);
            std::string key,unit;
            std::string value ;
            ss >> key >> value >> unit;
            if(key == "MemTotal:")   totalMem = std::stoi(value);
            if(key == "MemAvailable:")   availMem = std::stoi(value);
        }
    }
    memUsage = (100.0 * (totalMem - availMem)) / totalMem;
#endif
    return memUsage;
}


void GetSysInfo::GetPhysicalMemoryInfo(uint32_t& TotalPhyMemory , uint32_t& UsedPhyMemory)
{
#ifdef WIN32
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	TotalPhyMemory = memInfo.ullTotalPhys / (1024 * 1024);
	UsedPhyMemory = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024);
#else
    std::ifstream file("/proc/meminfo");
    std::string line;
    uint32_t totalMem = 0;
    uint32_t availMem = 0;
    if(file.is_open())
    {
        while(std::getline(file,line))
        {
            std::istringstream ss(line);
            std::string key,unit;
            std::string value ;
            ss >> key >> value >> unit;
            if(key == "MemTotal:")   totalMem = std::stoi(value);
            if(key == "MemAvailable:")   availMem = std::stoi(value);
        }
    }
    TotalPhyMemory = totalMem / 1024;
    UsedPhyMemory = (totalMem - availMem) / 1024;
#endif
}

void GetSysInfo::GetPhysicalMemoryInfo(uint32_t& CurrpUsedPhyMemory)
{
#ifdef WIN32
	PROCESS_MEMORY_COUNTERS_EX  pmc;
	HANDLE hProcess = GetCurrentProcess();
	if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {	
		CurrpUsedPhyMemory = pmc.WorkingSetSize / (1024 * 1024);
	}
	else {
		std::cerr << "Failed to retrieve process memory info. Error code: " << GetLastError() << std::endl;
	}

	CloseHandle(hProcess);
#else
    std::ifstream file("/proc/self/status");
    std::string line;
    if(file.is_open())
    {
        while(std::getline(file,line))
        {
            std::istringstream ss(line);
            std::string key;
            ss >> key;
            if(key == "VmRSS:")
            {
                ss >> CurrpUsedPhyMemory;
                break;
            }      
        }

    }
#endif
}

uint16_t GetSysInfo::GetThreadCount()
{

    uint16_t threadCount;
#ifdef WIN32
	PDH_FMT_COUNTERVALUE counterVal;
	Sleep(1000);
	PdhCollectQueryData(m_CpuQuery);
	PDH_STATUS status = PdhGetFormattedCounterValue(m_ThreadCount, PDH_FMT_LONG, NULL, &counterVal);
	if (status != ERROR_SUCCESS) {
		std::cerr << "转换 LONG VALUE 格式失败，status is : " << GetLastError()<< std::endl;
		return 0;
	}
	threadCount = counterVal.longValue;
#else
    std::ifstream file("/proc/self/status");
    std::string line;
    if(file.is_open())
    {
        while(std::getline(file,line))
        {
            std::istringstream ss(line);
            std::string key;
            ss >> key;
            if(key == "Threads:")
            {
                ss >> threadCount;
            }
        }

    }
#endif
    return threadCount;
}

void GetSysInfo::GetSystemDiskInfo(std::string& Path, uint32_t& TotalDiskSize, uint32_t& UsedDiskSize)
{
#ifdef WIN32
	char currentRoot[MAX_PATH];
	memcpy(currentRoot,Path.c_str(),Path.size());
	if (PathStripToRoot(currentRoot)) {
	}
	else {
	}
	//获取指定磁盘下的磁盘信息
	ULARGE_INTEGER totalBytes, freeBytes, totalFreeBytes;
	uint32_t freeDiskSpace;
	if (GetDiskFreeSpaceEx(currentRoot, &freeBytes, &totalBytes, &totalFreeBytes)) {
		TotalDiskSize = totalBytes.QuadPart / (1024 * 1024);
		freeDiskSpace = freeBytes.QuadPart / (1024 * 1024);
		UsedDiskSize = TotalDiskSize - freeDiskSpace;
	}
	else {
		std::cerr << "Error getting disk information." << std::endl;
	}
#else
    struct statvfs diskStats;
    if(statvfs(Path.c_str(), &diskStats) != 0)
    {
        //std::cerr << "statvfs error = " << perror("statvfs") << std::endl;
        return;
    }
    uint32_t totalsize = diskStats.f_blocks * diskStats.f_bsize / (1024 * 1024);
    uint32_t freeSize = diskStats.f_bfree *  diskStats.f_bsize / (1024 * 1024); 
    TotalDiskSize = totalsize;
    UsedDiskSize = totalsize - freeSize;
#endif
}