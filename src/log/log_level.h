#pragma once

#include <string>
#include <iostream>

/*
	日志级别类
*/

namespace fepoh{
	class LogLevel{
		public:
			enum Level{
				UNKOWN = 0,		//未知级别
				DEBUG,			//debug
				INFO,			//信息
				WARN,			//警告
				ERROR,			//错误
				FATAL			//致命错误
			};
		
		public:
			//日志级别转字符串
			static std::string ToString(LogLevel::Level level);
			//字符串转日志级别
			static LogLevel::Level FromString(const std::string& level);
	};
	
}//namespace