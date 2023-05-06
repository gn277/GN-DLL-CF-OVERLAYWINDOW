#include "Exception.h"

GN_Exception* gn_exception = new GN_Exception();


typedef struct _PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION
{
	ULONG Version;
	ULONG Reserved;
	PVOID Callback;
} PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION, * PPROCESS_INSTRUMENTATION_CALLBACK_INFORMATION;

extern "C" LONG(__stdcall * ZwQueryInformationThread)(IN HANDLE ThreadHandle, IN THREADINFOCLASS ThreadInformationClass, OUT PVOID ThreadInformation, IN ULONG ThreadInformationLength, OUT PULONG ReturnLength OPTIONAL) = NULL;
extern "C" NTSTATUS NTAPI NtSetInformationProcess(HANDLE ProcessHandle, ULONG ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength);
extern "C" NTSTATUS NtContinue(PCONTEXT, unsigned long long);
extern "C" void MyCallbackEntry();
extern "C" void MyCallbackRoutine(CONTEXT * context);


GN_Exception::GN_Exception()
{

}

GN_Exception::~GN_Exception()
{

}

void GN_Exception::SetVectoredExceptionHandler(bool potision, PVECTORED_EXCEPTION_HANDLER vectored_handler_pointer)
{
	//���VEH  ����1=1��ʾ����Veh����ͷ����=0��ʾ���뵽VEH����β��
	AddVectoredExceptionHandler(potision, vectored_handler_pointer);
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
						ZwQueryInformationThread(h_hook_thread, (THREADINFOCLASS)ThreadQuerySetWin32StartAddress, &startaddr, sizeof(startaddr), NULL);
						if (((__int64)startaddr >= (__int64)target_modulehandle) && ((__int64)startaddr <= target_modulehandle_endaddress))
						{
							//OutputDebugStringA_1Param("[GN]:veh->�߳���ʼ��ַ��%p", startaddr);
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


LONG WINAPI ReturnExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
	////OutputDebugStringA("[GN]:ReturnExceptionHandler() doing...");
	////hardware breakpoint
	//if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
	//{
	//	if (ExceptionInfo->ExceptionRecord->ExceptionAddress == (PVOID64)gn_exception->mdr1)
	//		return EXCEPTION_CONTINUE_EXECUTION;
	//	else if (ExceptionInfo->ExceptionRecord->ExceptionAddress == (PVOID64)gn_exception->mdr2)
	//		return EXCEPTION_CONTINUE_EXECUTION;
	//	else if (ExceptionInfo->ExceptionRecord->ExceptionAddress == (PVOID64)gn_exception->mdr3)
	//		return EXCEPTION_CONTINUE_EXECUTION;
	//	else if (ExceptionInfo->ExceptionRecord->ExceptionAddress == (PVOID64)gn_exception->mdr4)
	//		return EXCEPTION_CONTINUE_EXECUTION;
	//	else
	//		return EXCEPTION_CONTINUE_SEARCH;
	//}
	//else
	//	return EXCEPTION_CONTINUE_SEARCH;

	//if (ExceptionInfo->ExceptionRecord->ExceptionAddress == (PVOID64)gn_exception->mdr1)
	//	return EXCEPTION_CONTINUE_EXECUTION;
	//else if (ExceptionInfo->ExceptionRecord->ExceptionAddress == (PVOID64)gn_exception->mdr2)
	//	return EXCEPTION_CONTINUE_EXECUTION;
	//else if (ExceptionInfo->ExceptionRecord->ExceptionAddress == (PVOID64)gn_exception->mdr3)
	//	return EXCEPTION_CONTINUE_EXECUTION;
	//else if (ExceptionInfo->ExceptionRecord->ExceptionAddress == (PVOID64)gn_exception->mdr4)
	//	return EXCEPTION_CONTINUE_EXECUTION;
	//else
	//	return EXCEPTION_CONTINUE_SEARCH;

	OutputDebugStringA("[GN]:������ֵ...");
	return EXCEPTION_CONTINUE_EXECUTION;
}

bool GN_Exception::InstallException(ExceptionHandlerApi exception_handler_api)
{
	if (exception_handler_api == NULL)
	{
		OutputDebugStringA("[GN]:exception_handler_api is nullptr");
		return false;
	}
	else
		this->pExceptionHandlerApi = exception_handler_api;

	////�����쳣����ר�Ŵ�����ֵ
	//this->SetUnhandledExceptionFilter(ReturnExceptionHandler);
	this->SetVectoredExceptionHandler(1, ReturnExceptionHandler);

	if (!this->InitSymbol())
	{
		OutputDebugStringA("[GN]:InitSymbol() error!");
		return false;
	}

	this->tls_index = TlsAlloc();

	PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION info;
	info.Version = 0;
	info.Reserved = 0;
	info.Callback = MyCallbackEntry;
	ULONG status = NtSetInformationProcess(GetCurrentProcess(), 0x28, &info, sizeof(info));
	if (status)
	{
		OutputDebugStringA_1Param("[GN]:NtSetInformationProcess���� errorcode:%p", status);
		return false;
	}

	return true;
}

bool GN_Exception::InitSymbol()
{
	SymSetOptions(SYMOPT_UNDNAME);
	return SymInitialize(GetCurrentProcess(), NULL, TRUE);
}

PThreadData GN_Exception::GetThreadDataBuffer()
{
	void* thread_data = TlsGetValue(tls_index);
	if (!thread_data)
	{
		thread_data = LocalAlloc(LPTR, sizeof(ThreadData));
		if (!thread_data)
		{
			OutputDebugStringA("[GN]:LocalAlloc() error");
			//__debugbreak();
		}

		memset(thread_data, 0, sizeof(ThreadData));

		if (!TlsSetValue(tls_index, thread_data))
		{
			OutputDebugStringA("[GN]:TlsSetValue() error");
			//__debugbreak();
		}
	}

	return (PThreadData)thread_data;
}

void MyCallbackRoutine(CONTEXT* context)
{
	context->Rip = __readgsqword(0x02D8);//syscall �ķ��ص�ַ
	context->Rsp = __readgsqword(0x02E0);//context = rsp, ExceptionRecord = rsp + 0x4F0
	context->Rcx = context->R10;

	PThreadData current_thread_data = gn_exception->GetThreadDataBuffer();
	if (current_thread_data->IsThreadHandlingSyscall)
		NtContinue(context, 0);
	current_thread_data->IsThreadHandlingSyscall = true;

	//�������õĺ�����
	CHAR buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = { 0 };
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MAX_SYM_NAME;
	DWORD64 Displacement;
	BOOLEAN result = SymFromAddr(GetCurrentProcess(), context->Rip, &Displacement, pSymbol);
	if (result)
	{
		//if (_stricmp(pSymbol->Name, "ZwRaiseException") == 0)
		if (_stricmp(pSymbol->Name, "KiUserExceptionDispatcher") == 0)
		{
			//OutputDebugStringA_2Param("[GN]:Function: %s Address: %p", pSymbol->Name, context->Rip);
			LONG status = gn_exception->pExceptionHandlerApi((PEXCEPTION_RECORD)(context->Rsp + 0x4F0), (PCONTEXT)context->Rsp);
			if (status == EXCEPTION_CONTINUE_EXECUTION)
			{
				//OutputDebugStringA("[GN]:RaxΪ0");
				//context->Rax = 0;
				RtlRestoreContext((PCONTEXT)context->Rsp, 0);
			}
		}
	}

	current_thread_data->IsThreadHandlingSyscall = false;
	NtContinue(context, 0);
}



