#include "config.h"
#include "log/log.h"

using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test_base(){
    ConfigVar<int>::ptr int_val = Config::Lookup<int>(10,"system.port","port");
#define XX(type,val,name) \
    FEPOH_LOG_DEBUG(s_log_system) << Config::Lookup<type>(val,name,"")->getValue();
    XX(float,10.6,"test float");
    XX(bool,true,"test bool");
    XX(std::string,"i love zls","test,string");
#undef XX

}

void test_stl(){
    // ConfigVar<std::vector<int> >::ptr vec_int_val = Config::Lookup<std::vector<int>>({10,20,30},"test vec","");
    // FEPOH_LOG_DEBUG(s_log_system)<<vec_int_val->tostring();

    ConfigVar<std::vector<int> >::ptr vec_int_val = Config::Lookup<std::vector<int>>({3,452,3543,546,45,6,765,},"test vec_int");
    FEPOH_LOG_DEBUG(s_log_system)<<vec_int_val->tostring();

    ConfigVar<std::vector<int> >::ptr list_float_val = Config::Lookup<std::vector<int>>({3,452,3543,546,45,6,765,},"test list_float");
    FEPOH_LOG_DEBUG(s_log_system)<<list_float_val->tostring();

    ConfigVar<std::set<std::string> >::ptr set_str_val = Config::Lookup<std::set<std::string>>({"i love zls","wewewrq","dfasadf,rte","i love zls"},"test set_str");
    FEPOH_LOG_DEBUG(s_log_system)<<set_str_val->tostring();

    ConfigVar<std::unordered_set<std::string> >::ptr uset_str_val = Config::Lookup<std::unordered_set<std::string>>({"i love zls","wewewrq","dfasadf,rte","i love zls"},"test set_str");
    FEPOH_LOG_DEBUG(s_log_system)<<uset_str_val->tostring();


    ConfigVar<std::set<int> >::ptr set_int_val = Config::Lookup<std::set<int>>({3,5,2,34,6,1,6},"test set_str");
    FEPOH_LOG_DEBUG(s_log_system)<<set_int_val->tostring();

    ConfigVar<std::unordered_set<int> >::ptr uset_int_val = Config::Lookup<std::unordered_set<int>>({3,5,2,34,6,1,6},"test set_str");
    FEPOH_LOG_DEBUG(s_log_system)<<uset_int_val->tostring();
    
    ConfigVar<std::map<std::string,int> >::ptr map_str_val = Config::Lookup<std::map<std::string,int>>({{"7",1},{"5",2},{"3",3},{"2",5}},"test map_int");
    FEPOH_LOG_DEBUG(s_log_system)<<map_str_val->tostring();

    ConfigVar<std::unordered_map<std::string,int> >::ptr umap_str_val = Config::Lookup<std::unordered_map<std::string,int>>({{"7fsd",1},{"5dsfa",2},{"3dsaf",3},{"2sdf",5},{"8fdsf",3},{"1sdf",5}},"test map_int");
    FEPOH_LOG_DEBUG(s_log_system)<<umap_str_val->tostring();
    
    std::string str = "";
    if(str.empty()){
        std::cout <<1111<<std::endl;
    }

    // Json j2={
    //         {"name","LeBorn Jame"},
    //         {"number",23},
    //         {"man",true},
    //         {"children",{"LeBorn Jr","Bryce Maximus","Zhuri"}},
    //         };
    Json j2 = R"({
        "log":[
            {"name":"zls"},
            {"gender": "girl"}
        ]
    })"_json;

    for(auto& [key,value]:j2.items()){
        std::cout<< key <<":" <<value <<std::endl;
    }
}

void test_file(){
    Config::LoadFromJson("/home/fepoh/workspace/fepoh_server/resource/config/log.json");
}

ConfigVar<int>::ptr g_system_port = Config::Lookup<int>(10,"system.port");

int main(){
    s_log_system->setLevel(LogLevel::Level::WARN);
    test_base();
    //test_stl();
    test_file();
    std::string str = Config::dump();
    std::cout <<str <<std::endl;
    Logger::ptr ss_log_system = FEPOH_LOG_NAME("system");
    Config::LoadFromJson("/home/fepoh/workspace/fepoh_server/resource/config/log.json");
    FEPOH_LOG_DEBUG(ss_log_system) <<"wefg11111111111111111";
    FEPOH_LOG_DEBUG(g_log_root) <<"wefg11111111111111111";
    FEPOH_LOG_DEBUG(g_log_root) <<g_system_port->getValue();
    
}