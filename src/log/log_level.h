/*
 * @Author: fepo_h
 * @Date: 2022-11-19 14:15:06
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-22 20:01:02
 * @FilePath: /fepoh/workspace/fepoh_server/src/log/log_level.h
 * @Description: 日志级别类
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <string>
#include <iostream>

namespace fepoh{
class LogLevel{
public:
	enum Level{
		UNKOWN = 0,		//未知级别
		DEBUG,			//debug
		WARN,			//警告
		INFO,			//信息
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