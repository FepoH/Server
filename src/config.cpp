/*
 * @Author: Fepo_H fepo_h@163.com
 * @Date: 2022-06-19 21:56:38
 * @LastEditors: Fepo_H fepo_h@163.com
 * @LastEditTime: 2022-06-22 17:19:59
 * @FilePath: /admin/workspace/fepoh_server/fepoh/config.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by Fepo_H fepo_h@163.com, All Rights Reserved. 
 */
#include"config.h"

#include <ostream>


namespace fepoh{

RWMutex Config::m_mutex;

static void ListAllNode(std::vector<std::pair<std::string,Json> >& allNode,Json node,std::string prefix){
    if(prefix.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678")
            != std::string::npos) {
        FEPOH_LOG_ERROR(g_log_root) << "Config invalid name: " << prefix << " : " << node;
        return;
    }
    for(auto [key,value]:node.items()){
        std::string tmpstr = prefix.empty() ? key : prefix + "." + key;
        if(value.is_array()){
            allNode.push_back(std::make_pair(tmpstr,value));
            for(int i=0;i<value.size();++i){
                ListAllNode(allNode,value[i],tmpstr);
            }
        }else{
            allNode.push_back(std::make_pair(tmpstr,value));
        }
    }
}

void Config::LoadFromJson(const std::string& filepath){           
    std::ifstream ifs(filepath);
    Json root = Json::parse(ifs);
    std::vector<std::pair<std::string,Json> > allNode;
    ListAllNode(allNode,root,"");

    for(auto& item:allNode){
        std::string key = item.first;
        if(key.empty()){
            continue;
        }
        ConfigVarBase::ptr var = LookupBase(key);
        if(var) {
            std::stringstream ss;
            ss << item.second;
            var->fromstring(ss.str());
        }
    }
    
}

ConfigVarBase:: ConfigVarBase(const std::string& name,const std::string& description)
        :m_name(name),m_description(description){

}

std::string Config::dump(){
    std::stringstream ss;
    for(auto it=GetData().begin();it!=GetData().end();++it){
        ss <<it->first <<":" <<it->second->tostring() <<std::endl;
    }
    return ss.str();
}





}//namespace