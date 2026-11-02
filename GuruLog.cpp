#include "GuruLog.h"

#if !defined GURULOG_CPP
	#error Para poder incluir GuruLog.cpp en su proyecto por favor definir GURULOG_CPP
#endif

namespace GuruLog {
	GuruLog::nivel _nivel = GuruLog::level_none;
}
