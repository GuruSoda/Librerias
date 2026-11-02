// GuruRam.h:
//
//////////////////////////////////////////////////////////////////////

#if !defined GURURAM_H__
#define GURURAM_H__

#include <windows.h>
#include "..\GuruTiposDeDatos.h"
#include "..\GuruCompat.h"

enum titulo_ventana {arriba_derecha, arriba_centro, arriba_izquierda, abajo_derecha, abajo_centro, abajo_izquierda};
enum borde_ventana   {doble_doble, simple_simple, doble_simple, simple_doble};
enum posicion {alineado_centrado=0xff, alineado_derecha=0xfe, alineado_izquierda=0xfd, alineado_arriba=0xfc, alineado_abajo=0xfb};

typedef struct ESCRIBIR
	{
	unsigned int id;

	int  x;               // Posicion x
	int  y;               // Posicion y
	byte ColorFrente;     // Color del caracter
	byte ColorFondo;      // Color del fondo

	const char *Frase;          // Frase a escribir
	}ESCRIBIR;

typedef struct
	{
	CHAR_INFO *celdas_salvadas;
	SMALL_RECT region_salvada;
	}region_video;

typedef struct VENTANA
	{
	unsigned int id;

	int  x;               // Posicion x
	int  y;               // Posicion y
	int  Ancho;           // Cantidad de columnas
	int  Alto;            // Cantidad de filas
	byte ColorFrente;     // Color del caracter
	byte ColorFondo;      // Color del fondo

	char *Titulo;         // Titulo de la ventana
	enum titulo_ventana LugarTitulo;     // Posicion del titulo

	enum borde_ventana  tipo_borde;

	unsigned long *atras; // lo que hay atras de la ventana

	bool Sombra;          // Con o sin sombra
	}VENTANA;

class GuruRam
{
public:
	enum teclado {
 /* 08 VK_BACK */   kbBackSpace = 0x0E08,
 /* 09 VK_TAB */    kbTab		= 0x0F09,
 /* 0D VK_RETURN */ kbEnter		= 0x1C0D,
 /* 1B VK_ESCAPE */ kbEsc		= 0x011B,
 /* 20 VK_SPACE */  kbSpace		= 0x3920,
 /* 21 VK_PRIOR */  kbPgUp      = 0x2100,
 /* 22 VK_NEXT */   kbPgDn      = 0x2200,
 /* 23 VK_END */    kbEnd       = 0x2300,
 /* 24 VK_HOME */   kbHome      = 0x2400,
 /* 25 VK_LEFT */   kbLeft      = 0x2500,
 /* 26 VK_UP */     kbUp        = 0x2600,
 /* 27 VK_RIGHT */  kbRight     = 0x2700,
 /* 28 VK_DOWN */   kbDown      = 0x2800,
 /* 2D VK_INSERT */ kbInsert    = 0x2D00,
 /* 2E VK_DELETE */ kbDelete    = 0x2E00,
 /* 30 VK_0 '0' */  kb0         = 0x30,
 /* 31 VK_1 '1' */  kb1         = 0x31,
 /* 32 VK_2 '2' */  kb2         = 0x32,
 /* 33 VK_3 '3' */  kb3         = 0x33,
 /* 34 VK_4 '4' */  kb4         = 0x34,
 /* 35 VK_5 '5' */  kb5         = 0x35,
 /* 36 VK_6 '6' */  kb6         = 0x36,
 /* 37 VK_7 '7' */  kb7         = 0x37,
 /* 38 VK_8 '8' */  kb8         = 0x38,
 /* 39 VK_9 '9' */  kb9         = 0x39,
 /* 41 VK_A 'A' */  kbA         = 0x41,
 /* 42 VK_B 'B' */  kbB         = 0x42,
 /* 43 VK_C 'C' */  kbC         = 0x43,
 /* 44 VK_D 'D' */  kbD         = 0x44,
 /* 45 VK_E 'E' */  kbE         = 0x45,
 /* 46 VK_F 'F' */  kbF         = 0x46,
 /* 47 VK_G 'G' */  kbG         = 0x47,
 /* 48 VK_H 'H' */  kbH         = 0x48,
 /* 49 VK_I 'I' */  kbI         = 0x49,
 /* 4A VK_J 'J' */  kbJ         = 0x4A,
 /* 4B VK_K 'K' */  kbK         = 0x4B,
 /* 4C VK_L 'L' */  kbL         = 0x4C,
 /* 4D VK_M 'M' */  kbM         = 0x4D,
 /* 4E VK_N 'N' */  kbN         = 0x4E,
 /* 4F VK_O 'O' */  kbO         = 0x4F,
 /* 50 VK_P 'P' */  kbP         = 0x50,
 /* 51 VK_Q 'Q' */  kbQ         = 0x51,
 /* 52 VK_R 'R' */  kbR         = 0x52,
 /* 53 VK_S 'S' */  kbS         = 0x53,
 /* 54 VK_T 'T' */  kbT         = 0x54,
 /* 55 VK_U 'U' */  kbU         = 0x55,
 /* 56 VK_V 'V' */  kbV         = 0x56,
 /* 57 VK_W 'W' */  kbW         = 0x57,
 /* 58 VK_X 'X' */  kbX         = 0x58,
 /* 59 VK_Y 'Y' */  kbY         = 0x59,
 /* 5A VK_Z 'Z' */  kbZ         = 0x5A,
 /* 60 NumKeyPad '0' */  kbNumPad0    = 0x60,
 /* 61 NumKeyPad '1' */  kbNumPad1    = 0x61,
 /* 62 NumKeyPad '2' */  kbNumPad2    = 0x62,
 /* 63 NumKeyPad '3' */  kbNumPad3    = 0x63,
 /* 64 NumKeyPad '4' */  kbNumPad4    = 0x64,
 /* 65 NumKeyPad '5' */  kbNumPad5    = 0x65,
 /* 66 NumKeyPad '6' */  kbNumPad6    = 0x66,
 /* 67 NumKeyPad '7' */  kbNumPad7    = 0x67,
 /* 68 NumKeyPad '8' */  kbNumPad8    = 0x68,
 /* 69 NumKeyPad '9' */  kbNumPad9    = 0x69,
 /* 6A VK_MULTIPLY */    kbAsterisk   = 0x6A,
 /* 6B VK_ADD */         kbPlus       = 0x6B,
 /* 6C VK_SEPARATOR */   kbBackSlash  = 0x6C,
 /* 6D VK_SUBSTRACT */   kbMinus      = 0x6D,
 /* 6E VK_DECIMAL */     kbGrave      = 0x6E,
 /* 6F VK_DIVIDE */      kbSlash      = 0x6F,
 /* 70 VK_F1 'F1' */     kbF1         = 0x7000,
 /* 71 VK_F2 'F2' */     kbF2         = 0x7100,
 /* 72 VK_F3 'F3' */     kbF3         = 0x7200,
 /* 73 VK_F4 'F4' */     kbF4         = 0x7300,
 /* 74 VK_F5 'F5' */     kbF5         = 0x7400,
 /* 75 VK_F6 'F6' */     kbF6         = 0x7500,
 /* 76 VK_F7 'F7' */     kbF7         = 0x7600,
 /* 77 VK_F8 'F8' */     kbF8         = 0x7700,
 /* 78 VK_F9 'F9' */     kbF9         = 0x7800,
 /* 79 VK_F10 'F10' */   kbF10        = 0x7900,
 /* 7A VK_F11 'F11' */   kbF11        = 0x7A00,
 /* 7B VK_F12 'F12' */   kbF12        = 0x7B00
};

	enum color {
// ---------------------¦Back-¦Fore-
// Constant       ¦Value¦grnd?¦grnd?
// ---------------------¦-----¦-----
  black = 0,   /* ¦  0  ¦ Yes ¦ Yes */
  blue,        /* ¦  1  ¦ Yes ¦ Yes */
  green,       /* ¦  2  ¦ Yes ¦ Yes */
  cyan,        /* ¦  3  ¦ Yes ¦ Yes */
  red,         /* ¦  4  ¦ Yes ¦ Yes */
  magenta,     /* ¦  5  ¦ Yes ¦ Yes */
  brown,       /* ¦  6  ¦ Yes ¦ Yes */
  lightgray,   /* ¦  7  ¦ Yes ¦ Yes */
  darkgray,    /* ¦  8  ¦ No  ¦ Yes */
  lightblue,   /* ¦  9  ¦ No  ¦ Yes */
  lightgreen,  /* ¦ 10  ¦ No  ¦ Yes */
  lightcyan,   /* ¦ 11  ¦ No  ¦ Yes */
  lightred,    /* ¦ 12  ¦ No  ¦ Yes */
  lightmagenta,/* ¦ 13  ¦ No  ¦ Yes */
  yellow,      /* ¦ 14  ¦ No  ¦ Yes */
  white,       /* ¦ 15  ¦ No  ¦ Yes */
// ---------------+-----+-----+------
  blink = 128  /* ¦128  ¦ No  ¦ ***	 */
};
	static GuruRam* instancia();

//	GuruRam(GuruRam &copia);
	virtual ~GuruRam();

	bool	inicializar(byte ColorFrente = white, byte ColorFondo = blue, enum borde_ventana borde = doble_doble, byte ancho_ventana = 80, byte alto_ventana = 25);
	void	poner_handle(HANDLE Handle_Consola);
	HANDLE	sacar_handle();

//////////////////////////////////////////////////////////////////////
// Para las funciones basicas de Consola
//////////////////////////////////////////////////////////////////////
	virtual void	ventana  (struct VENTANA *data_ventana);
	virtual void	escribir (struct ESCRIBIR *data_escribir);
	virtual void	escribir (int x, int y, byte ColorFrente, byte ColorFondo, const char *Frase);
	virtual void	escribir (int x, int y, byte ColorFrente, byte ColorFondo, char caracter);
//	virtual void	escribir (int x, int y, char caracter);

//	void    salvar(int x, int y, int ancho, int alto);
//	void    recuperar();

	void	salvar();
	void	recuperar();

	void	salvar(int x, int y, int ancho, int alto, region_video* ptr_celdas_salvadas);
	void	recuperar(region_video* ptr_celdas_salvadas);
	void	mostrar_region(region_video *ptr_region_salvada);
	void	destruir(region_video *ptr_region_salvada);

	bool	cursor(word x, word y);
	void	cursor (bool mostrar);

//////////////////////////////////////////////////////////////////////
// Miscelaneas Publicas
//////////////////////////////////////////////////////////////////////
	void	fondo();

//////////////////////////////////////////////////////////////////////
// Miscelaneas Privadas
//////////////////////////////////////////////////////////////////////
	byte	armar_color(byte ColorFrente, byte ColorFondo);

//////////////////////////////////////////////////////////////////////
// Atributos basicos
//////////////////////////////////////////////////////////////////////
protected:
	GuruRam(); // el constructor esta protegido debido al patro singleton
	GuruRam(int nada /*dummy*/);

private:
	static GuruRam* singleton;

public:

    HANDLE	original;
	HANDLE	consola;

	HANDLE	entrada;
	HANDLE	error;

	byte	DefaultColorFrente;
	byte	DefaultColorFondo;

	byte	ancho;		// de cuantas columnas en la pantalla/ventana
	byte	alto;		// de cuantas filas es la pantalla/ventana

	byte	ancho_dispositivo; // ancho del dispositivo (en Windows 80)
	byte	alto_dispositivo;  // ancho del dispositivo (en Windows 25)

	region_video *ptr_region_video;		// puede salvar parte de la ventana;
};

#endif // !defined GURURAM_H__
