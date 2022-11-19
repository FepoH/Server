#include "log_formatter.h"
#include "logger.h"

#include <functional>

#include <time.h>
#include <vector>


namespace fepoh{
/**
 * @description: tab组件
 * @return {*}
 */
class TabItem : public LogFormatter::FormatItem{
	public:
		TabItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << "\t";
		}
};
/**
 * @description: 换行组件
 * @return {*}
 */
class NewLineItem : public LogFormatter::FormatItem{
	public:
		NewLineItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << std::endl;
		}
};
/**
 * @description: 文件名组件
 * @return {*}
 */
class FileNameItem : public LogFormatter::FormatItem{
	public:
		FileNameItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << event->getFilePath();
		}
};
/**
 * @description: 文件行号组件
 * @return {*}
 */
class LineNumItem : public LogFormatter::FormatItem{
	public:
		LineNumItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << event->getLine();
			
		}
};
/**
 * @description: 附加内容组件
 * @return {*}
 */
class AddContentItem : public LogFormatter::FormatItem{
	public:
		AddContentItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << m_addContent;
		}
};
/**
 * @description: 已过时间
 * @return {*}
 */
class ElapseItem : public LogFormatter::FormatItem{
	public:
		ElapseItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << event->getElapse();
		}
};
/**
 * @description: 线程id组件
 * @return {*}
 */
class ThreadIdItem : public LogFormatter::FormatItem{
	public:
		ThreadIdItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << event->getThreadId();
		}
};
/**
 * @description: 协程id组件
 * @return {*}
 */
class FiberIdItem :public LogFormatter::FormatItem{
	public:
		FiberIdItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << event->getFiberId();
		}
};
/**
 * @description: 日期组件
 * @return {*}
 */
class DataTimeItem : public LogFormatter::FormatItem{
	public:
		DataTimeItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			const time_t iTime = event->getTime();
			char buf[128];
			std::string pattern = (m_addContent == "")?"%Y-%m-%d %H:%M:%S" : m_addContent;
			strftime(buf,128,pattern.c_str(),localtime(&iTime));
			out << buf;
		}
};
/**
 * @description: 线程名组件
 * @return {*}
 */
class ThreadNameItem : public LogFormatter::FormatItem{
	public:
		ThreadNameItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << event->getThreadName();
		}
};
/**
 * @description: 日志内容组件
 * @return {*}
 */
class LogContentItem : public LogFormatter::FormatItem{
	public:
		LogContentItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << event->getSS().str();
		}
};
/**
 * @description: 日志级别组件
 * @return {*}
 */
class LogLevelItem : public LogFormatter::FormatItem{
	public:
		LogLevelItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << LogLevel::ToString(level);
		}
};
/**
 * @description: 日志器名组件
 * @return {*}
 */
class LoggerNameItem : public LogFormatter::FormatItem{
	public:
		LoggerNameItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << logger->getName();
		}
};
/**
 * @description: 函数名组件
 * @return {*}
 */
class FuncNameItem : public LogFormatter::FormatItem{
	public:
		FuncNameItem(const std::string& addContent = ""):FormatItem(addContent){}
		void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override{
			out << event->getFuncName();
		}
};

LogFormatter::LogFormatter(const std::string& pattern):m_pattern(pattern),m_isInit(false){
	init();
}

void LogFormatter::format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger){
	if(m_isInit){
		MutexLock locker(m_mutex);
		for(auto& item:m_items){
			item->format(out,event,level,logger);
		}
	}else{
		std::cout << "LogFormatter init fail." << std::endl;	
	}
}

/**
 * @description: 解析%d	%d%dd	%d{afd},这三种格式,附加内容为多个字符必须使用{}
 * @return {*}
 */
void LogFormatter::init(){
	bool flag = false;
	std::string addContent = "";
	std::vector<std::pair<char,std::string>> vctChStr; 
	for(size_t i=1;i<m_pattern.size();++i){
		//必须以'%'开头
		if(!(std::isalpha(m_pattern[i])&&m_pattern[i-1] =='%')){
			continue;
		}
		char resChar = m_pattern[i];
		size_t n=i+1;
		if(n < m_pattern.size()){
			addContent = "";
			if(m_pattern[n] == '%'){
				vctChStr.push_back(std::make_pair(resChar,addContent));
				i = n;
				continue;
			}
			if(m_pattern[n] == '{'){
				int index = m_pattern.find('}',n);
				if(index ==-1){
					i = n;
					break;
				}
				i = index;
				addContent = m_pattern.substr(n+1,index-n-1);
				vctChStr.push_back(std::make_pair(resChar,addContent));
			}else{
				if(n+1<m_pattern.size()&&m_pattern[n+1]=='%'){
					addContent = m_pattern.substr(n,1);
					vctChStr.push_back(std::make_pair(resChar,""));
					vctChStr.push_back(std::make_pair('S',addContent));
					i = n+1;
				}else{
					continue;
				}
			}
		}
		if(n == m_pattern.size()){
			vctChStr.push_back(std::make_pair(resChar,""));
		}
	}
	static std::map<char, std::function<FormatItem::ptr(const std::string& addContent)> > s_format_items = {
#define XX(ch, C) \
	{ch, [](const std::string& str) { return FormatItem::ptr(new C(str));}}

		XX('m', LogContentItem),        //m:消息
		XX('p', LogLevelItem),          //p:日志级别
		XX('r', ElapseItem),            //r:累计毫秒数
		XX('c', LoggerNameItem),        //c:日志名称
		XX('t', ThreadIdItem),          //t:线程id
		XX('n', NewLineItem),           //n:换行
		XX('d', DataTimeItem),          //d:时间
		XX('f', FileNameItem),          //f:文件名
		XX('l', LineNumItem),           //l:行号
		XX('T', TabItem),               //T:Tab
		XX('F', FiberIdItem),           //F:协程id
		XX('N', ThreadNameItem),        //N:线程名称
		XX('S', AddContentItem),		//添加字符内容
		XX('i', FuncNameItem),			//添加字符内容
#undef XX
	};
	if(!vctChStr.empty()){
		for(auto& i:vctChStr){
			try{
				m_items.push_back(s_format_items[i.first](i.second));
				
			}catch(...){
				std::string s;
				s = " ";
				s[0] = i.first;
				m_items.push_back(s_format_items['S']("<Pattern error:"+ s + ">"));
			}
			m_isInit = true;
		}
	}else{
		m_isInit = false;
	}
}

	
}//namespace