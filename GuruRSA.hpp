#ifndef __GURURSA_H__
#define __GURURSA_H__

#include <polarssl/x509.h>
#include <polarssl/rsa.h>
#include <polarssl/havege.h>

template <typename T>
unsigned char HexToDec(const T hexa1, const T hexa2)
{
	T uno=hexa1,dos=hexa2,final=0;

	if (hexa1 >= 'A' && hexa1 <= 'F')
		uno = hexa1 - 55;
	else if (hexa1 >= 'a' && hexa1 <= 'f')
		uno = hexa1 - 87;
	else if (hexa1 >= '0' && hexa1 <= '9')
		uno = hexa1 - 48;

	if (hexa2 >= 'A' && hexa2 <= 'F')
		dos = hexa2 - 55;
	else if (hexa2 >= 'a' && hexa2 <= 'f')
		dos = hexa2 - 87;
	else if (hexa2 >= '0' && hexa2 <= '9')
		dos = hexa2 - 48;

	final = (uno<<4) + dos;

	return final;
}

template <typename T>
class GuruRSA_base { 
	rsa_context rsa;
public:
	GuruRSA(const T& ArchivoPEM):archivo(ArchivoPEM) { 
		rsa_init(&rsa, RSA_PKCS_V15, 0);

		if (x509parse_public_keyfile( &rsa, archivo.c_str()) == 0) {
			if (rsa_check_pubkey(&rsa) == 0)
				llave=RSA_PUBLIC;
			else if (rsa_check_privkey(&rsa) == 0)
				llave=RSA_PRIVATE;
			else 
				llave=POLARSSL_ERR_RSA_KEY_CHECK_FAILED;
		} else 
			llave=POLARSSL_ERR_RSA_BAD_INPUT_DATA;
	};

	virtual ~GuruRSA(void) {
		if (rsa.len) rsa_free(&rsa);
	};
private:
	string	archivo;
	int		llave; //  privada o publica
};

template <typename T>
class GuruRSAEncriptar:public GuruRSA_base {
public:
	GuruRSAEncriptar(const T& ArchivoPEM):GuruRSA(ArchivoPEM) {
		havege_init( &hs );
	}

	string encriptar(const unsigned char& buffer) {
		// int salida = rsa_pkcs1_encrypt( &rsa, havege_rand, &hs, llave, strlen("Soda Stereo"), txt, encriptado);
		return string();
	};

private:
	havege_state hs;
};

template <typename T>
class GuruRSADesencriptar: public GuruRSA_base {
public:
	GuruRSADesencriptar(const T& ArchivoPEM):GuruRSA(ArchivoPEM) {}
	string desencriptar() {
		// int salida = rsa_pkcs1_decrypt( &rsa, llave, &i, decode, desencriptado, 2048);
		return string();
	};

};

#endif
