/*
 * @Author: fepo_h
 * @Date: 2022-11-19 18:09:07
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 19:10:24
 * @FilePath: /fepoh/workspace/fepoh_server/src/config.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */

#include"config.h"

#include <ostream>
#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>


namespace fepoh{
/**
 * @description: 初始化类静态变量
 * @return {*}
 */
RWMutex Config::m_mutex;

static void ListAllNode(std::vector<std::pair<std::string,Json> >& allNode,Json node,std::string prefix){
    //名称只能使用小写._数字组成
    if(prefix.find_first_not_of("abcdefghikjlmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._012345678")
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

/**
 * @description: 递归获取文件夹下所有文件路径
 * @return {*}
 * @param {string} path 
 */
static bool LoadDir(std::vector<std::string>& vecPath ,std::string path){
    DIR* dir = NULL;
    struct dirent *dp;
    dir = opendir(path.c_str());
    if(dir == NULL){
        return false;;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if(strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0){
            continue;
        }
        struct stat sta;
        std::string new_file = path + "/" + dp->d_name;
        int rt = stat(new_file.c_str(),&sta);
        if(S_ISDIR(sta.st_mode)){
            LoadDir(vecPath,new_file);
        }else{
            vecPath.push_back(new_file);
        }
    }
    closedir(dir);
    return true;
}


bool Config::LoadFromDir(const std::string& path){
    if(path.empty()){
        return false;
    }
    int i = path.find("/");
    std::string tmpPath;
    if(i == path.size() -1){
        tmpPath = path.substr(0,path.size() - 1);
    }
    std::vector<std::string> vecPath;
    if(!LoadDir(vecPath,tmpPath)){
        return false;
    }
    for(auto& item : vecPath){
        int index = item.rfind(".json");
        if((index != std::string::npos) 
            || (index == item.size() - 4)){
            LoadFromJson(item);
        }
    }
    return true;
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