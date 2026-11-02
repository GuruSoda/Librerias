#include "GuruZip.h"

void GuruZip::agregar_archivo(const string& nombre_en_zip, const string& nombre_en_disco)
{
	archivos.insert(make_pair(nombre_en_zip, nombre_en_disco));
}

void GuruZip::grabar()
{
	archivo_zip = zipOpen(nombre_archivo.c_str(), 0);

	if (archivo_zip==NULL) throw string("Error creando archivo zip.");

	for (map<string, string>::iterator it=archivos.begin();it!=archivos.end();++it){
		zip_fileinfo zinfo;
		zinfo.dosDate=0;
		zinfo.external_fa=0;
		zinfo.internal_fa=0;

		fs::path ruta(it->second);
		struct tm* fecha={0};
		time_t fecha_archivo=fs::last_write_time(ruta);

		fecha=localtime(&fecha_archivo);

		fecha->tm_isdst=-1;
		zinfo.tmz_date.tm_hour = fecha->tm_hour;
		zinfo.tmz_date.tm_min  = fecha->tm_min;
		zinfo.tmz_date.tm_sec  = fecha->tm_sec;
		zinfo.tmz_date.tm_year = fecha->tm_year+1900;
		zinfo.tmz_date.tm_mon  = fecha->tm_mon;
		zinfo.tmz_date.tm_mday = fecha->tm_mday;

		int cod_abrir=zipOpenNewFileInZip(archivo_zip,
		                       it->first.c_str(),
		                       &zinfo,
		                       NULL,
		                       0,
		                       NULL,
		                       0,
		                       "Hecho por ArMame 2.0",
		                       Z_DEFLATED,
		                       Z_DEFAULT_COMPRESSION);

		if (cod_abrir!=0) throw string("Error creando nuevo archivo en zip.");

		ifstream archivo_a_guardar(it->second.c_str(), ifstream::in | ios_base::binary);

		if (!archivo_a_guardar) throw GuruError().descripcion();

		uint64_t tamanio_archivo=fs::file_size(ruta);

		GuruBuffer buffer_archivo(tamanio_archivo+1);

		archivo_a_guardar.read(reinterpret_cast<char*>(buffer_archivo.buf()), tamanio_archivo);

		if (archivo_a_guardar.bad()) throw string("Error leyendo el archivo " + ruta.filename());

		archivo_a_guardar.close();

		int cod_grabar=zipWriteInFileInZip(archivo_zip, buffer_archivo.buf(), tamanio_archivo);

		if (cod_grabar!=0) throw string("Error grando el nuevo archivo en zip.");

		zipCloseFileInZip(archivo_zip);
	}

	zipClose(archivo_zip, "Archivo enzamblado con el ArMame 2.0");
}
