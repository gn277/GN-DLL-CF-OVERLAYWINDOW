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
	//���VEH  ����1=1��ʾ����Veh����ͷ����=0��ʾ���뵽VEH����β��
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
	//printf("��ģ������%S\n", main_modulename);
	//printf("br1:%p\n", br1);
	//printf("br2:%p\n", br2);
	//printf("br3:%p\n", br3);
	//printf("br4:%p\n", br4);
	this->mdr1 = br1;
	this->mdr2 = br2;
	this->mdr3 = br3;
	this->mdr4 = br4;
	//�����߳� ͨ��openthread��ȡ���̻߳���������Ӳ���ϵ�
	HANDLE hTool32 = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hTool32 != INVALID_HANDLE_VALUE)
	{
		//�̻߳����ṹ��
		THREADENTRY32 thread_entry32;
		thread_entry32.dwSize = sizeof(THREADENTRY32);
		HANDLE h_hook_thread = NULL;
		//ģ����Ϣ
		MODULEINFO module_info = { 0 };
		HANDLE target_modulehandle = GetModuleHandleW(main_modulename);
		if (target_modulehandle != 0)
		{
			//��ȡģ�������ַ
			GetModuleInformation(GetCurrentProcess(), (HMODULE)target_modulehandle, &module_info, sizeof(MODULEINFO));
			__int64 target_modulehandle_endaddress = ((__int64)module_info.lpBaseOfDll + module_info.SizeOfImage);
			//�����߳�
			if (Thread32First(hTool32, &thread_entry32))
			{
				do
				{
					//����̸߳�����IDΪ��ǰ����ID
					if (thread_entry32.th32OwnerProcessID == GetCurrentProcessId())
					{
						h_hook_thread = OpenThread(THREAD_SET_CONTEXT | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, thread_entry32.th32ThreadID);
						// �� ntdll.dll ��ȡ�� ZwQueryInformationThread
						(FARPROC&)ZwQueryInformationThread = ::GetProcAddress(GetModuleHandle(L"ntdll"), "ZwQueryInformationThread");
						// ��ȡ�߳���ڵ�ַ
						PVOID startaddr;//���������߳���ڵ�ַ
						ZwQueryInformationThread(h_hook_thread, ThreadQuerySetWin32StartAddress, &startaddr, sizeof(startaddr), NULL);
						if (((__int64)startaddr >= (__int64)target_modulehandle) && ((__int64)startaddr <= target_modulehandle_endaddress))
						{
							OutputDebugStringA_1Param("[GN]:veh->�߳���ʼ��ַ��%p", startaddr);
							//��ͣ�߳�
							SuspendThread(h_hook_thread);
							//����Ӳ���ϵ�
							CONTEXT thread_context = { CONTEXT_DEBUG_REGISTERS };
							thread_context.ContextFlags = CONTEXT_ALL;
							//�õ�ָ���̵߳Ļ����������ģ�
							if (!GetThreadContext(h_hook_thread, &thread_context))
							{
								OutputDebugStringA("[GN]:veh->����߳�������ʧ��!");
								return 3;
							}
							thread_context.Dr0 = br1;
							thread_context.Dr1 = br2;
							thread_context.Dr2 = br3;
							thread_context.Dr3 = br4;
							thread_context.Dr7 = dr7_statu;
							if (!SetThreadContext(h_hook_thread, &thread_context))
							{
								OutputDebugStringA("[GN]:veh->�����߳�������ʧ��!");
								return 4;
							}
							//�ָ��߳�
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
			return 2;//ģ������ȡʧ��
	}
	return 0;
}


