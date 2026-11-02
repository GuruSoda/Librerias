// GuruRam.cpp: implementation of the GuruRam class.
//
//////////////////////////////////////////////////////////////////////

#include "GuruRam.h"

GuruRam* GuruRam::singleton = NULL;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruRam::GuruRam()
{
	consola = INVALID_HANDLE_VALUE;

	ptr_region_video = NULL;

	original = GuruRam::instancia()->original;
	consola  = GuruRam::instancia()->consola;

	entrada  = GuruRam::instancia()->entrada;
	error    = GuruRam::instancia()->error;

	ancho    = GuruRam::instancia()->ancho;
	alto     = GuruRam::instancia()->alto;

	ancho_dispositivo = GuruRam::instancia()->ancho_dispositivo;
	alto_dispositivo  = GuruRam::instancia()->alto_dispositivo;	


//	inicializar(white, blue, doble_doble, 80, 25);
}

GuruRam::GuruRam(int nada)
{
	inicializar(white, blue, doble_doble, 80, 25);

	nada = 0;
}

/*
GuruRam::GuruRam(GuruRam &copia)
{
	original = copia.original;
	consola  = copia.consola;

	entrada  = copia.entrada;
	error    = copia.error;

	ancho    = copia.ancho;
	alto     = copia.alto;

	ancho_dispositivo = copia.ancho_dispositivo;
	alto_dispositivo  = copia.alto_dispositivo;	

	ptr_region_video = NULL;
}
*/
GuruRam::~GuruRam()
{
	SetConsoleActiveScreenBuffer(consola);

	if (ptr_region_video) 
		{
		delete ptr_region_video->celdas_salvadas;
		delete ptr_region_video;
		}
}

GuruRam* GuruRam::instancia()
{
	if (singleton == NULL)
		singleton = new GuruRam(8);

	return singleton;
}


//////////////////////////////////////////////////////////////////////
// Inicializa el modo de la consola y la GuruRam
// ColorFrente  - Color del frente del caracter
// ColorFondo   - Color de fondo del caracter
// TipoBorde    - Tipo de borde de las ventanas
// Ancho        - Ancho de la pantalla (en Win32 -> 80)
// Alto         - Alto de la pantall (en Win32 -> 25)
//////////////////////////////////////////////////////////////////////
bool GuruRam::inicializar(byte ColorFrente, byte ColorFondo, enum borde_ventana borde, byte ancho_ventana, byte alto_ventana)
{
	original = GetStdHandle(STD_OUTPUT_HANDLE);

	consola = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	entrada = GetStdHandle(STD_INPUT_HANDLE);
	error   = GetStdHandle(STD_ERROR_HANDLE);


	COORD tamanio = {ancho_ventana, alto_ventana};
	SetConsoleScreenBufferSize(consola, tamanio);

	ancho	= ancho_ventana;
	alto	= alto_ventana;

	ancho_dispositivo = ancho_ventana; // ancho del dispositivo (en Windows 80)
	alto_dispositivo  = alto_ventana;  // ancho del dispositivo (en Windows 25)

	DefaultColorFrente = ColorFrente; 
	DefaultColorFondo  = ColorFondo;

	SetConsoleActiveScreenBuffer(consola);

	return true;
}

void GuruRam::poner_handle(HANDLE Handle_Consola)
{
	consola = Handle_Consola;
}

HANDLE GuruRam::sacar_handle()
{
	return consola;
}

void GuruRam::fondo()
{
	COORD inicio = {0,0};
	dword puestos = 0;

	FillConsoleOutputAttribute(consola, 3, ancho*alto,  inicio, &puestos);
	FillConsoleOutputCharacter(consola, '²', ancho*alto, inicio, &puestos);
}

void GuruRam::ventana (struct VENTANA *data_ventana)
{
	unsigned int columnas = data_ventana->Ancho;
	unsigned int filas    = data_ventana->Alto;
	unsigned int sombra   = 0;
	unsigned int i;
	byte         color    = armar_color(data_ventana->ColorFrente, data_ventana->ColorFondo);

	// para el programa, la posicion empieza en 1,1 y no en 0,0
	if (data_ventana->Sombra) {columnas++; filas++; sombra++;}

	CHAR_INFO *celdas  = new CHAR_INFO [columnas * filas];
	memset(celdas, 0, columnas * filas);

	COORD      tamanio = {columnas, filas};
	COORD      inicio  = {0, 0}; // inicio en el buffer
	SMALL_RECT pintado = {data_ventana->x - 1, data_ventana->y - 1, (data_ventana->x + columnas + sombra), (data_ventana->y + filas + sombra)};

	char borde[11];

	static char lineas_dobles[]         = {'É','»','È','¼','Í','º','Ì','Î','¹','Ë','Ê'};
	static char lineas_simples[]        = {'Ú','¿','À','Ù','Ä','³','Ã','Å','´','Â','Á'};
	static char lineas_simples_dobles[] = {'Ö','·','Ó','½','Ä','º','Ç','×','¶','Ò','Ð'};
	static char lineas_dobles_simples[] = {'Õ','¸','Ô','¾','Í','³','Æ','Ø','µ','Ñ','Ï'};

	switch (data_ventana->tipo_borde)
		{
	case doble_doble: //	static char lineas_dobles[11] = {201,187,200,188,205,186,204,206,185,203,202,};
						for (i=0;i<11;i++)
							borde[i] = lineas_dobles[i];
					
						break;
	case simple_simple: //	static char lineas_simples[11] = {218,191,192,217,196,179,195,197,180,194,193,};
						for (i=0;i<11;i++)
							borde[i] = lineas_simples[i];
											
						break;
	case doble_simple: //	static char simple_doble[11] = {214,183,211,189,196,186,199,215,182,210,208,};
						for (i=0;i<11;i++)
							borde[i] = lineas_dobles_simples[i];
											
						break;
	case simple_doble: //	static char doble_simple[11] = {213,184,212,190,205,179,198,216,181,209,207,};
						for (i=0;i<11;i++)
							borde[i] = lineas_simples_dobles[i];
											
						break;
	default:
			for (i=0;i<11;i++)
				borde[i] = lineas_dobles[i];

		}

//  angulo superior izquierdo
	celdas[0].Char.AsciiChar = borde[0];
	celdas[0].Attributes = color;

//  angulo superior derecho
	celdas[(columnas - sombra) - 1].Char.AsciiChar = borde[1];
	celdas[(columnas - sombra) - 1].Attributes = color;

//  angulo inferior izquierdo
	celdas[columnas * (filas-sombra-1)].Char.AsciiChar = borde[2];
	celdas[columnas * (filas-sombra-1)].Attributes = color;

//  angulo inferior derecho
	celdas[(columnas * (filas-sombra)) - 1 - (sombra)].Char.AsciiChar = borde[3];
	celdas[(columnas * (filas-sombra)) - 1 - (sombra)].Attributes = color;


	for (i = 1; i < columnas - 1 - sombra;i++)    // los marcos horizontales
		{
		celdas[i].Char.AsciiChar = borde[4];
		celdas[i].Attributes = color;

		celdas[i + columnas * ((filas-sombra)-1)].Char.AsciiChar = borde[4];
		celdas[i + columnas * ((filas-sombra)-1)].Attributes = color;
		}

	for (i = 1; i < filas - 1 - sombra;i++)         // los marcos verticales
		{
		celdas[i * columnas].Char.AsciiChar = borde[5];
		celdas[i * columnas].Attributes = color;

		celdas[(i * columnas) + (columnas - 1 - sombra)].Char.AsciiChar = borde[5];
		celdas[(i * columnas) + (columnas - 1 - sombra)].Attributes = color;
		}

	for (i = 1; i < columnas - 1 - sombra;i++)        // el relleno adentro de la ventana
		for (unsigned int j = 1; j < filas - 1 - sombra;j++)
			{
			celdas[(j * columnas) + i].Char.AsciiChar = ' ';
			celdas[(j * columnas) + i].Attributes = color;
			}

	if (data_ventana->Sombra)
		{
		WORD   atributo;
		char   caracter[2];
		COORD  lugar = {0,0};
		dword  leidos;
		byte   color_sombra = armar_color(lightgray, black);

		//  para el lugarcito del angulo superior derecho
		lugar.X = (columnas-1) + data_ventana->x - 1;
		lugar.Y = data_ventana->y - 1;

		ReadConsoleOutputCharacter(consola, caracter, 1, lugar, &leidos);
		ReadConsoleOutputAttribute(consola, &atributo, sizeof(word), lugar, &leidos);

		celdas[columnas - 1].Char.AsciiChar = caracter[0];
		celdas[columnas - 1].Attributes = atributo;

		//  para el lugarcito del angulo inferior izquierdo
		lugar.X = data_ventana->x-1;
		lugar.Y = data_ventana->y-1 + filas-1;// filas-1 agregado el 24/09/2004

		ReadConsoleOutputCharacter(consola, caracter, 1, lugar, &leidos);
		ReadConsoleOutputAttribute(consola, &atributo, sizeof(word), lugar, &leidos);

		celdas[columnas * (filas - 1)].Char.AsciiChar = caracter[0];
		celdas[columnas * (filas - 1)].Attributes = atributo;

		for (i = 2; i < filas + 1;i++) // sombra vertical
			{
			celdas[i * columnas - 1].Char.AsciiChar = ' ';
			celdas[i * columnas - 1].Attributes = color_sombra;
			}

		for (i = 1; i < columnas;i++) // sombra horizontal
			{
			celdas[i + (columnas * (filas - 1))].Char.AsciiChar = ' ';
			celdas[i + (columnas * (filas - 1))].Attributes = color_sombra;
			}
		}

	if (data_ventana->Titulo)
		{
		dword inicio_frase = 0;

		switch (data_ventana->LugarTitulo)
			{
		case arriba_derecha: inicio_frase = (columnas - 2 - sombra) - strlen(data_ventana->Titulo);
						break;
		case arriba_centro: inicio_frase = ((columnas - sombra)/ 2) - (strlen(data_ventana->Titulo) / 2);
						break;
		case arriba_izquierda: inicio_frase = 2;
						break;
		case abajo_derecha: inicio_frase = ((columnas * (filas-sombra)) - 1 - sombra) - (strlen(data_ventana->Titulo) + 1);
						break;
		case abajo_centro:	inicio_frase = (((columnas * (filas-sombra)) - 1 - sombra) - columnas / 2) - (strlen(data_ventana->Titulo) / 2);
						break;
		case abajo_izquierda: inicio_frase = (columnas * (filas-sombra-1)) + 2;
						break;
			}

		if (data_ventana->Titulo)
			for (dword j = 0; j < strlen(data_ventana->Titulo); j++)
				celdas[inicio_frase + j].Char.AsciiChar = data_ventana->Titulo[j];
		}

	WriteConsoleOutput(consola, celdas, tamanio, inicio, &pintado);

	delete celdas;
}

void GuruRam::escribir (struct ESCRIBIR *data_escribir)
{
	dword       escritos = 0;
	COORD       inicio   = {0,0};
	COORD       tamanio  = {strlen(data_escribir->Frase), 1};
	SMALL_RECT  lugar    = {data_escribir->x-1, data_escribir->y-1, (data_escribir->x-1) + strlen(data_escribir->Frase), data_escribir->y-1};
	byte        color    = armar_color(data_escribir->ColorFrente, data_escribir->ColorFondo);

	CHAR_INFO *frase = new CHAR_INFO [strlen(data_escribir->Frase)+1];

	for (unsigned int i=0; i < strlen(data_escribir->Frase); i++)
		{
		frase[i].Char.AsciiChar = data_escribir->Frase[i];
		frase[i].Attributes = color;
		}

	WriteConsoleOutput(consola, frase, tamanio, inicio, &lugar);

	delete frase;
}

void GuruRam::escribir (int x, int y, byte ColorFrente, byte ColorFondo, const char *Frase)
{
	if (Frase)
		{
		struct ESCRIBIR info_escribir = {0xffff, x, y, ColorFrente, ColorFondo, Frase};

		GuruRam::escribir(&info_escribir);
		}
}

void GuruRam::escribir (int x, int y, byte ColorFrente, byte ColorFondo, char caracter)
{
	CHAR_INFO	nuevo_caracter = {caracter, armar_color(ColorFrente, ColorFondo)};
	SMALL_RECT  lugar    = {x-1, y-1, (x-1) + 1, y-1};
	COORD		inicio = {0, 0};
	COORD       tamanio  = {1, 1};

	WriteConsoleOutput(consola, &nuevo_caracter, tamanio, inicio, &lugar);
}
/*
void GuruRam::escribir (int x, int y, char caracter)
{
	COORD lugar = {x - 1, y - 1};
	dword escritos = 0;

	WriteConsoleOutputCharacter(consola, &caracter, 1, lugar, &escritos);
}
*/
void GuruRam::salvar(int x, int y, int ancho, int alto, region_video *ptr_region_salvada)
{
	ptr_region_salvada->celdas_salvadas  = new CHAR_INFO [ancho * alto];

	memset(ptr_region_salvada->celdas_salvadas, 0, ancho * alto);

	COORD      tamanio = {ancho, alto};
	COORD      inicio  = {0, 0}; // inicio en el buffer

	ptr_region_salvada->region_salvada.Left   = x - 1;
	ptr_region_salvada->region_salvada.Top    = y - 1;
	ptr_region_salvada->region_salvada.Right  = x + ancho;
	ptr_region_salvada->region_salvada.Bottom = y + alto;

	ReadConsoleOutput(consola, ptr_region_salvada->celdas_salvadas, tamanio, inicio, &ptr_region_salvada->region_salvada);
}

void GuruRam::recuperar(region_video *ptr_region_salvada)
{
	if (ptr_region_salvada->celdas_salvadas)
		{
		COORD  inicio  = {0, 0}; // inicio en el buffer
		COORD  tamanio = {ptr_region_salvada->region_salvada.Right -  ptr_region_salvada->region_salvada.Left + 1, ptr_region_salvada->region_salvada.Bottom - ptr_region_salvada->region_salvada.Top + 1};

		WriteConsoleOutput(consola, ptr_region_salvada->celdas_salvadas, tamanio, inicio, &ptr_region_salvada->region_salvada);

		delete ptr_region_salvada->celdas_salvadas;

		ptr_region_salvada->celdas_salvadas = NULL;
		}
}

void GuruRam::salvar()
{
	if (ptr_region_video) 
		destruir(ptr_region_video);
	else
		ptr_region_video = new region_video;

	ptr_region_video->celdas_salvadas  = new CHAR_INFO [ancho * alto];

	memset(ptr_region_video->celdas_salvadas, 0, ancho * alto);

	COORD      tamanio = {ancho, alto};
	COORD      inicio  = {0, 0}; // inicio en el buffer

	ptr_region_video->region_salvada.Left   = 0;
	ptr_region_video->region_salvada.Top    = 0;
	ptr_region_video->region_salvada.Right  = ancho;
	ptr_region_video->region_salvada.Bottom = alto;

	ReadConsoleOutput(consola, ptr_region_video->celdas_salvadas, tamanio, inicio, &ptr_region_video->region_salvada);
}

void GuruRam::recuperar()
{
	if (ptr_region_video->celdas_salvadas)
		{
		COORD  inicio  = {0, 0}; // inicio en el buffer
		COORD  tamanio = {ptr_region_video->region_salvada.Right -  ptr_region_video->region_salvada.Left + 1, ptr_region_video->region_salvada.Bottom - ptr_region_video->region_salvada.Top + 1};

		WriteConsoleOutput(consola, ptr_region_video->celdas_salvadas, tamanio, inicio, &ptr_region_video->region_salvada);

		delete ptr_region_video->celdas_salvadas;

		ptr_region_video->celdas_salvadas = NULL;
		}
}

void GuruRam::mostrar_region(region_video *ptr_region_salvada)
{
	if (ptr_region_salvada->celdas_salvadas)
		{
		COORD  inicio  = {0, 0}; // inicio en el buffer
		COORD  tamanio = {ptr_region_salvada->region_salvada.Right -  ptr_region_salvada->region_salvada.Left + 1, ptr_region_salvada->region_salvada.Bottom - ptr_region_salvada->region_salvada.Top + 1};

		WriteConsoleOutput(consola, ptr_region_salvada->celdas_salvadas, tamanio, inicio, &ptr_region_salvada->region_salvada);
		}
}

void GuruRam::destruir(region_video *ptr_region_salvada)
{
	if (ptr_region_salvada->celdas_salvadas)
		{
		delete ptr_region_salvada->celdas_salvadas;
		ptr_region_salvada->celdas_salvadas = NULL;
		}
}

byte GuruRam::armar_color(byte ColorFrente, byte ColorFondo)
{
	return (ColorFondo << 4) + ColorFrente;
}

bool GuruRam::cursor(word x, word y)
{
	COORD posicion;

	posicion.X = x - 1;
	posicion.Y = y - 1;

	return (SetConsoleCursorPosition(consola, posicion)) ? true : false;
}

void GuruRam::cursor (bool mostrar)
{
	CONSOLE_CURSOR_INFO info_cursor;

	GetConsoleCursorInfo(consola, &info_cursor);

	if (mostrar != ((info_cursor.bVisible) ? true : false))
		{
		info_cursor.bVisible = mostrar;

		SetConsoleCursorInfo(consola, &info_cursor);
		}
}
