#pragma once
#include <Windows.h>
#include <stdio.h>
#include <psapi.h>
#include <tlhelp32.h>


#define OutputDebugStringA_1Param(fmt,var) {CHAR sOut[256];CHAR sfmt[50];sprintf_s(sfmt,"%s%s","",fmt);sprintf_s(sOut,(sfmt),var);OutputDebugStringA(sOut);}
#define OutputDebugStringA_2Param(fmt,var1,var2) {CHAR sOut[256];CHAR sfmt[50];sprintf_s(sfmt,"%s%s","",fmt);sprintf_s(sOut,(sfmt),var1,var2);OutputDebugStringA(sOut);}
#define OutputDebugStringA_3Param(fmt,var1,var2,var3) {CHAR sOut[256];CHAR sfmt[50];sprintf_s(sfmt,"%s%s","",fmt);sprintf_s(sOut,(sfmt),var1,var2,var3);OutputDebugStringA(sOut);}

typedef enum _THREADINFOCLASS {
    ThreadBasicInformation,
    ThreadTimes,
    ThreadPriority,
    ThreadBasePriority,
    ThreadAffinityMask,
    ThreadImpersonationToken,
    ThreadDescriptorTableEntry,
    ThreadEnableAlignmentFaultFixup,
    ThreadEventPair_Reusable,
    ThreadQuerySetWin32StartAddress,
    ThreadZeroTlsCell,
    ThreadPerformanceCount,
    ThreadAmILastThread,
    ThreadIdealProcessor,
    ThreadPriorityBoost,
    ThreadSetTlsArrayAddress,
    ThreadIsIoPending,
    ThreadHideFromDebugger,
    ThreadBreakOnTermination,
    MaxThreadInfoClass
}THREADINFOCLASS;
typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
}CLIENT_ID;
typedef struct _THREAD_BASIC_INFORMATION {
    LONG ExitStatus;
    PVOID TebBaseAddress;
    CLIENT_ID ClientId;
    LONG AffinityMask;
    LONG Priority;
    LONG BasePriority;
}THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;

class GN_Exception
{
private:

public:
    __int64 mdr1 = 0;//断点地址1
    __int64 mdr2 = 0;//断点地址2
    __int64 mdr3 = 0;//断点地址3
    __int64 mdr4 = 0;//断点地址4
    //软件断点
    __int64 software_breakpoint1 = 0;
    __int64 software_breakpoint2 = 0;
    __int64 software_breakpoint3 = 0;
    __int64 software_breakpoint4 = 0;
    __int64 software_breakpoint5 = 0;
    __int64 software_breakpoint6 = 0;
    __int64 software_breakpoint7 = 0;
    __int64 software_breakpoint8 = 0;
    __int64 software_breakpoint9 = 0;
    __int64 software_breakpoint10 = 0;
    __int64 software_breakpoint11 = 0;
    __int64 software_breakpoint12 = 0;
    __int64 software_breakpoint13 = 0;
    __int64 software_breakpoint14 = 0;
    __int64 software_breakpoint15 = 0;
    __int64 software_breakpoint16 = 0;
    __int64 software_breakpoint17 = 0;
    __int64 software_breakpoint18 = 0;
    __int64 software_breakpoint19 = 0;
    __int64 software_breakpoint20 = 0;
    __int64 software_breakpoint21 = 0;
    __int64 software_breakpoint22 = 0;
    __int64 software_breakpoint23 = 0;
    __int64 software_breakpoint24 = 0;
    __int64 software_breakpoint25 = 0;

public:
    GN_Exception();
    ~GN_Exception();
    //设置异常处理函数
    void SetVectoredExceptionHandler(PVECTORED_EXCEPTION_HANDLER vectored_handler_pointer);
    //设置结构异常处理函数
    LPTOP_LEVEL_EXCEPTION_FILTER SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);
    //卸载异常处理函数
    ULONG UnSetVectoredExceptionHandler(PVECTORED_EXCEPTION_HANDLER vectored_handler_pointer);
    //0 创建快照失败，1 成功，2 模块句柄获取失败，3 获得线程上下文失败，4 设置线程上下文失败
    int SetHardWareBreakPoint(const wchar_t* main_modulename, DWORD64 dr7_statu, __int64 br1, __int64 br2, __int64 br3, __int64 br4);

};

//// <<intel-325462-sdm>> page 3414
//// Debug control register (DR7)
//// Specifies the forms of memory or I / O access that cause breakpoints to be generated.
//struct xx_dr7
//{
//    uint32_t L0 : 1;
//    uint32_t G0 : 1;
//    uint32_t L1 : 1;
//    uint32_t G1 : 1;
//    uint32_t L2 : 1;
//    uint32_t G2 : 1;
//    uint32_t L3 : 1;
//    uint32_t G3 : 1;
//
//    uint32_t LE : 1;
//    uint32_t GE : 1;
//    uint32_t no_use1 : 1;
//    uint32_t RTM : 1;
//    uint32_t no_use2 : 1;
//    uint32_t GD : 1;
//    uint32_t no_use3 : 2;
//
//    uint32_t RW0 : 2;
//    uint32_t LEN0 : 2;
//    uint32_t RW1 : 2;
//    uint32_t LEN1 : 2;
//    uint32_t RW2 : 2;
//    uint32_t LEN2 : 2;
//    uint32_t RW3 : 2;
//    uint32_t LEN3 : 2;
//};
//
//#define RW_EXE    0b00
//#define RW_WRITE  0b01
//#define RW_RW    0b11
//#define LEN_1B    0b00
//#define LEN_2B    0b01
//#define LEN_4B    0b11
//static bool xx_set_hw_bp(HANDLE thread, int idx, void* addr, int RW = RW_EXE, int LEN = LEN_4B)
//{
//    if (RW == RW_EXE) 
//    {    //If the corresponding RWn field in register DR7 is 00 (instruction execution), then the LENn field should also be 00. 
//        // The effect of using other lengths is undefined.See Section 17.2.5, “Breakpoint Field Recognition, ” below.
//        LEN = 0b00;
//    }  // get context
//    CONTEXT context = { 0 };
//    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
//    BOOL get_ret = GetThreadContext(thread, &context);  
//    if (FALSE == get_ret) 
//    {
//        return false;
//    }// set dr7
//    xx_dr7 dr7{ 0 };  if (0 == idx) {
//        memcpy(&context.Dr0, &addr, sizeof(addr));
//        dr7.L0 = 1;
//        dr7.G0 = 1;
//        dr7.RW0 = RW;
//        dr7.LEN0 = LEN;
//    }
//    else if (1 == idx) {
//        memcpy(&context.Dr1, &addr, sizeof(addr));
//        dr7.L1 = 1;
//        dr7.G1 = 1;
//        dr7.RW1 = RW;
//        dr7.LEN1 = LEN;
//    }
//    else if (2 == idx) {
//        memcpy(&context.Dr2, &addr, sizeof(addr));
//        dr7.L2 = 1;
//        dr7.G2 = 1;
//        dr7.RW2 = RW;
//        dr7.LEN2 = LEN;
//    }
//    else if (3 == idx) {
//        memcpy(&context.Dr3, &addr, sizeof(addr));
//        dr7.L3 = 1;
//        dr7.G3 = 1;
//        dr7.RW3 = RW;
//        dr7.LEN3 = LEN;
//    }  // set context
//    context.Dr7 = dr7.get();
//    BOOL set_ret = SetThreadContext(thread, &context);  return TRUE == set_ret;
//}
