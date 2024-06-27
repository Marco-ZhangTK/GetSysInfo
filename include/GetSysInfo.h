#pragma once
#include<iostream>
#include <fstream>
#include <sstream>
#include <thread>
#ifdef WIN32
#include <Windows.h>
#include <pdh.h>
#else
#include <unistd.h>
#include <sys/statvfs.h>
#endif

typedef struct tagAllDISKSTATUS
{
	//_DiskCount;				//磁盘数量
	uint32_t	_Total;					//所有磁盘总容量MB
	uint32_t	_OfFree;				//所有磁盘剩余容量MB
}AllDISKSTATUS, * LPAllDISKSTATUS;

struct cpuinfo{
    std::string name;
    uint32_t user;
    uint32_t nice;
    uint32_t system;
    uint32_t idle;
    uint32_t iowait;
    uint32_t irq;
    uint32_t softirq;
    uint32_t steal;
    uint32_t pid;

};

class GetSysInfo
{
public:
    GetSysInfo(const std::string& name);
    ~GetSysInfo();

    void		SystemCpuInit();			//系统CPU使用率 设置检测开始点
	void		SystemCpuInit(std::string& processName);
	void		SystemCpuUnInit();			//系统CPU使用率 (windows 下初始化) 
    double GetSystemCpuCurrentUsage();    //获取 windows/Linux cpu 使用率
    double GetProcCpuCurrentUsage();      //获取当前进程 windows/Linux cpu 使用率
    double GetTotalPhysicalMemory();      //获取 Windows/Linux 物理内存使用率
    void GetPhysicalMemoryInfo(uint32_t& TotalPhyMemory , uint32_t& UsedPhyMemory); //获取 Windows /Linux 物理内存 使用 内存 (M) 
    void GetPhysicalMemoryInfo(uint32_t& CurrpUsedPhyMemory);   //获取 Windows/Linux 当前进程 使用的物理内存
    void GetSystemDiskInfo(std::string& Path, uint32_t& TotalDiskSize, uint32_t& UsedDiskSize); // 获取 Windows/Linux 硬盘信息

    uint16_t GetThreadCount();  //获取 Windows /Linux 下当前进程使用线程数
private:
#ifdef WIN32
	PDH_HQUERY		m_CpuQuery;
	PDH_HCOUNTER	m_CpuTotal;
	PDH_HCOUNTER	m_ThreadCount;
#endif
    uint32_t get_pid();    //获取当前进程 pid
    inline double calc_cpuoccupy(cpuinfo& cpuoccupyf,cpuinfo& cpuoccupys); //Linux 下 cpu 占用率计算
    inline void get_cpuoccupy(cpuinfo& cpuoccupy);    //Linux 下 获取cpu 占用时间
    inline void get_cpuoccupy(cpuinfo& cpuoccupy,uint32_t& pid);//Linux 下 获取当前进程cpu 占用时间
   // struct cpuinfo cpuoccupy;
    std::string pidName;


};