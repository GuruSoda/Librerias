#ifndef __GURUREGEXFUNCIONES_HPP__
#define __GURUREGEXFUNCIONES_HPP__

#if __cplusplus <= 199711L
        #include <boost/regex.hpp>
        using namespace boost;
        #define FILTRO boost::regbase::icase
        #define REGBOOST
#else
        #include <regex>
        using namespace std;
        #define FILTRO std::regex::icase
        #define REGSTD
#endif

class GuruRegExFunciones: public GuruInterprete::GuruFunciones<GuruRegExFunciones> {
public:
	GuruRegExFunciones () {
		cargar_funcion(this, &GuruRegExFunciones::match, "regex.match");
	};

	virtual ~GuruRegExFunciones() {
	};

	string match(const vector<string>& param) {
		if (param.size() != 2) return string();

#if defined REGSTD
		std::regex er(param.at(1), FILTRO);
		return (std::regex_match(param.at(0), er)) ? string("1") : string();
#elif defined REGBOOST
		boost::regex er(param.at(1), FILTRO);
		return (boost::regex_match(param.at(0), er)) ? string("1") : string();
#endif
	}
};

#endif
