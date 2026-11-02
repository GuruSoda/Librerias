#include <GuruCRC.h>

GuruCRC::GuruCRC()
{
}

GuruCRC::~GuruCRC()
{

}

//////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////
std::string GuruCRC::crc32(byte *buffer, word tamanio)
{
	if (!buffer) return std::string("0");

	dword crc = 0xffffffffL;

	while (tamanio >= 8)
		{
		DO8(buffer);
		tamanio -= 8;
		}

	if (tamanio)
		do {
			DO1(buffer);
		} while (--tamanio);

	return dword_a_string(crc ^ 0xffffffffL);
}

//////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////
word GuruCRC::crc16(byte *buffer, word tamanio)
{
	if (!buffer) return 0L;

	word crc = 0xffff;
//	word crc = 0x0000;


	for ( dword i = 0; i < tamanio; i++ )
		crc = (word)(( tabla_crc16[ ((crc) ^ (*buffer++ )) & 0xff ] ^ ((crc) >> 8 )));
	//	crc = (crc << 8) ^ tabla_crc16[((crc >> 8) ^ (*buffer++)) & 0xff];

//	return (word)crc;
	return (word)(crc ^ 0xffff);
}

//////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////
dword GuruCRC::generar_crc32(byte *buffer, word tamanio, dword *buffer_crc)
{
	if (!buffer) return 0L;

	dword crc = *buffer_crc ^ 0xffffffffL;

	while (tamanio >= 16)
		{
		DO16(buffer);
		tamanio -= 16;
		}

    if (tamanio)
		do {
			DO1(buffer);
		} while (--tamanio);

	return *buffer_crc = crc ^ 0xffffffffL;
}

//////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////
word GuruCRC::generar_crc16(byte *buffer, word tamanio, word *buffer_crc)
{
	if (!buffer) return 0L;

//	word crc = *buffer_crc ^ 0xffff;

	word crc = *buffer_crc;

	for ( dword i = 0; i < tamanio; i++ )
		crc = (word)(( tabla_crc16[ ((crc) ^ (*buffer++ )) & 0xff ] ^ ((crc) >> 8 )));
//		crc = (crc<<8) ^ tabla_crc16[((crc>>8) ^ *(char *)buffer++)&0x00FF];

//	return *buffer_crc = (word)(crc);
	return *buffer_crc = (word)(crc ^ 0xffff);
}

//////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////
std::string GuruCRC::archivo_crc32(const std::string& nombre_archivo)
{
	FILE* archivo = fopen(nombre_archivo.c_str(), "rb");

	if (!archivo) return std::string("0");

	char	buffer[1024];
	dword	crc = 0;
	dword	cant_leidos = 0;

	while ( (cant_leidos = (dword)fread(buffer, 1, 1024, archivo)) > 0)
		generar_crc32((byte*)buffer, cant_leidos, &crc);

	fclose(archivo);

	return dword_a_string(crc);
}

//////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////
std::string GuruCRC::archivo_crc16(const std::string& nombre_archivo)
{
	FILE* archivo = fopen(nombre_archivo.c_str(), "rb");

	if (!archivo) return std::string("0");

	char	buffer[1024];
	word	crc = 0;
	dword	cant_leidos = 0;

	while ((cant_leidos = (dword)fread(buffer, 1, 1024, archivo)) > 0)
		generar_crc16((byte*)buffer, cant_leidos, &crc);

	fclose(archivo);

	return dword_a_string(crc);
}

std::string GuruCRC::dword_a_string(dword checksum)
{
	std::ostringstream		os;

	os  << hex  << checksum;
	
	return string(os.str());

/*
	static const char* binToStr = "0123456789ABCDEF";
	char		cadena_crc[9] = {0};
	dword		crc = checksum;

	cout << "Checksum: "  << checksum  << " - "  << hex  << checksum <<endl;
	for (int i=28,j=0;j<8;i-=4,j++)	{
		
		crc = (crc << (28-i));
		crc = (crc >> (28-i));
		cout << "Indice ... "  << j << " "  <<  (crc >> i) << " " << crc  <<endl;
		cadena_crc[j] = binToStr[crc >> i];
		crc = checksum;
	}

	return std::string(cadena_crc);
*/

}

