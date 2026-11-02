#include "GuruUnzip.h"

GuruUnzip::~GuruUnzip(void)
{
	cerrar();
}

void GuruUnzip::extraerTodo(const string& dir_destino)
{
	if (!GuruDirectorio::propiedades_archivo(dir_destino).Existe())
		GuruDirectorio::crear_directorio(dir_destino);

	try{
		for (GuruUnzip::iterator it=begin();it!=end();++it)
			it.extraer(dir_destino);
	}catch (string& e){cout << e << endl;};
}

void GuruUnzip::extraer(const string& archivo, const string& dir_destino)
{
	abrir();

	if (unzLocateFile (archivo_zip, archivo.c_str(), 0) == UNZ_END_OF_LIST_OF_FILE) {
		cerrar();
		throw GuruError().cargar_descripcion("the file is not found.");
	}

	extraer_actual_archivo(dir_destino);

	cerrar();
}

GuruBuffer& GuruUnzip::extraer(const string& archivo, GuruBuffer& buffer)
{
	try{
		abrir();

		if (unzLocateFile (archivo_zip, archivo.c_str(), 2) != UNZ_OK) throw string("Error en LocateFile");

		if (unzOpenCurrentFile(archivo_zip) != UNZ_OK) throw string("Error en OpenCurrentFile");

		unz_file_info info;

		if (unzGetCurrentFileInfo(archivo_zip, &info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK) throw string("Error en GetCurrentFileInfo");

		buffer.liberar();

		buffer.reservar(info.uncompressed_size+1);

		if (unzReadCurrentFile(archivo_zip, buffer.buf(), info.uncompressed_size) < 0) throw string("Error en leyendo el contendio del archivo compactado.");

		if (unzCloseCurrentFile(archivo_zip) != UNZ_OK) throw string("Error en unzCloseCurrentFile");

		cerrar();

	}catch (const string& e) {
		cerr << e << endl;
		buffer.liberar();
	}

	return buffer;
}

void GuruUnzip::extraer_actual_archivo(const string& directorio)
{
	if ((codigo_api=unzOpenCurrentFile(archivo_zip)) != UNZ_OK) throw GuruError().cargar_descripcion("Error abriendo el archivo compactado.");

	if ((codigo_api=unzGetCurrentFileInfo(archivo_zip, &info_actual_archivo, nombre_archivo_actual, MAX_PATH, NULL, 0, NULL, 0)) != UNZ_OK) throw GuruError().cargar_descripcion("Error retornando informacion del archivo compactado.");

	GuruBuffer buffer_archivo;

	buffer_archivo.reservar(info_actual_archivo.uncompressed_size+1);

	if ((codigo_api=unzReadCurrentFile(archivo_zip, buffer_archivo.buf(), info_actual_archivo.uncompressed_size)) < 0) throw GuruError().cargar_descripcion("Error leyendo del archivo compactado.");

	if ((codigo_api=unzCloseCurrentFile(archivo_zip)) != UNZ_OK) throw GuruError().cargar_descripcion("Error cerrando el archivo compactado.");

	string ruta_completa;
	ruta_completa = directorio + nombre_archivo_actual;

	if (!GuruDirectorio::propiedades_archivo(ruta_completa).Existe())
		GuruDirectorio::crear_directorio(ruta_completa);

	ofstream archivo_nuevo(ruta_completa.c_str(), ios::trunc | ios::binary);

	if (!archivo_nuevo) throw GuruError().descripcion();

	archivo_nuevo.write(reinterpret_cast<const char*>(buffer_archivo.buf()), info_actual_archivo.uncompressed_size);

	archivo_nuevo.close();

	struct tm st;
	memset(&st, 0, sizeof(struct tm));
	st.tm_isdst = -1;

	st.tm_year	= info_actual_archivo.tmu_date.tm_year - 1900;
	st.tm_mon	= info_actual_archivo.tmu_date.tm_mon;
	st.tm_mday	= info_actual_archivo.tmu_date.tm_mday;

	st.tm_hour	= info_actual_archivo.tmu_date.tm_hour;
	st.tm_min 	= info_actual_archivo.tmu_date.tm_min;
	st.tm_sec	= info_actual_archivo.tmu_date.tm_sec;

	struct utimbuf times;

	times.actime = (time_t)mktime(&st);
	times.modtime = (time_t)mktime(&st);

	if (utime(ruta_completa.c_str(), &times) == -1) throw GuruError().descripcion();
}

void GuruUnzip::contenido(map<string, unz_file_info>& info_archivos)
{
	info_archivos.clear();

	try{
		for (GuruUnzip::iterator it=begin();it!=end();++it)
			info_archivos.insert(make_pair(it.nombre(), it.informacion()));

	}catch (string&){ throw ;};
}

void GuruUnzip::abrir()
{
	archivo_zip = unzOpen(nombre_archivo.c_str());

	if (archivo_zip == NULL) throw GuruError().descripcion(errno);
}

void GuruUnzip::cerrar()
{
	unzClose(archivo_zip);

	archivo_zip = NULL;
}
