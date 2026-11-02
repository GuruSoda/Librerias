#if !defined __GURUHTTPCONFIG_H__
#define	__GURUHTTPCONFIG_H__

#include <GuruCompat.h>

#if defined __GURUWIN32__
//////////////////////////////////////////////////////////////////////
//	Link: Ws2_32.lib
//////////////////////////////////////////////////////////////////////
// Para compilar Windows.h y Winsock2.h en el mismo fuente
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef _WINSOCKAPI_
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
	#endif

	#include <windows.h>
	#include <winsock2.h>
#elif defined __GURULINUX__
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <unistd.h>
#endif

#include <map>
#include <string>
#include <vector>

#include <GuruToken.hpp>
#include <GuruAlgo.hpp>
#include <GuruDirectorio.hpp>

#include <GuruHttp/GuruhttpInterface.hpp>
#include <GuruHttp/Guruhttp_algo.hpp>

using namespace std;

class GuruhttpInterface;

template <typename T>
T& cargar_direcciones_ip(const string& direccion, T& contenedor) {
	contenedor.clear();
	struct hostent*		hosts;
	::uint32_t			ip=0;

	// me fijo si es un nombre
	if ((hosts = gethostbyname(direccion.c_str()))) {

		if (!GuruStringAlgo::stringequals(direccion, string(hosts->h_name)))
			cargar_direcciones_ip(string(hosts->h_name), contenedor);

		while (*hosts->h_addr_list) {
			memcpy((char *)&ip, *hosts->h_addr_list, hosts->h_length);
			contenedor.push_back(ip);

			// solo para verificar la IP adquirida y mostrarla por pantalla
			// struct sockaddr_in adr_inet;
			// memcpy((char *)&adr_inet.sin_addr, *hosts->h_addr_list, hosts->h_length);
			// cout << inet_ntoa(adr_inet.sin_addr) << endl;

			hosts->h_addr_list++;
		}
	} else
		// Verificar si es una notacion de numeros y puntos
		if ((ip = inet_addr (direccion.c_str())) != INADDR_NONE)
			contenedor.push_back(ip);
//			else throw GuruSocketException();

	return contenedor;
}

///////////////////////////////////////
// Configuracion de instancia http
///////////////////////////////////////
class GuruhttpConf {
public:
	enum {	simple_http=0x01,
			cookie=0x02
	};

	GuruhttpConf():_puerto(80), _velocidad(0), _opciones(simple_http) {};

	void puerto(uint16_t puerto){this->_puerto=puerto;}
	inline uint16_t puerto(){return this->_puerto;}

	void interfaz(const string& interfaz) {this->_interfaz=interfaz;}
	inline string& interfaz() {return this->_interfaz;}

	void agregar_direccion_rechazar(const string& direccion) {
		vector<uint32_t> tmp;

		if (cargar_direcciones_ip(direccion, tmp).size() != 0)
			_ip_rechazadas.insert(_ip_rechazadas.end(), tmp.begin(), tmp.end());
	}

	inline bool rechazada(const uint32_t& direccion) {return false;}

	void agregar_direccion_aceptar(const string& direccion) {
			vector<uint32_t> tmp;

		if (cargar_direcciones_ip(direccion, tmp).size() != 0)
			_ip_aceptadas.insert(_ip_aceptadas.end(), tmp.begin(), tmp.end());
	}

	inline bool aceptada() {return true;}

	void sitios(const string& dir) {_directorio_sitios=GuruDirectorio::comoDirectorio(dir);}
	const string& sitios() {return _directorio_sitios;}

	void nombre(const string& seccion_conf) {_nombre_conf=seccion_conf;};
	const string& nombre() {return _nombre_conf;};

	virtual void ponerVelocidad(::uint32_t t) {_velocidad=t;}
	virtual ::uint32_t sacarVelocidad() {return _velocidad;}

	void log_error(const string& nombre) {_log_error=nombre;};
	const string& log_error() {return _log_error;};

	void opciones(const string& string_opciones) {
		GuruTokenEx tok;

		size_t total=tok.extraer(string_opciones);

		for (size_t i=0;i<total;i++) {
			if (GuruStringAlgo::stringequals(tok[i], string("simple"))) _opciones |= GuruhttpConf::simple_http;

			if (GuruStringAlgo::stringequals(tok[i], string("cookie"))) _opciones |= GuruhttpConf::cookie;
		}
	}

	::uint32_t opciones() {
		return _opciones;
	}

private:
	::uint16_t			_puerto;
	string				_interfaz;
	vector< ::uint32_t>	_ip_rechazadas;
	vector< ::uint32_t>	_ip_aceptadas;
	string				_directorio_sitios;
	string				_nombre_conf;
	::uint32_t			_velocidad;
	::uint32_t			_opciones;
	string				_log_error;
};

////////////////////////////////
// Configuracion del sitio
////////////////////////////////
class GuruhttpDirVirtual
{
public:
	GuruhttpDirVirtual() {
		_listar = false;
		_velocidad = 0;
		sitioweb = NULL;
		_id = 0;
		_ptr = NULL;
	};

	virtual void dirApps(const string& directorio) {
		this->dir_apps=GuruDirectorio::comoDirectorio(directorio);
	};

	virtual string& dirApps() {return dir_apps;};

	virtual void agregarLog(const string& archivo, const string& formato) {
		this->logs.insert(make_pair(archivo, formato));
	};
	virtual map<string, string>& sacarLog() {return logs;};

	virtual void sitio(const string& sitio) {
		if (sitio.empty()) return;

		dir_virt.clear();

		if (sitio.at(0) != '/') dir_virt += '/';

		dir_virt += sitio;

		if (*sitio.rbegin() != '/') dir_virt += '/';
	};

	virtual string& sitio() {return dir_virt;};

	virtual void interfaz(GuruhttpInterface* sitio) {this->sitioweb=sitio;};
	virtual GuruhttpInterface* interfaz() {return sitioweb;};

	virtual void agregarHost(const vector<string>& hosts) {this->hosts = hosts;};
	virtual void agregarHost(const string& host) {
		if (find(this->hosts.begin(), this->hosts.end(), host) == this->hosts.end())
			this->hosts.push_back(host);
	};
	virtual vector<string>& sacarHost() {return hosts;};

	virtual void agregarAtributosExtra(const string& atributo, const string& valor) {
		_atributos_extra.insert(make_pair(atributo, valor));
	};

	virtual void borrarAtributosExtra(const string& atributo) {
		map<string, string>::iterator it = _atributos_extra.find(atributo);

		if (it != _atributos_extra.end())
			_atributos_extra.erase(it);
	};

	virtual map<string, string>& sacarAtributosExtra() { return _atributos_extra; }

	virtual bool listar() {return _listar;};
	virtual void listar(bool listado) {this->_listar=listado;};

	virtual void agregarBusqueda(const vector<string>& archivos) {this->busqueda=archivos;};
	virtual void agregarBusqueda(const string& busqueda) {
		if (find(this->busqueda.begin(), this->busqueda.end(), busqueda) == this->busqueda.end())
			this->busqueda.push_back(busqueda);
	};

	virtual vector<string>& sacarBusqueda() {return busqueda;};

	virtual void velocidad(::uint32_t t) {_velocidad=t;}
	virtual ::uint32_t velocidad() {return _velocidad;}

	virtual void id(::uint32_t t) {_id=t;}
	virtual ::uint32_t id() {return _id;}

	virtual void extra_ptr(void* a) {_ptr=a;};
	virtual void* extra_ptr() {return _ptr;};

	virtual string env(const std::string& var) { 
		std::map<std::string, std::string>::iterator it = _env.find(var);

		return (it != _env.end()) ? it->second : string();
	}

	void env(const string& clave, const string& valor) {
		_env.insert(make_pair(clave, valor));
	}

	std::map<std::string, std::string>& env() { return _env; }

	void lib(const string& lib) { _libreria = lib; }
	string lib() { return _libreria; }

	void intefaz_lib(const string& interlib) { _libreria_interfaz = interlib; }
	string intefaz_lib() { return _libreria_interfaz; }

//////////////////////////////////////////////////////////////////////
// Para tener un sitio dentro de un zip
//////////////////////////////////////////////////////////////////////
	virtual void sitioZip(const string& archivo) { archivo_sitio = archivo; };
	virtual string& sitioZip() { return archivo_sitio; };
	virtual bool enZip() { return archivo_sitio.empty(); };

private:
	string				dir_virt;
	string				dir_apps;
	bool				_listar;
	::uint32_t			_velocidad;
	GuruhttpInterface*	sitioweb;
	vector<string>		hosts;
	vector<string>		busqueda;
	map<string, string>	logs;
	::uint32_t			_id;
	void*				_ptr;
	map<string, string>	_env;
	string				_libreria;
	string				_libreria_interfaz;
	map<string, string>	_atributos_extra;

//////////////////////////////////////////////////////////////////////
// Para tener un sitio dentro de un zip
//////////////////////////////////////////////////////////////////////
	string				archivo_sitio;
};

//////////////////////////////////////
// Configuracion de instancias de sitios.
//////////////////////////////////////
class GuruhttpConfig
{
public:
/*
	void agregar(GuruhttpInterface* dir, const string& url, const string& host = "*", const string& directorio="") {
		if (dir == NULL) return ;

		if (url.empty()) return ;

		GuruhttpDirVirtual conf;

		conf.ponerSitio(url);
		conf.ponerInterfaz(dir);
		conf.agregarHost(host);
		conf.ponerDirApps(directorio);

		map <string, vector<pair<string, GuruhttpDirVirtual> > >::iterator it(sitios.find(host));
		if (it == sitios.end()) {
			vector<pair<string, GuruhttpDirVirtual> > sitio;
			sitio.push_back(make_pair(url, conf));
			sitios.insert(make_pair(host, sitio));
		} else {
			it->second.push_back(make_pair(url, conf));
		}
	}
*/
/*
	void agregar(GuruhttpInterface* dir, const string& url, const vector<string>& host, const string& directorio="") {
		if (dir == NULL || url.empty()) return ;

		GuruhttpDirVirtual conf;

		conf.ponerSitio(url);
		conf.ponerInterfaz(dir);
		conf.ponerDirApps(directorio);

		for (vector<string>::const_iterator it0=host.begin();it0!=host.end();++it0) {
			map <string, vector<pair<string, GuruhttpDirVirtual> > >::iterator it(sitios.find(*it0));
			if (it == sitios.end()) {
				vector<pair<string, GuruhttpDirVirtual> > sitio;
				conf.agregarHost(*it0);
				sitio.push_back(make_pair(url, conf));
				sitios.insert(make_pair(*it0, sitio));
			} else {
				it->second.push_back(make_pair(url, conf));
			}
		}
	}
*/
	void agregar(const GuruhttpDirVirtual& conf) {
		GuruhttpDirVirtual& conf2 = const_cast<GuruhttpDirVirtual&>(conf);

		for (vector<string>::const_iterator it0=conf2.sacarHost().begin();it0!=conf2.sacarHost().end();++it0) {
			map <string, vector<pair<string, GuruhttpDirVirtual> > >::iterator it(sitios.find(*it0));
			if (it == sitios.end()) {
				vector<pair<string, GuruhttpDirVirtual> > sitio;
				sitio.push_back(make_pair(conf2.sitio(), conf2));
				sitios.insert(make_pair(*it0, sitio));
			} else {
				it->second.push_back(make_pair(conf2.sitio(), conf2));
			}
		}
	}

/*
	bool existe(const string& sitio, const string& host="*"){
		if (GuruhttpConfig::sacarInterfaz(sitio, host) == NULL) 
			return false;
		else 
			return true;
	}
*/

	GuruhttpDirVirtual*		sacarConfiguracion(const string& sitio="/", const string& host="*") {
		string nombre_host(host);

		string::size_type lugar=host.find(':');
		if (lugar != string::npos) nombre_host=host.substr(0, lugar);

		map <string, vector<pair<string, GuruhttpDirVirtual> > >::iterator it(sitios.find(nombre_host));

		// si no existe el nombre de host, lo busco si existe para todos los hosts.
		if (it == sitios.end()) {
			it=sitios.find("*");
			if (it == sitios.end())
				return NULL;
		}

		for (vector<pair<string, GuruhttpDirVirtual> >::iterator it2=it->second.begin();it2!=it->second.end();++it2) {
			if (GuruStringAlgo::stringequals(it2->first, sitio))
				return (GuruhttpDirVirtual*)&it2->second;
		}

		// si llega es por que el sitio no existe en el host especificado, busco si figura en todos los sitios ( * )
		if (host.compare("*") != 0)
			return sacarConfiguracion(sitio, "*");
		else
			return NULL;
	}

/*
	GuruhttpInterface*		sacarInterfaz(const string& sitio="/", const string& host="*") {
		string nombre_host(host);

		string::size_type lugar=host.find(':');
		if (lugar != string::npos) nombre_host=host.substr(0, lugar);

		map <string, vector<pair<string, GuruhttpDirVirtual> > >::iterator it(sitios.find(nombre_host));

		// si no existe el nombre de host, lo busco si existe para todos los hosts.
		if (it == sitios.end()) {
			it=sitios.find("*");
			if (it == sitios.end())
				return NULL;
		}

		for (vector<pair<string, GuruhttpDirVirtual> >::iterator it2=it->second.begin();it2!=it->second.end();++it2) {
			if (GuruStringAlgo::stringequals(it2->first, sitio))
				return it2->second.sacarInterfaz();
		}

		// si llega es por que el sitio no existe en el host especificado, busco si figura en todos los sitios ( * )
		if (host.compare("*") != 0)
			return sacarInterfaz(sitio, "*");
		else
			return NULL;
	}
*/
/*
	GuruhttpConf*			sacarConfHttp(const string& nombre) {
		map<string, GuruhttpConf>::iterator lugar;

		if ((lugar=confhttp.find(nombre)) != confhttp.end())
			return &lugar->second;
		else
			return NULL;
	}
*/

	void			agregarConfHttp(const string& nombre, GuruhttpConf& conf) {
		confhttp.insert(make_pair(nombre, conf));
	}

/*
	void nuevo_sitio(GuruhttpInterface* dir, const string& url, const string& host = "*", const string& directorio=""){ 
		agregar(dir, GuruhttpAlgo::comoDirectorioHTTP(url), host, directorio);
	};
*/
	// falta agregar chequeos de cada atributo, hay que hacerlo aca.
	void nuevo_sitio(const GuruhttpDirVirtual& conf){ 
		agregar(conf);
	};
/*
	GuruhttpInterface* sitio(const string& url, const string& host = "*", const string& directorio=""){ 
		return sacarInterfaz(url, host);
	};
*/

	GuruhttpDirVirtual* configuracion(const string& url, const string& host = "*", const string& directorio=""){
		return sacarConfiguracion(url, host);
	};

	vector<GuruhttpConf*>& configuraciones(vector<GuruhttpConf*>& contenedor) {
		contenedor.clear();

		for (map<string, GuruhttpConf>::iterator it=confhttp.begin();it!=confhttp.end();++it) {
			contenedor.push_back(&it->second);
		}

		return contenedor;
	}
public:
	GuruhttpConfig() {};
	virtual ~GuruhttpConfig() {};

	void clear() {
		sitios.clear();
		confhttp.clear();
	}

	map <string, vector<pair<string, GuruhttpDirVirtual> > >& map_sitios() {
		return sitios;
	}

private:
	// configuracion de cada instancia http.
	map<string, GuruhttpConf>	confhttp;

	// configuracion de cada sitio, que host tiene y con que urls.
	map <string, vector<pair<string, GuruhttpDirVirtual> > >	sitios;		// llave:host valor: vector de url,sitio
};

#endif
