#if !defined __GURUPARSERMAME_H__
#define __GURUPARSERMAME_H__

#include "GuruCompat.h"
#include "GuruTiposDeDatos.h"
#include "GuruBuffer.h"
#include "GuruListaGenerica.h"

#if (defined __GURUWIN32__)
	#include "windows.h" // para la constante MAX_PATH
#endif
// #include "GuruCorrer.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

// #include <unistd.h>

#define				LISTAINFO					"listinfo.txt"
#define				GPM_TAMANIO_BUFFER			164

#define				GPM_LARGO_ARCHIVO			15

#define				GPM_ALL						0			// 0
#define				GPM_NAME					1			// 1
#define				GPM_DESCRIPTION				2			// 2
#define				GPM_YEAR					4			// 3
#define				GPM_MANUFACTURER			8			// 4
#define				GPM_CLONEOF					16			// 5
#define				GPM_ROMOF					32			// 6
#define				GPM_SAMPLEOF				64			// 7
#define				GPM_ROM						128			// 8
#define				GPM_SAMPLE					256			// 9
#define				GPM_CHIP					512			// 10
#define				GPM_VIDEO					1024		// 11
#define				GPM_INPUT					2048		// 12
#define				GPM_DIPSWITCH				4096		// 13
#define				GPM_DRIVER					8192		// 14
#define				GPM_SOUND					16384		// 15
#define				GPM_GAME					32768		// 16
#define				GPM_RESOURCE				65536		// 17


enum	tipo_caracter	{error, fin_archivo, simbolo, abre_parentesis, cierra_parentesis, cadena};
enum	entrada			{game , name, description, year, manufacturer, cloneof, romof, sampleof, rom, disk, biosset,
						sample, chip, video, input, dipswitch, driver, sound, resource, fin_game, fin_resource};

typedef struct _rom
	{
	GuruBuffer nombre;
	GuruBuffer unido;
	GuruBuffer tamanio;
	GuruBuffer crc;
	GuruBuffer sha1;
	GuruBuffer region;
	GuruBuffer ofset;
	}data_rom;

typedef struct 
		{
		GuruBuffer nombre;
		GuruBuffer descripcion;
		GuruBuffer anio;
		GuruBuffer fabricante;
		GuruBuffer rom_de;
		GuruBuffer clon_de;

		GuruListaGenerica <data_rom> rom;
		}juego;

typedef	void (entrada_listinfo) (enum entrada, int cant_items, char *valores[], void *extra_parametro);
typedef	void (seccion_listinfo) (juego *info_juego, void *extra_parametro);

class GuruParserMame
{
public:
	GuruParserMame();
	virtual ~GuruParserMame();

	bool	cargar_listinfo(const char *ruta_mame = "mame.exe", char *dir_temp = NULL);
	bool	cargar_listinfo_V2(const char *ruta_mame = "mame.exe", char *dir_temp = NULL);
	void	cargar_notificacion(entrada_listinfo *nueva_notificacion, void *extra_parametro = NULL);
	void	cargar_notificacion_seccion(seccion_listinfo *nueva_notificacion, void *extra_parametro_seccion = NULL);
	void	cancelar_carga();
	time_t	tiempo_analisis();

protected:

	bool	abrir_listinfo(const char *ruta_mame, char *dir_temp = NULL);
	bool	extraer_listinfo(char *nombre_listinfo, const char *ruta_mame, char *dir_temp = NULL);
	bool	cargar_a_memoria();

	void	cerrar_listinfo();

	int		sacar_caracter();
	void	agregar_caracter(int caracter);
	enum	tipo_caracter	sacar_token();
	enum	tipo_caracter	sacar_cadena();
	enum	tipo_caracter	sacar_simbolo();

	void	armar_registro_de_simbolos(enum	entrada tipo_entrada);
	void	armar_registro_de_simbolos_V2(enum	entrada tipo_entrada);
	void	armar_registro_de_cadenas(enum	entrada tipo_entrada);


	char *	mostrar_buffer();
	void	limpiar_cadena();

protected:

	dword	opciones_notificacion;

	bool	analizando;

	time_t	inicio_parser;
	time_t	fin_parser;

	bool	en_memoria;					// indica si tiene que cargar el archivo completo a memoria
	char *	archivo;					// puntero a memoria donde se almacena la informacion del archivo
	char *  inicio_archivo;				// puntero al inicio de memoria donde almacena el archivo
	FILE	*mame_txt;					// Almacena la info sobre el archivo analizando
	int		caracter;					// Almacena el caracter leido desde el archivo
	char	*buffer;					// Almacena el token sacado del archivo

	entrada_listinfo	*notificar;		// Puntero a la funcion donde se notifica cada carga del archivo
	seccion_listinfo	*notificar_seccion;		// Puntero a la funcion donde se notifica cada carga de una seccion completa
	void	*parametro_notificacion;			// Guarda un parametro extra para la llamada a la notificacion
	void	*parametro_notificacion_seccion;	// Guarda un parametro extra para la llamada a la notificacion
};

#endif // !defined(AFX_GURUPARSERMAME_H__EB88AD2C_7014_4FA3_9468_1D8B58E11984__INCLUDED_)

/*
game (
	name gng
	description "Ghosts'n Goblins (World? set 1)"
	year 1985
	manufacturer "Capcom"
	rom ( name gg4.bin size 16384 crc 66606beb sha1 4c640f49be93c7d2b12d4d4c56c56e74099b6c2f region cpu1 offs 4000 )
	rom ( name gg3.bin size 32768 crc 9e01c65e sha1 a87880d87c64a6d61313c3bc69c8d49511e0f9c3 region cpu1 offs 8000 )
	rom ( name gg5.bin size 32768 crc d6397b2b sha1 39aa3cb8c229e60ac0ac410ff61e0c09dba78501 region cpu1 offs 10000 )
	rom ( name gg2.bin size 32768 crc 615f5b6f sha1 7ef9ec5c2072e21c787a6bbf700033f50c759c1d region cpu2 offs 0 )
	rom ( name gg1.bin size 16384 crc ecfccf07 sha1 0a1518e19a2e0a4cc3dde4b9568202ea911b5ece region gfx1 flags dispose offs 0 )
	rom ( name gg11.bin size 16384 crc ddd56fa9 sha1 f9d77eee5e2738b7e83ba02fcc55dd480391479f region gfx2 flags dispose offs 0 )
	rom ( name gg10.bin size 16384 crc 7302529d sha1 8434c994cc55d2586641f3b90b6b15fd65dfb67c region gfx2 flags dispose offs 4000 )
	rom ( name gg9.bin size 16384 crc 20035bda sha1 bbb1fba0eb19471f66d29526fa8423ccb047bd63 region gfx2 flags dispose offs 8000 )
	rom ( name gg8.bin size 16384 crc f12ba271 sha1 1c42fa02cb27b35d10c3f7f036005e747f9f6b79 region gfx2 flags dispose offs c000 )
	rom ( name gg7.bin size 16384 crc e525207d sha1 1947f159189b3a53f1251d8653b6e7c65c91fc3c region gfx2 flags dispose offs 10000 )
	rom ( name gg6.bin size 16384 crc 2d77e9b2 sha1 944da1ce29a18bf0fc8deff78bceacba0bf23a07 region gfx2 flags dispose offs 14000 )
	rom ( name gg17.bin size 16384 crc 93e50a8f sha1 42d367f57bb2fdf60a0445ac1533da99cfeaa617 region gfx3 flags dispose offs 0 )
	rom ( name gg16.bin size 16384 crc 06d7e5ca sha1 9e06012bcd82f98fad43de666ef9a75979d940ab region gfx3 flags dispose offs 4000 )
	rom ( name gg15.bin size 16384 crc bc1fe02d sha1 e3a1421d465b87148ffa94f5673b2307f0246afe region gfx3 flags dispose offs 8000 )
	rom ( name gg14.bin size 16384 crc 6aaf12f9 sha1 207a7407288182a4f3eddaea634c6a6452131182 region gfx3 flags dispose offs c000 )
	rom ( name gg13.bin size 16384 crc e80c3fca sha1 cb641c25bb04b970b2cbeca41adb792bbe142fb5 region gfx3 flags dispose offs 10000 )
	rom ( name gg12.bin size 16384 crc 7780a925 sha1 3f129ca6d695548b659955fe538584bd9ac2ff17 region gfx3 flags dispose offs 14000 )
	rom ( name tbp24s10.14k size 256 crc 0eaf5158 sha1 bafd4108708f66cd7b280e47152b108f3e254fc9 region proms offs 0 )
	rom ( name 63s141.2e size 256 crc 4a1285a4 sha1 5018c3950b675af58db499e2883ecbc55419b491 region proms offs 100 )
	chip ( type cpu name M6809 clock 1500000 )
	chip ( type cpu flags audio name Z80 clock 3000000 )
	chip ( type audio name YM2203 clock 1500000 )
	chip ( type audio name YM2203 clock 1500000 )
	video ( screen raster orientation horizontal x 256 y 224 aspectx 4 aspecty 3 freq 60.000000 )
	sound ( channels 1 )
	input ( players 2 control joy8way buttons 2 coins 2 )
	dipswitch ( name "Coinage" entry "4 Coins/1 Credit" entry "3 Coins/1 Credit" entry "2 Coins/1 Credit" entry "3 Coins/2 Credits" entry "4 Coins/3 Credits" entry "1 Coin/1 Credit" default "1 Coin/1 Credit" entry "3 Coins/4 Credits" entry "2 Coins/3 Credits" entry "1 Coin/2 Credits" entry "2 Coins/5 Credits" entry "1 Coin/3 Credits" entry "1 Coin/4 Credits" entry "1 Coin/5 Credits" entry "1 Coin/6 Credits" entry "1 Coin/7 Credits" entry "Free Play" )
	dipswitch ( name "Coinage affects" entry "Coin A" default "Coin A" entry "Coin B" )
	dipswitch ( name "Demo Sounds" entry "Off" entry "On" default "On" )
	dipswitch ( name "Service Mode" )
	dipswitch ( name "Flip Screen" entry "Off" default "Off" entry "On" )
	dipswitch ( name "Lives" entry "3" default "3" entry "4" entry "5" entry "7" )
	dipswitch ( name "Cabinet" entry "Upright" default "Upright" entry "Cocktail" )
	dipswitch ( name "Bonus Life" entry "20000 70000 70000" default "20000 70000 70000" entry "30000 80000 80000" entry "20000 80000" entry "30000 80000" )
	dipswitch ( name "Difficulty" entry "Easy" entry "Normal" default "Normal" entry "Difficult" entry "Very Difficult" )
	dipswitch ( name "Unknown" entry "Off" default "Off" entry "On" )
	driver ( status good color good sound good palettesize 256 )
)


*/
