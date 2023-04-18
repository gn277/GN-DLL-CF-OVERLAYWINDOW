#include "Exception.h"

extern "C" LONG(__stdcall * ZwQueryInformationThread)(
	IN HANDLE ThreadHandle,
	IN THREADINFOCLASS ThreadInformationClass,
	OUT PVOID ThreadInformation,
	IN ULONG ThreadInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	) = NULL;


GN_Exception::GN_Exception()
{

}

GN_Exception::~GN_Exception()
{

}

void GN_Exception::SetVectoredExceptionHandler(PVECTORED_EXCEPTION_HANDLER vectored_handler_pointer)
{
	//添加VEH  参数1=1表示插入Veh链的头部，=0表示插入到VEH链的尾部
	AddVectoredExceptionHandler(1, vectored_handler_pointer);
}

LPTOP_LEVEL_EXCEPTION_FILTER GN_Exception::SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	return ::SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);
}

ULONG GN_Exception::UnSetVectoredExceptionHandler(PVECTORED_EXCEPTION_HANDLER vectored_handler_pointer)
{
	return RemoveVectoredExceptionHandler(vectored_handler_pointer);
}

int GN_Exception::SetHardWareBreakPoint(const wchar_t* main_modulename, DWORD64 dr7_statu, __int64 br1, __int64 br2, __int64 br3, __int64 br4)
{
	//printf("主模块名：%S\n", main_modulename);
	//printf("br1:%p\n", br1);
	//printf("br2:%p\n", br2);
	//printf("br3:%p\n", br3);
	//printf("br4:%p\n", br4);
	this->mdr1 = br1;
	this->mdr2 = br2;
	this->mdr3 = br3;
	this->mdr4 = br4;
	//遍历线程 通过openthread获取到线程环境后设置硬件断点
	HANDLE hTool32 = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hTool32 != INVALID_HANDLE_VALUE)
	{
		//线程环境结构体
		THREADENTRY32 thread_entry32;
		thread_entry32.dwSize = sizeof(THREADENTRY32);
		HANDLE h_hook_thread = NULL;
		//模块信息
		MODULEINFO module_info = { 0 };
		HANDLE target_modulehandle = GetModuleHandleW(main_modulename);
		if (target_modulehandle != 0)
		{
			//获取模块结束地址
			GetModuleInformation(GetCurrentProcess(), (HMODULE)target_modulehandle, &module_info, sizeof(MODULEINFO));
			__int64 target_modulehandle_endaddress = ((__int64)module_info.lpBaseOfDll + module_info.SizeOfImage);
			//遍历线程
			if (Thread32First(hTool32, &thread_entry32))
			{
				do
				{
					//如果线程父进程ID为当前进程ID
					if (thread_entry32.th32OwnerProcessID == GetCurrentProcessId())
					{
						h_hook_thread = OpenThread(THREAD_SET_CONTEXT | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, thread_entry32.th32ThreadID);
						// 从 ntdll.dll 中取出 ZwQueryInformationThread
						(FARPROC&)ZwQueryInformationThread = ::GetProcAddress(GetModuleHandle(L"ntdll"), "ZwQueryInformationThread");
						// 获取线程入口地址
						PVOID startaddr;//用来接收线程入口地址
						ZwQueryInformationThread(h_hook_thread, ThreadQuerySetWin32StartAddress, &startaddr, sizeof(startaddr), NULL);
						if (((__int64)startaddr >= (__int64)target_modulehandle) && ((__int64)startaddr <= target_modulehandle_endaddress))
						{
							OutputDebugStringA_1Param("[GN]:veh->线程起始地址：%p", startaddr);
							//暂停线程
							SuspendThread(h_hook_thread);
							//设置硬件断点
							CONTEXT thread_context = { CONTEXT_DEBUG_REGISTERS };
							thread_context.ContextFlags = CONTEXT_ALL;
							//得到指定线程的环境（上下文）
							if (!GetThreadContext(h_hook_thread, &thread_context))
							{
								OutputDebugStringA("[GN]:veh->获得线程上下文失败!");
								return 3;
							}
							thread_context.Dr0 = br1;
							thread_context.Dr1 = br2;
							thread_context.Dr2 = br3;
							thread_context.Dr3 = br4;
							thread_context.Dr7 = dr7_statu;
							if (!SetThreadContext(h_hook_thread, &thread_context))
							{
								OutputDebugStringA("[GN]:veh->设置线程上下文失败!");
								return 4;
							}
							//恢复线程
							ResumeThread(h_hook_thread);
						}
						CloseHandle(h_hook_thread);
					}
				} while (Thread32Next(hTool32, &thread_entry32));
			}
			CloseHandle(hTool32);
			OutputDebugStringA("[GN]:veh->it's over!");
			return true;
		}
		else
			return 2;//模块句柄获取失败
	}
	return 0;
}


