#ifndef _GURUTOKEN_H_
#define _GURUTOKEN_H_

#include <vector>
#include <string>
#include <GuruString_algo.hpp>

using namespace std;

class GuruToken {
public:
	GuruToken(const string& separadores = " "):_separadores(separadores) {} ;
	virtual ~GuruToken() {};

	vector<string>& extraer(const string& caracteres, vector<string>& tokens, const string& separadores=""){

		tokens.clear();

		if (caracteres.empty()) return tokens;

		if (!separadores.empty()) _separadores=separadores;

		string cadena(GuruStringAlgo::trim_copy(caracteres));
		string temp;

		string::iterator it=cadena.begin();
		string::size_type lugar;
		while (it != cadena.end())	{
			if (*it == '"'){
				char t=*it;
				it++;
				if (!temp.empty()) {
					temp+=t;temp+=sub_cadena(it, t, cadena.end());temp+=t;
				}else temp+=sub_cadena(it, t, cadena.end());
			} else if ((lugar=_separadores.find(*it)) != string::npos || it == cadena.end()){
				tokens.push_back(temp);
				temp.clear();
				if ((lugar=_separadores.find(*it)) != string::npos) {
					it++;
					if (it == cadena.end()) // la cadena termina con un separador
						tokens.push_back(temp);
				}
			} else temp+=*it++;
		}

		if (!temp.empty()) tokens.push_back(temp);

		return tokens;
	}

protected:
	string sub_cadena(string::iterator& it, char caracter_fin, string::iterator itfin){
		string buffer;
		while (it != itfin && *it != caracter_fin){
			if (*it == '"'){
				char tmp=*it;
				buffer+=sub_cadena(it, tmp, itfin);
			}else buffer+=*it;

			it++;
		}

		if (it != itfin && *it == caracter_fin) it++;

		return buffer;
	}

	string _separadores;
};

class GuruTokenEx:public GuruToken {
public:
	GuruTokenEx(const string& separadores = " "):GuruToken(separadores) {} ;

	size_t extraer(const string& caracteres, const string& separadores="") {
		string sep;

		sep=(!separadores.empty()) ? separadores : _separadores;

		GuruToken::extraer(caracteres, _tokens, sep);

		return _tokens.size();
	}

	string operator[] (size_t lugar) {
		if (lugar >= _tokens.size()) return string();

		return _tokens.at(lugar);
	}

	size_t tokens() { return _tokens.size(); };

	bool borrar(size_t lugar) {
		_tokens.erase(_tokens.begin() + lugar);

		return true;
	}

private:
	vector<string> _tokens;
};

#endif
