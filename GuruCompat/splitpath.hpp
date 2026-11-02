#if !defined __SPLITPATH__
#define __SPLITPATH__

//////////////////////////////////////////////////////////////////////
// Esta funcion existe en Win32 y no en Linux
//////////////////////////////////////////////////////////////////////
#include <GuruCompat.h>

#ifdef __GURULINUX__

#include <string.h>

// #define mostrar cout << "path:" << path << " temp:" << temp << " drive:" << drive << " dir:" << dir << " fname:" << " ext:" << ext << endl;

template <typename T>
T _strrev(T cadena) {
	int largo = strlen(cadena) - 1;

	for (int i=0; i<(largo/2) + 1;i++) {
		char temp_char = cadena[i];
		cadena[i] = cadena[largo-i];
		cadena[largo-i] = temp_char;
	}

	return cadena;
}


template <typename T>
void _splitpath(T path, char* drive, char* dir, char* fname, char* ext)
{
	if (!path) return ;

	if (strlen(path) == 0) return ;

	std::string temp;

	char *inicio = (char *)path;

	if (drive) *drive = '\0';
	if (dir)   *dir   = '\0';
	if (fname) *fname = '\0';
	if (ext)   *ext   = '\0';

	// voy al final de la cadena
	while (*path++)
		;

	path--; // retrocedo por que estoy parado en el fin de cadena

	path--;
// busco la extension o nombre del directorio
	while (*path != '.' && ((*path != '/') && (*path != '\\')) && path != inicio) {
		temp += *path;
		path--;
	}

	if (*path == '.') { // el archivo tiene extension
		temp += *path;
		if (ext)
			strcpy(ext, _strrev((char*)temp.c_str()));
		temp.clear();

		path--; // saco el punto

		// guardo el nombre
		while (*path != '/' && *path != '\\' && inicio != path)
			temp += *path--;

		if (inicio == path && (*path != '/' && *path != '\\')) temp += *path;

		if (fname)
			strcpy(fname, _strrev((char*)temp.c_str()));
	} else // el archivo NO tiene extension
		if (*path == '/' || *path == '\\') {
			if (fname)
				strcpy(fname, _strrev((char*)temp.c_str()));
		}

	temp.clear();

	// pregunto si solo me vino el nombre de un archivo con su extension
	if (*path == '\0') return ;

// hasta aca ya tengo el nombre del archivo y extension
	while (path != inicio
#if defined __GURUWIN32__
		&& *path != ':'
#endif
		) {
		temp += *path;
		path--;
	}

#if defined __GURUWIN32__
	if (*path == ':') { // si es una unidad
		if (dir)
			strcpy(dir, _strrev((char*)temp.c_str()));

		temp.clear();

		while (path != inicio)
			temp+=*path--;

		temp+=*path;
		if (drive)
			strcpy(drive, _strrev((char*)temp.c_str()));
	} else // puede ser una unidad de red \\Server\Share\Programa.exe
#endif
		{
		temp += *path;
		if (dir)
			strcpy(dir, _strrev((char*)temp.c_str()));
		}
}
#endif
#endif

