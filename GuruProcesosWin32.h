#if !defined __GURUPROCESOSWIN32_H__
#define __GURUPROCESOSWIN32_H__

//////////////////////////////////////////////////////////////////////
// Link con Psapi.lib
//////////////////////////////////////////////////////////////////////
#include <GuruCompat.h>
#include <windows.h>
#include <Psapi.h>

#include <vector>

using namespace std;

class data_proceso
{
	friend class GuruProcesosWin32;

	string		nombre_archivo;
	uint32_t	id_proceso;
	uint32_t	uso_pagefile;
	uint32_t	maximo_uso_pagefile;
	uint32_t	area_trabajo;
	uint32_t	maximo_area_trabajo;

public:
	string		sacar_nombre_archivo() {return nombre_archivo;};
	uint32_t	sacar_id_proceso() {return id_proceso;};
	uint32_t	sacar_uso_pagefile() {return uso_pagefile;};
	uint32_t	sacar_maximo_uso_pagefile() {return maximo_uso_pagefile;};
	uint32_t	sacar_area_trabajo() {return area_trabajo;};
	uint32_t	sacar_maximo_area_trabajo() {return maximo_area_trabajo;};
};

class GuruProcesosWin32
{
public:
	GuruProcesosWin32() {};
	virtual ~GuruProcesosWin32() {};

	vector<data_proceso> procesos();
	bool	sacar_info_proceso(uint32_t id_proceso, data_proceso& info);
};

#endif
