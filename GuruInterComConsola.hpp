#if !defined _GURUINTERCOMCONSOLA_H__
#define _GURUINTERCOMCONSOLA_H__

#include <iostream>
#include <string>

#include <GuruInterCom.hpp>

using namespace std;

class GuruInterComConsola  : public GuruInterCom
{
public:
	GuruInterComConsola() {};
	virtual ~GuruInterComConsola(){};

protected:
	virtual	void salida(const string& texto) {
		cout << texto;
	}

	virtual	void salida(unsigned long numero) {
		cout << numero;
	}

	virtual	string entrada(void) {
		string comando_usuario;

		getline(cin, comando_usuario);

		return comando_usuario;
	}
};

#endif
