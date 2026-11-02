#ifndef __GURUHCONSOLA_H__
#define __GURUHCONSOLA_H__

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class GuruHConsola
{
public:
	GuruHConsola(void):buff_cout(NULL),buff_cerr(NULL),buff_clog(NULL) {};
	virtual ~GuruHConsola(void) {};

	void redirect_cout(const string& archivo="") {
		archivo_cout.open(archivo.c_str());

		if (archivo_cout){
			buff_cout=cout.rdbuf();
			cout.rdbuf(archivo_cout.rdbuf());
		} else if (buff_cout!=NULL) {
				cout.rdbuf(buff_cout);
				if (archivo_cout) 
					archivo_cout.close();
		}
	}

	void redirect_cerr(const string& archivo=""){
		archivo_cerr.open(archivo.c_str());

		if (archivo_cerr){
			buff_cerr=cerr.rdbuf();
			cerr.rdbuf(archivo_cerr.rdbuf());
		} else if (buff_cerr!=NULL) {
				cerr.rdbuf(buff_cerr);
				if (archivo_cerr) 
					archivo_cerr.close();
		}
	}
	
	void redirect_clog(const string& archivo="") {};

protected:
	streambuf*	buff_cout;
	streambuf*	buff_cerr;
	streambuf*	buff_clog;

	ofstream	archivo_cout;
	ofstream	archivo_cerr;
	ofstream	archivo_clog;
};

#endif
