#ifndef __GURUFECHAHORA_HPP__
#define __GURUFECHAHORA_HPP__

#include <ctime>
#include <sstream>
#include <time.h>

namespace GuruFechaHora {

	template<typename T>
	inline std::string fecha_RFC1123(T fecha) {
		const char* dias_semana_1123[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
		const char* meses[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		if (fecha == -1) fecha = 0;
		struct tm *newtime = gmtime(&fecha);
		ostringstream oss;
		oss << dias_semana_1123[newtime->tm_wday] << ", " << std::setw(2) << setfill('0') << newtime->tm_mday << " " << meses[newtime->tm_mon] << " " << newtime->tm_year + 1900 << " " << std::setw(2) << setfill('0') << newtime->tm_hour << ":" << std::setw(2) << setfill('0') << newtime->tm_min << ":" << std::setw(2) << setfill('0') << newtime->tm_sec << " GMT";
		return oss.str();
	};

	template<typename T>
	inline std::string fecha_RFC850(T fecha) {
		const char* dias_semana_850[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
		const char* meses[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		if (fecha == -1) fecha = 0;
		struct tm *newtime = gmtime(&fecha);
		ostringstream oss;
		oss << dias_semana_850[newtime->tm_wday] << ", " << std::setw(2) << setfill('0') << newtime->tm_mday << "-" << meses[newtime->tm_mon] << "-" << std::setw(2) << setfill('0') << ((newtime->tm_year>100) ? (newtime->tm_year - 100) : newtime->tm_year) << " " << std::setw(2) << setfill('0') << newtime->tm_hour << ":" << std::setw(2) << setfill('0') << newtime->tm_min << ":" << std::setw(2) << setfill('0') << newtime->tm_sec << " GMT";
		return oss.str();
	};

	template<typename T>
	inline std::string fecha_asctime(T fecha) {
		if (fecha == -1) fecha = 0;
		struct tm *newtime = localtime(&fecha);
		return string(asctime(newtime));
	};

	//////////////////////////////////////////////////////////////////////
	//      Segun la rfc 1123 el formato es:
	//	    Date: Sat, 29 Oct 1994 19:43:31 GMT
	//////////////////////////////////////////////////////////////////////
	inline std::string fechaRFC1123(time_t const& fecha)
	{
		const char* dias_semana_1123[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
		const char* meses[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

		struct tm *newtime = gmtime(&fecha);

		ostringstream oss;

		oss << dias_semana_1123[newtime->tm_wday] << ", " << std::setw(2) << setfill('0') << newtime->tm_mday << " " << meses[newtime->tm_mon] << " " << newtime->tm_year + 1900 << " " << std::setw(2) << setfill('0') << newtime->tm_hour << ":" << std::setw(2) << setfill('0') << newtime->tm_min << ":" << std::setw(2) << setfill('0') << newtime->tm_sec << " GMT";

		return oss.str();
	}

	inline std::string fechaRFC1123(void)
	{
		const char* dias_semana_1123[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
		const char* meses[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

		time_t fecha;
		time(&fecha);

		struct tm *newtime = gmtime(&fecha);

		ostringstream oss;

		oss << dias_semana_1123[newtime->tm_wday] << ", " << std::setw(2) << setfill('0') << newtime->tm_mday << " " << meses[newtime->tm_mon] << " " << newtime->tm_year + 1900 << " " << std::setw(2) << setfill('0') << newtime->tm_hour << ":" << std::setw(2) << setfill('0') << newtime->tm_min << ":" << std::setw(2) << setfill('0') << newtime->tm_sec << " GMT";

		return oss.str();
	}

	// recibe un time_t UTC.
	// retorna segundos en localtime.
	inline time_t timeToLocaltime(time_t now=NULL) {
		time_t origen = NULL;

		// quiero la fecha hora actual.
		if (now == NULL)
			origen = time(NULL);
		else
			origen = time(&now);

		struct tm destino = *localtime(&origen);

		return mktime(&destino);
	}
};

#endif
