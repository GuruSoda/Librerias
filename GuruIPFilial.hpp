#ifndef __FILIAL_A_IP_HPP__
#define __FILIAL_A_IP_HPP__

#include <string>
#include <boost/regex.hpp>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace GuruIPFilial {

	static std::string ip(const std::string& nombre) {
		static const boost::regex ws("C\\d{3}WS\\d{2}", boost::regbase::normal | boost::regbase::icase);
		static const boost::regex cr("C\\d{3}CR\\d{2}", boost::regbase::normal | boost::regbase::icase);
		bool esWS=false, esCR=false;

		if (boost::regex_match(nombre, ws))
			esWS=true;		
		else if (boost::regex_match(nombre, cr))
			esCR=true;

		if (esWS || esCR){
			int nro_filial;
			int o2=10;

			nro_filial = atoi(nombre.substr(1,3).c_str());
			while (nro_filial > 254){
				o2++;
				nro_filial -= 254;
			}

			int o4;
			if (esWS)
				o4=atoi(nombre.substr(6,2).c_str());
			else // esCR
				o4=atoi(nombre.substr(6,2).c_str()) + 192;

			std::ostringstream oss;

			oss << "10." << o2 << "." << nro_filial << "." << o4;

			return oss.str();
		}else 
			return std::string("");
	}

	static std::string nombre(const std::string& ip) {
		static const boost::regex e("^\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b$", boost::regbase::normal | boost::regbase::icase);
	 
		if (boost::regex_match(ip, e)){
			std::string::size_type o1=ip.find('.');
			std::string::size_type o2=ip.find('.', o1+1);
			std::string::size_type o3=ip.find('.', o2+1);

			std::string str_o1=ip.substr(0, o1);
			std::string str_o2=ip.substr(o1+1, (o2-o1)-1);
			std::string str_o3=ip.substr(o2+1, (o3-o2)-1);
			std::string str_o4=ip.substr(o3+1);

			int int_o2=atoi(str_o2.c_str());
			int int_o3=atoi(str_o3.c_str());
			int nro_filial=int_o3;

			if ((str_o1.compare("10") == 0) && (int_o2 >= 10 && int_o2 <=13)) {
				while (int_o2 > 10) {
					nro_filial=int_o3+254;
					int_o3+=254;
					int_o2--;
				}
			} else if ((str_o1.compare("10") == 0) && (int_o2 >= 20 && int_o2 <=23)) {
						while (int_o2 > 20) {
							nro_filial=int_o3+254;
							int_o3+=254;
							int_o2--;
						}
			} else return ip;

			std::ostringstream oss;
			oss << "C" << std::setw(3) << std::setfill('0') << nro_filial;

			int int_o4=atoi(str_o4.c_str());
			if (int_o4 < 193)
				oss << "WS" << std::setw(2) << std::setfill('0') << str_o4;
			else
				oss << "CR" << std::setw(2) << std::setfill('0') << int_o4 - 192;

			return oss.str();
		} else
			return std::string("");
	}


	static bool esEstacion(const std::string& nombre) {
		static const boost::regex ws("C\\d{3}WS\\d{2}", boost::regbase::normal | boost::regbase::icase);
		static const boost::regex cr("C\\d{3}CR\\d{2}", boost::regbase::normal | boost::regbase::icase);

		if (boost::regex_match(nombre, ws))
			return true;
		else if (boost::regex_match(nombre, cr))
			return true;
		else 
			return false;
	}

	static bool esIP(const std::string& ip) {
		static const boost::regex e("^\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b$", boost::regbase::normal | boost::regbase::icase);
		if (boost::regex_match(ip, e)) return true;
		else return false;
	}

};

#endif
