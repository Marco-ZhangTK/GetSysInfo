#include <iostream>
#include "GetSysInfo.h"

int main()
{
    std::string name("");
    GetSysInfo currInfo(name);
    double cpu_currUsage;
    double mem_currUsage;
    double cpu_proUsage;
    uint32_t totalPhyMem;
    uint32_t usedPhyMem;
    cpu_currUsage = currInfo.GetSystemCpuCurrentUsage();
    std::cout << "cpu_currUsage = " << cpu_currUsage << std::endl;
    cpu_proUsage = currInfo.GetProcCpuCurrentUsage();
    std::cout << "cpu_proUsage = " << cpu_proUsage << std::endl;
    currInfo.GetPhysicalMemoryInfo(totalPhyMem,usedPhyMem);
    std::cout << "total mem = " << totalPhyMem << "  used mem =" << usedPhyMem << std::endl;
    mem_currUsage = currInfo.GetTotalPhysicalMemory();
    std::cout << "mem_currUsage = " << mem_currUsage << std::endl;
    uint32_t procUseMem;
    currInfo.GetPhysicalMemoryInfo(procUseMem);
    std::cout << "procUseMem = " << procUseMem  << std::endl;
    uint16_t threadCount = currInfo.GetThreadCount();
    std::cout << "thread count = " << threadCount << std::endl;
    uint32_t totalDiskSize,usedDiskSize;
    std::string path("/");
    currInfo.GetSystemDiskInfo(path,totalDiskSize,usedDiskSize);
    std::cout << "totalDiskSize = " << totalDiskSize << "  usedDiskSize = " << usedDiskSize << std::endl;
    return 0;
}