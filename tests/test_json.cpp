#include "log/log.h"
#include "macro.h"

#include <nlohmann/json.hpp>
#include <istream>

using namespace fepoh;

using Json = nlohmann::json;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test_json(){

    std::ifstream in("/home/fepoh/workspace/fepoh_server/tests/test_json.json",std::ios::binary);

    if(!in.is_open()){
        FEPOH_LOG_ERROR(s_log_system) << "open file error.";
        return ;
    }
    Json j = Json::parse(in);
    std::cout << j["port"] <<std::endl;

}

class Person{
public:
    std::string name;
    std::string address;
    int age;

    
};

 void to_json(Json& j, const Person& p) {
    j = Json{{"name", p.name}, {"address", p.address}, {"age", p.age}};
}

 void from_json(const Json& j, Person& p) {
    j.at("name").get_to(p.name);
    j.at("address").get_to(p.address);
    j.at("age").get_to(p.age);
}


void test_selfdefine(){
    Person p = {"zhou le sha","jiahe",23};
    Json j(p);
    std::cout << j <<std::endl;
    auto p2 = j.get<Person>();
}


int main(){
    test_json();
    test_selfdefine();
}