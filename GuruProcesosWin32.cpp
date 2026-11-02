#include "GuruProcesosWin32.h"

vector<data_proceso> GuruProcesosWin32::procesos()
{
	DWORD		idprocesos[3000];
	DWORD		necesarios = 0;
	uint32_t	total_procesos = 0;

	BOOL salida = EnumProcesses(idprocesos, 3000, &necesarios);	

	total_procesos = necesarios / sizeof(uint32_t);

	vector<data_proceso> temp_procesos;

	for (uint32_t i=0;i<total_procesos;i++)
		{
		data_proceso temp;
		if (sacar_info_proceso(idprocesos[i], temp))
			temp_procesos.push_back(temp);
		}

	return vector<data_proceso>(temp_procesos);
}

bool GuruProcesosWin32::sacar_info_proceso(uint32_t id_proceso, data_proceso& info)
{
	HANDLE hproceso = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, id_proceso);

	if (hproceso == NULL) return false;
	
	PROCESS_MEMORY_COUNTERS contadores;

	if (GetProcessMemoryInfo(hproceso, &contadores, sizeof(contadores)) == 0) return false;

	char	nombre_archivo[MAX_PATH];
	DWORD	tamanio=MAX_PATH;

	if (GetModuleBaseName(hproceso, NULL, nombre_archivo, tamanio) == 0) return false;

	info.nombre_archivo = nombre_archivo;
	info.id_proceso = id_proceso;
	info.area_trabajo = (uint32_t)contadores.WorkingSetSize;
	info.maximo_area_trabajo = (uint32_t)contadores.PeakWorkingSetSize;
	info.uso_pagefile = (uint32_t)contadores.PagefileUsage;
	info.maximo_uso_pagefile = (uint32_t)contadores.PeakPagefileUsage;

	return true;
}
