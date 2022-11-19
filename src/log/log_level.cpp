/*
 * @Author: fepo_h
 * @Date: 2022-11-19 14:36:10
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 14:36:10
 * @FilePath: /fepoh/workspace/fepoh_server/src/log/log_level.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
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
#undef XX
}
	
	
}//namespace