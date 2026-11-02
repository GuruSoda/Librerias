#include "Base64.h"
#include <stdio.h>

int main ()
{
	char nombre[150];
	char destino[150];

	strcpy(nombre, "Fabricio y Soda Stereo");
	printf("Codificando %s\n", nombre);

	base64_encode(nombre, 50, destino, 30);

	printf("Codificado = %s\n", destino);

	base64_decode(destino, nombre, 50);

	printf("Decodificado = %s\n", nombre);

	return 0;
}

