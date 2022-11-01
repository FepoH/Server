#include "log_level.h"

#include <algorithm>

namespace fepoh{
	
	std::string LogLevel::ToString(LogLevel::Level level){
		switch(level){
#define XX(var) \
		case var: \
			return #var; \
			break;
			
			XX(UNKOWN);
			XX(DEBUG);
			XX(INFO);
			XX(WARN);
			XX(ERROR);
			XX(FATAL);
#undef XX
			default:
				return "UNKOWN";
		}
		return "UNKOWN";
	}
	
	LogLevel::Level LogLevel::FromString(const std::string& level){
	//也可采用strcasecmp
		std::string tmp = level;
		std::transform(level.begin(),level.end(),tmp.begin(),::toupper);
#define XX(str,l) \
		if(tmp == str){ \
			return l; \
		}
		XX("UNKOWN",UNKOWN)
		XX("DEBUG",DEBUG)
		XX("INFO",INFO)
		XX("WARN",WARN)
		XX("ERROR",ERROR)
		XX("FATAL",FATAL)
		return UNKOWN;
	}
	
	
}//namespace