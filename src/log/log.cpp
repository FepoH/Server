#include "log.h"
#include "config.h"
#include "log/log_formatter.h"

#include <string>

namespace fepoh{


static std::string LogDefineDump(const LogDefine& ld){
    Json js(ld);
    return js.dump();
}

template<>
class LexicalCast<std::string,LogDefine>{
public:
    LogDefine operator()(const std::string& str){
        Json root = Json::parse(str);
        LogDefine ld;
        //无日志器名称或无输出地,直接忽略
        if((!root.contains("name"))||(!root.contains("appenders"))){
            return ld;
        }
        Json appenders = root["appenders"];
        for(auto& item:appenders){
            LogAppenderDefine apd;
            if(item.contains("type")){
                if(item["type"].get<int>() == 0){
                    apd.type = 0;
                }else if(item["type"].get<int>() == 1){
                    //文件输出地无路径直接忽略
                    if(!item.contains("filepath")){
                        continue;
                    }
                    apd.type = 1;
                    apd.filepath = item["filepath"].get<std::string>();
                }else{
                    continue;
                }
            }else{
                continue;
            }
            apd.level = item.contains("level") ? LogLevel::FromString(item["level"].get<std::string>()) : LogLevel::DEBUG;
            apd.format = item.contains("format") ? item["format"].get<std::string>() : LOG_DEFAULT_FORMAT; 
            ld.appenders.push_back(apd);
        }
        if(ld.appenders.empty()){
            return ld;
        }
        ld.name = root["name"].get<std::string>();
        ld.level = root.contains("level") ? LogLevel::FromString(root["level"].get<std::string>()) : LogLevel::DEBUG;
        //std::cout<<"----" <<LogDefineDump(ld) <<std::endl;
        return ld;
    }
};

template<>
class LexicalCast<LogDefine,std::string>{
public:
    std::string operator()(const LogDefine& val){
        if(val.appenders.empty()||val.name.empty()){
            return "";
        }
        Json root;
        root["name"] = val.name;
        root["level"] = LogLevel::ToString(val.level);
        Json jsonArr = Json::array();
        std::vector<LogAppenderDefine> vecAppender = val.appenders;
        for(int i=0;i<vecAppender.size();++i){
            Json node;
            if(vecAppender[i].type == 0){
                node["type"] = 0;
            }else if(vecAppender[i].type == 1){
                if(vecAppender[i].filepath.empty()){
                    continue;
                }
                node["type"] = 1;
                node["filepath"] = vecAppender[i].filepath;
            }else{
                continue;
            }
            node["level"] = LogLevel::ToString(vecAppender[i].level);
            node["format"] = vecAppender[i].format;
            jsonArr.push_back(node);
        }
        if(jsonArr.empty()){
            return "";
        }
        root["appenders"] = jsonArr;
        return root.dump();
    }
};

//周乐莎 99.11.15

ConfigVar<std::set<LogDefine> >::ptr g_logs_define
        = Config::Lookup<std::set<LogDefine> >(std::set<LogDefine>(),"logs","logs config");

//1.old存在,new不存在
//2.old存在,new存在
//3.old不存在,new存在
//4.old不存在,new不存在(不需要处理)

//此处直接采取清空,再添加新logger的方式处理,不进行以上判断
struct __LogInit__{
    __LogInit__(){
        g_logs_define->addListener([](const std::set<LogDefine>& old_value,
                    const std::set<LogDefine>& new_value){
            LogManager::GetInstance()->clrLogger();
            FEPOH_LOG_DEBUG(g_log_root) <<"logs config cb";
            for(auto& i : new_value) {
                fepoh::Logger::ptr logger;
                logger.reset(new Logger(i.name));
                logger->setLevel(i.level);
                logger->clrAppender();
                for(auto& a : i.appenders) {
                    fepoh::LogAppender::ptr ap;
                    if(a.type == 0) {
                        ap.reset(new StdLogAppender);
                    } else if(a.type == 1) {
                        ap.reset(new FileLogAppender(a.filepath));
                    }else{
                        continue;
                    }
                    ap->setLevel(a.level);
                    if(!a.format.empty()) {
                        LogFormatter::ptr fmt(new LogFormatter(a.format));
                        if(!fmt->getInit()) {
                            fmt.reset(new LogFormatter(LOG_DEFAULT_FORMAT));
                            std::cout << "log.name=" << i.name << " appender type=" << a.type
                                      << " formatter=" << a.format << " is invalid" << std::endl;
                        } 
                        ap->setFormatter(fmt);
                    }
                    logger->addAppender(ap);
                }
                LogManager::GetInstance()->addLogger(logger);
            }
            // for(auto& i : old_value) {
            //     auto it = new_value.find(i);
            //     if(it == new_value.end()) {
            //         //删除logger
            //         auto logger = FEPOH_LOG_NAME(i.name);
            //         logger->setLevel((LogLevel::Level)0);
            //         logger->clrAppender();
            //     }
            // }
            
        });
        //Config::LoadFromJson("../../resource/config/log.json");
    }
};

static struct __LogInit__  _log_init_;

// struct __LogInit__{
//     __LogInit__(){
//         const std::string root_format = "";
//         const std::string sys_format = "";

//         const std::string root_file = "../bin/log/root.txt";
//         const std::string sys_file = "../bin/log/system.txt";


//         Logger::ptr sys_log(new Logger("system"));
//         sys_log->addDefaultAppender();
//         sys_log->addAppender(LogAppender::ptr(new FileLogAppender(sys_file)));
//         LogManager::GetInstance()->addLogger(sys_log);

//         LogManager::GetInstance()->getLogger("root")->addAppender(LogAppender::ptr(new FileLogAppender(root_file)));
//     }
// };

// static struct __LogInit__ log_init;

}