#ifndef __GURUALGO_H__
#define __GURUALGO_H__

#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>

#include <GuruCompat.h>
#include <GuruToken.hpp>
#include <GuruVariable.hpp>

#if defined __GURUWIN32__
#elif defined __GURULINUX__
#include <sys/time.h>
#include <cctype>
#include <stdint.h>
#include <GuruCompat/splitpath.hpp>
#endif

namespace GuruAlgo {

	template <typename T>
	std::string segundos_a_legible_dias(T tiempo_segundos) {
		ostringstream cadena;

		if (tiempo_segundos < 60) {cadena << tiempo_segundos << " Segundos"; return cadena.str();}

		T sec,dias,horas, minutos, segundos;

		dias=tiempo_segundos/86400;
		if (dias > 0) {
			sec=tiempo_segundos-(86400*dias);
			cadena << dias << " Dias ";
		} else sec=tiempo_segundos;

		horas=sec/3600;
		minutos=(sec-(horas*3600))/60;
		segundos=sec-((horas*3600)+(minutos*60));

		cadena << std::setw(2) << std::setfill('0') << horas << ":" << std::setw(2) << std::setfill('0') << minutos << ":" << std::setw(2) << std::setfill('0') << segundos;

		return cadena.str();
	}

	/////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	inline int sacar_mes(const T& mes)
	{
		int encontrado = 0;
		static const char *los_meses[] = {"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};

		for (int i=1;i<=12;i++) {
			if (stricmp(mes.c_str(), los_meses[i-1]) == 0) {
				encontrado = i;
				break;
			}
		}

		return encontrado;
	}

	//////////////////////////////////////////////////////////////////////
	// Convierte time_t a RFC822
	// Segun la rfc 821/2 el formato es: 23 Oct 1981 11:22:33
	// Entrada: 1234567890
	// Salida: 23 Oct 1981 11:22:33
	//////////////////////////////////////////////////////////////////////
	template <typename T>
	inline std::string fechaHoraRFC822(T fecha_archivo)
	{
		static const char* meses[] = {"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
		struct tm stm;

		// En algunos sistemas de archivos remotos retorna -1.
		if (fecha_archivo < 0) fecha_archivo = 0;

		time_t fa = fecha_archivo;
		stm = *localtime((const time_t*)&fa);

		std::ostringstream oss;
		// La Fecha
		oss << std::setw(2) << std::setfill('0') <<  stm.tm_mday << " " << meses[stm.tm_mon] << " " << std::setw(4) << std::setfill('0') << stm.tm_year + 1900 << " ";
		// La Hora
		oss << std::setw(2) << std::setfill('0') << stm.tm_hour << ":" << std::setw(2) << std::setfill('0') << stm.tm_min << ":" << std::setw(2) << std::setfill('0') << stm.tm_sec;

		return oss.str();
	}

	//////////////////////////////////////////////////////////////////////
	// Convierte RFC822 a time_t
	// Entrada: 04 OCT 2001 10:52:06
	// Saliad: 1234567890
	//////////////////////////////////////////////////////////////////////
	template <typename T>
	inline time_t RFC822FechaHora(const T& fecha)
	{
		GuruToken			tk(" ");
		std::vector<string>	tokens;
		time_t				fecha_archivo = 0;

		uint16_t cant_param = static_cast<uint16_t>(tk.extraer(fecha, tokens).size());

		struct tm st;
		memset(&st, 0, sizeof(struct tm));
		st.tm_isdst = -1;
		if (cant_param == 4){  // 04(1) OCT(2) 2001(3) 10:52:06(4)
			char temp[10];
			st.tm_year		= atoi(tokens[2].c_str()) - 1900;
			st.tm_mon		= sacar_mes(tokens[1]) - 1;
			st.tm_mday		= atoi(tokens[0].c_str());

			st.tm_hour		= atoi(strncpy(temp,&tokens[3].c_str()[0],2));
			st.tm_min		= atoi(strncpy(temp,&tokens[3].c_str()[3],2));
			st.tm_sec		= atoi(strncpy(temp,&tokens[3].c_str()[6],2));

			fecha_archivo = (time_t)mktime(&st);
		}

		return fecha_archivo;
	}

	//////////////////////////////////////////////////////////////////////
	// Las variables en mayusculas H-M-S-AA-MM-DD se rellenan con 0
	// Las variables en minusculas h-m-s-aa-mm-dd NO se rellenan con 0
	//////////////////////////////////////////////////////////////////////
	class reemplazoFecha {
	public:
		reemplazoFecha(time_t fecha=0):fecha(fecha) {};

		string reemplazo(const string& variable) {
			if (fecha == 0)
				time(&fecha);

			struct tm *ahora = localtime(&fecha);

			ostringstream oss;

			if (variable.compare("H") == 0)
				oss << std::setw(2) << std::setfill('0') << ahora->tm_hour;
			else if (variable.compare("M") == 0)
				oss << std::setw(2) << std::setfill('0') << ahora->tm_min;
			else if (variable.compare("S") == 0)
				oss << std::setw(2) << std::setfill('0') << ahora->tm_sec;
			else if (variable.compare("AA") == 0)
				oss << std::setw(2) << std::setfill('0') << ahora->tm_year+1900;
			else if (variable.compare("MM") == 0)
				oss << std::setw(2) << std::setfill('0') << ahora->tm_mon+1;
			else if (variable.compare("DD") == 0)
				oss << std::setw(2) << std::setfill('0') << ahora->tm_mday;
			else if (variable.compare("h") == 0)
				oss << ahora->tm_hour;
			else if (variable.compare("m") == 0)
				oss << ahora->tm_min;
			else if (variable.compare("s") == 0)
				oss << ahora->tm_sec;
			else if (variable.compare("aa") == 0)
				oss << ahora->tm_year;
			else if (variable.compare("mm") == 0)
				oss << ahora->tm_mon+1;
			else if (variable.compare("dd") == 0)
				oss << ahora->tm_mday;

			return oss.str();
		}

		void cargar_fecha(time_t fecha) {this->fecha=fecha;}

	private:
		time_t fecha;
	};

	//////////////////////////////////////////////////////////////////////
	// Modo de uso:
	// cout << armarFecha("%H%:%M%:%S%") << endl;
	//////////////////////////////////////////////////////////////////////
	template <typename T>
	string armarFecha(const T& cadena, time_t epoc=0) {
		reemplazoFecha a;

		if (epoc != 0) 
			a.cargar_fecha(epoc);

		GuruVariable<reemplazoFecha> var(&a, &reemplazoFecha::reemplazo);

		return var.reemplazar(cadena);
	}

	enum {fecha_anio=0x01, 
		  fecha_mes=0x02,
		  fecha_dia=0x04,
		  fecha_hora=0x08,
		  fecha_minuto=0x10,
		  fecha_segundo=0x20
	};

	template <typename T>
	T& fecha_actual(T& fecha, uint32_t opciones=GuruAlgo::fecha_anio | GuruAlgo::fecha_mes | GuruAlgo::fecha_dia) {
		time_t		fecha_t=time(NULL);
		struct tm* ahora = localtime(&fecha_t);
		ostringstream oss;

		fecha.clear();

		if (opciones & GuruAlgo::fecha_anio) oss << std::setw(2) << std::setfill('0') << ahora->tm_year+1900;
		if (opciones & GuruAlgo::fecha_mes) oss << std::setw(2) << std::setfill('0') << ahora->tm_mon+1;
		if (opciones & GuruAlgo::fecha_dia) oss << std::setw(2) << std::setfill('0') << ahora->tm_mday;

		if (opciones & GuruAlgo::fecha_hora) oss << std::setw(2) << std::setfill('0') << ahora->tm_hour;
		if (opciones & GuruAlgo::fecha_minuto) oss << std::setw(2) << std::setfill('0') << ahora->tm_min;
		if (opciones & GuruAlgo::fecha_segundo) oss << std::setw(2) << std::setfill('0') << ahora->tm_sec;

		return fecha = oss.str();
	};

	static string fecha_actual(uint32_t opciones=GuruAlgo::fecha_anio | GuruAlgo::fecha_mes | GuruAlgo::fecha_dia | GuruAlgo::fecha_hora | GuruAlgo::fecha_minuto | GuruAlgo::fecha_segundo) {
		time_t		fecha_t=time(NULL);
		struct tm*	ahora = localtime(&fecha_t);
		ostringstream oss;

		if (opciones & GuruAlgo::fecha_anio) oss << std::setw(2) << std::setfill('0') << ahora->tm_year+1900;
		if (opciones & GuruAlgo::fecha_mes) oss << std::setw(2) << std::setfill('0') << ahora->tm_mon+1;
		if (opciones & GuruAlgo::fecha_dia) oss << std::setw(2) << std::setfill('0') << ahora->tm_mday;

		if (opciones & GuruAlgo::fecha_hora) oss << std::setw(2) << std::setfill('0') << ahora->tm_hour;
		if (opciones & GuruAlgo::fecha_minuto) oss << std::setw(2) << std::setfill('0') << ahora->tm_min;
		if (opciones & GuruAlgo::fecha_segundo) oss << std::setw(2) << std::setfill('0') << ahora->tm_sec;

		return oss.str();
	};

	static string fecha_actual(time_t fecha, uint32_t opciones = GuruAlgo::fecha_anio | GuruAlgo::fecha_mes | GuruAlgo::fecha_dia | GuruAlgo::fecha_hora | GuruAlgo::fecha_minuto | GuruAlgo::fecha_segundo) {
		struct tm*	ahora = localtime(&fecha);
		ostringstream oss;

		if (opciones & GuruAlgo::fecha_anio) oss << std::setw(2) << std::setfill('0') << ahora->tm_year + 1900;
		if (opciones & GuruAlgo::fecha_mes) oss << std::setw(2) << std::setfill('0') << ahora->tm_mon + 1;
		if (opciones & GuruAlgo::fecha_dia) oss << std::setw(2) << std::setfill('0') << ahora->tm_mday;

		if (opciones & GuruAlgo::fecha_hora) oss << std::setw(2) << std::setfill('0') << ahora->tm_hour;
		if (opciones & GuruAlgo::fecha_minuto) oss << std::setw(2) << std::setfill('0') << ahora->tm_min;
		if (opciones & GuruAlgo::fecha_segundo) oss << std::setw(2) << std::setfill('0') << ahora->tm_sec;

		return oss.str();
	};

	#define GURUSEGUNDOS(x, y) { std::clock_t inicio = std::clock();	x; y = (std::clock() - inicio) / (double)CLOCKS_PER_SEC; }

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Para medir el tiempo en intervalos cortos no mayor a 36 Minutos.
	// Los resultados son en segundos.
	// Utiliza la funcion clock.
	//
	// NOTA Mental: 
	//		The value returned by clock() may wrap around on some implementations.
	//		For example, on a machine with 32-bit std::clock_t, it wraps after 2147 seconds or 36 minutes.
	class GuruTiempos {
	public:
		GuruTiempos() { iniciar(); }
		virtual ~GuruTiempos() {}

		void   iniciar() { _inicio = std::clock(); }
		double detener() { return _total = (std::clock() - _inicio) / (double)CLOCKS_PER_SEC; }

		double ahora() { return (std::clock() - _inicio) / (double)CLOCKS_PER_SEC; }

		double segundos() { return _total; }

	private:
		std::clock_t _inicio;
		double _total;
	};

	//////////////////////////////////////////////////////
	// Para calcular tiempos desde milisegundos hasta lo que de en 64bits.
	// Utiliza la funcion gettimeofday/GetSystemTime.
	//
	// Nota Mental:
	//		Se puede usar int clock_gettime(clockid_t clock_id, struct timespec *tp); // con clock_it CLOCK_PROCESS_CPUTIME_ID o CLOCK_REALTIME
	//	o
	//		int gettimeofday(struct timeval *tv, struct timezone *tz);
	class GuruTiemposEX {
	public:
		GuruTiemposEX() { iniciar(); }
		virtual ~GuruTiemposEX() {}

		void iniciar() {			
		#if defined __GURULINUX__
			gettimeofday(&handle, NULL);
			_inicio = (handle.tv_sec * 1000) + (handle.tv_usec / 1000);
		#elif defined __GURUWIN32__
			GetSystemTime(&handle);
			_inicio = (handle.wSecond * 1000) + handle.wMilliseconds;
		#endif
		}

		uint64_t detener() { 
		#if defined __GURULINUX__
			gettimeofday(&handle, NULL);
			_total = ((handle.tv_sec * 1000) + (handle.tv_usec / 1000)) - _inicio;
		#elif defined __GURUWIN32__
			GetSystemTime(&handle);
			_total = ((handle.wSecond * 1000) + handle.wMilliseconds) - _inicio;
		#endif
			return _total; 
		}

		double ahora() { 
		#if defined __GURULINUX__
			gettimeofday(&handle, NULL);
			return (handle.tv_sec * 1000) + (handle.tv_usec / 1000);
		#elif defined __GURUWIN32__
			GetSystemTime(&handle);
			return (handle.wSecond * 1000) + handle.wMilliseconds;
		#endif
		}

		uint64_t milisegundos() { return _total; }

//		double segundos() { return (((double)_total/(double)1000)*100+0.5)/100; }

		double segundos() { return (double)_total/(double)1000; }

		double minutos() { return ((double)_total / (double)1000)/(double)60; }

		string humano() { return string(); }

	private:
	#if defined __GURULINUX__
		struct timeval handle;
	#elif defined __GURUWIN32__
		SYSTEMTIME handle;
	#endif
		uint64_t	_inicio;	// en milisegundos.
		uint64_t	_total;		// en milisegundos.
	};

// Ejemplo de comparacion case insensitive
/*
	// templated version of my_equal so it could work with both char and wchar_t
	template<typename charT>
	struct my_equal {
		my_equal(const std::locale& loc) : loc_(loc) {}
		bool operator()(charT ch1, charT ch2) {
			return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
		}
	private:
		const std::locale& loc_;
	};

	// find substring (case insensitive)
	template<typename T>
	int ci_find_substr(const T& str1, const T& str2, const std::locale& loc = std::locale())
	{
		typename T::const_iterator it = std::search(str1.begin(), str1.end(),
			str2.begin(), str2.end(), my_equal<typename T::value_type>(loc));
		if (it != str1.end()) return it - str1.begin();
		else return -1; // not found
	}

	int main(int arc, char *argv[])
	{
		// string test
		std::string str1 = "FIRST HELLO";
		std::string str2 = "hello";
		int f1 = ci_find_substr(str1, str2);

		// wstring test
		std::wstring wstr1 = L"????? ??????";
		std::wstring wstr2 = L"??????";
		int f2 = ci_find_substr(wstr1, wstr2);

		return 0;
	}
*/
}

#endif
