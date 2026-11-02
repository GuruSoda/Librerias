#ifndef __GURUPROCESOSNT_HPP__
#define __GURUPROCESOSNT_HPP__

// Extraido de http://jgrunzweig.github.io/posts/2014/12/unique-technique-for-iterating-through-processes/
// Thanks to http://pages.cs.wisc.edu/~stjones/proj/zwenum.c
// Thanks to http://www.rohitab.com/discuss/topic/40626-list-processes-using-ntquerysysteminformation/

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <windows.h>

#include <iostream>
#include <list>

#include <GuruAlgo.hpp>

namespace GuruProcesosNT {

	#define NT_SUCCESS(Status)					((NTSTATUS)(Status) >= 0)
	#define STATUS_INFO_LENGTH_MISMATCH			((NTSTATUS)0xC0000004L)
	#define SystemProcessAndThreadInformation	5

	using namespace std;

	typedef LONG KPRIORITY;

	typedef NTSTATUS(WINAPI *tNTQuerySystemInformation)(
		ULONG SystemInformationClass,
		PVOID SystemInformation,
		ULONG SystemInformationLength,
		PULONG ReturnLength
	);

	typedef struct _CLIENT_ID {
		DWORD          UniqueProcess;
		DWORD          UniqueThread;
	} CLIENT_ID;

	typedef struct _SYSTEM_THREADS {
		LARGE_INTEGER  KernelTime;
		LARGE_INTEGER  UserTime;
		LARGE_INTEGER  CreateTime;
		ULONG          WaitTime;
		PVOID          StartAddress;
		CLIENT_ID      ClientId;
		KPRIORITY      Priority;
		KPRIORITY      BasePriority;
		ULONG          ContextSwitchCount;
		LONG           State;
		LONG           WaitReason;
	} SYSTEM_THREADS, *PSYSTEM_THREADS;

	typedef struct _UNICODE_STRING {
		USHORT         Length;
		USHORT         MaximumLength;
		PWSTR          Buffer;
	} UNICODE_STRING;

	typedef struct _VM_COUNTERS {
		SIZE_T             PeakVirtualSize;
		SIZE_T             VirtualSize;
		ULONG              PageFaultCount;
		SIZE_T             PeakWorkingSetSize;
		SIZE_T             WorkingSetSize;
		SIZE_T             QuotaPeakPagedPoolUsage;
		SIZE_T             QuotaPagedPoolUsage;
		SIZE_T             QuotaPeakNonPagedPoolUsage;
		SIZE_T             QuotaNonPagedPoolUsage;
		SIZE_T             PagefileUsage;
		SIZE_T             PeakPagefileUsage;
	} VM_COUNTERS;

	typedef struct _SYSTEM_PROCESSES {
		ULONG              NextEntryDelta;
		ULONG              ThreadCount;
		ULONG              Reserved1[6];
		LARGE_INTEGER      CreateTime;
		LARGE_INTEGER      UserTime;
		LARGE_INTEGER      KernelTime;
		UNICODE_STRING     ProcessName;
		KPRIORITY          BasePriority;
		ULONG              ProcessId;
		ULONG              InheritedFromProcessId;
		ULONG              HandleCount;
		ULONG              Reserved2[2];
		VM_COUNTERS        VmCounters;
		IO_COUNTERS        IoCounters;
		SYSTEM_THREADS     Threads[1];
	} SYSTEM_PROCESSES, *PSYSTEM_PROCESSES;

	struct proceso {
		uint64_t	id;
		string		nombre;
		time_t		creado;
		uint64_t	identificadores; // handles
		uint64_t	subProcesos;	// threads

		uint64_t	espacioTrabajo;			// WorkingSetSize - memoria usada
		uint64_t	maximoEspacioTrabajo;	// PeakWorkingSetSize - maixma memoria usada

		time_t		tiempoCPU;
	};

	list<GuruProcesosNT::proceso>& procesos (list<GuruProcesosNT::proceso>& procs) {

		procs.clear();

		ULONG cbBuffer = 0;
		PVOID pBuffer = NULL;
		NTSTATUS Status = 0xc0000004;

		tNTQuerySystemInformation ntQSI = (tNTQuerySystemInformation)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation");

		if (ntQSI == NULL) return procs;

		if(ntQSI(SystemProcessAndThreadInformation, 0, 0, &cbBuffer) == 0xc0000004) {
			if(cbBuffer == 0) cbBuffer = 131072;

			if(cbBuffer) {
				pBuffer = VirtualAlloc(0, cbBuffer, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				if(pBuffer) {
					if(!ntQSI(SystemProcessAndThreadInformation, pBuffer, cbBuffer, &cbBuffer) && cbBuffer) {

						PSYSTEM_PROCESSES infoP = NULL;
						infoP = (PSYSTEM_PROCESSES)pBuffer;

						while (infoP) {
							GuruProcesosNT::proceso un_proceso;

							char pName[512];
							memset(pName, 0, sizeof(pName));
							WideCharToMultiByte(CP_ACP, 0, infoP->ProcessName.Buffer, infoP->ProcessName.Length, pName, sizeof(pName), NULL, NULL);

							un_proceso.nombre = pName;
							un_proceso.id = infoP->ProcessId;
							// lo convierto a time_t, LARGE_INTEGER a time_t = infoP->CreateTime.QuadPart / 10000000ULL - 11644473600ULL
							un_proceso.creado = infoP->CreateTime.QuadPart / 10000000ULL - 11644473600ULL;
							un_proceso.identificadores = infoP->HandleCount;
							un_proceso.subProcesos = infoP->ThreadCount;
							un_proceso.espacioTrabajo = infoP->VmCounters.WorkingSetSize;
							un_proceso.maximoEspacioTrabajo = infoP->VmCounters.PeakWorkingSetSize;
							un_proceso.tiempoCPU = (infoP->KernelTime.QuadPart + infoP->UserTime.QuadPart) / 10000000;

							procs.push_back(un_proceso);

							if(!infoP->NextEntryDelta) break;

							infoP = (PSYSTEM_PROCESSES)(((LPBYTE)infoP) + infoP->NextEntryDelta);
						}
					}
					VirtualFree(pBuffer, cbBuffer, MEM_RELEASE);
				}
			}
		}
		return procs;
	}

}

#endif
