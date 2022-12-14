/*
 * @Author: fepo_h
 * @Date: 2022-11-19 17:46:30
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 20:13:07
 * @FilePath: /fepoh/workspace/fepoh_server/src/config.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once
#include "log/log.h"
#include "util.h"
#include "thread/mutex.h"

#include <memory>
#include <string>
#include <set> 
#include <nlohmann/json.hpp>
#include <boost/lexical_cast.hpp>

#include <typeinfo>
#include <sstream>
#include <functional>
#include <exception>

#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

using Json = nlohmann::json;

static fepoh::Logger::ptr g_log_root=FEPOH_LOG_NAME("root");

namespace fepoh{

/*
问题:
    1.在调用tostring时,会导致基础类型转为string类型
        例如:1--tostring-->"1",而yaml不会出现此类问题
        后续应增加基础类型判断,或采用其他方法处理该问题
解决过程:
    1.  已完成对基础类型判断,采取util中的IsBaseType,对基础类型进行判断,并且对性能由一定提升

      但是在此过程中发现该Json库含有一个bug,对于unorder_map,此库dump输出总是排序的,
      这与unorder_map的性质不符合,因为tostring对与项目影响不大,故未深究.
*/

//配置变量基类 
class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    /**
     * @description: 构造函数
     * @return {*}
     * @param {string&} name 配置名称
     * @param {string&} description 简短描述
     */    
    ConfigVarBase(const std::string& name,const std::string& description = "");

    virtual ~ConfigVarBase(){}

    const std::string& getName() const {return m_name;}
    const std::string& getDescription() const {return m_description;}
    /**
     * @description: 获取类型名称
     * @return {*}
     */
    virtual std::string getTypeName() = 0;
    //变量转字符串
    virtual const std::string tostring() = 0;
    //字符串转变量
    virtual bool fromstring(const std::string& val) = 0;
protected:
    RWMutex m_mutex;
    std::string m_name;         //配置名称
    std::string m_description;  //配置描述
};

/**
 * @description: 转换基类
 * @return {*}
 * @param SRC 源类型
 * @param DST 目标类型
 */
template<class SRC,class DST>
class LexicalCast{
public:
    DST operator()(const SRC& val){
        return boost::lexical_cast<DST>(val);
    }
};

/**
 * @description: vector<-->std::string
 * @return {*}
 */
template<class T>
class LexicalCast<std::string,std::vector<T> >{
public:
    std::vector<T> operator()(const std::string& str){
        std::vector<T> vecT;
        Json val = Json::parse(str);
        for(size_t i=0;i<val.size();++i){
            std::stringstream ss;
            ss<<val[i];
            vecT.push_back(LexicalCast<std::string,T>()(ss.str()));
        }
        return vecT;
    }
};

template<class T>
class LexicalCast<std::vector<T>,std::string >{
public:
    std::string operator()(const std::vector<T>& val){
        if(!val.empty()){
            if(IsBaseType(val[0])){
                Json res(val);
                return res.dump();
            }else{
                std::vector<std::string> vecStr;
                for(auto item:val){
                    vecStr.push_back(LexicalCast<T,std::string>()(item));
                }
                Json res(vecStr);
                return res.dump();
            }
        }
        return "";
    }
};
/**
 * @description: list<-->std::string
 * @return {*}
 */
template<class T>
class LexicalCast<std::string,std::list<T> >{
public:
    std::list<T> operator()(const std::string& str){
        std::list<T> lsT;
        Json val = Json::parse(str);
        for(size_t i=0;i<val.size();++i){
            std::stringstream ss;
            ss<<val[i];
            lsT.push_back(LexicalCast<std::string,T>()(ss.str()));
        }
        return lsT;
    }
};

template<class T>
class LexicalCast<std::list<T>,std::string >{
public:
    std::string operator()(const std::list<T>& val){
        if(!val.empty()){
            if(IsBaseType(*val.begin())){
                Json res(val);
                return res.dump();
            }else{
                std::vector<std::string> vecStr;
                for(auto item:val){
                    vecStr.push_back(LexicalCast<T,std::string>()(item));
                }
                Json res(vecStr);
                return res.dump();
            }
        }
        return "";
    }
};

/**
 * @description: set<-->std::string
 * @return {*}
 */
template<class T>
class LexicalCast<std::string,std::set<T> >{
public:
    std::set<T> operator()(const std::string& str){
        std::set<T> setT;
        Json val = Json::parse(str);
        for(size_t i=0;i<val.size();++i){
            std::stringstream ss;
            ss<<val[i];
            setT.insert(LexicalCast<std::string,T>()(ss.str()));
        }
        return setT;
    }
};

template<class T>
class LexicalCast<std::set<T>,std::string >{
public:
    std::string operator()(const std::set<T>& val){
        //此处不用std::set,当使用std::set,因为转为std::string,会按照字典序排列
        //例如:6和36,当转为字符串时,"6"的字典序比"36"大,倒是输出结果不符合std::set的特性;
        //故此处使用vector,对结果不产生影响,也无副作用
        if(!val.empty()){
            if(IsBaseType(*(val.begin()))){
                Json res(val);
                return res.dump();
            }else{
                std::vector<std::string> vecStr;
                for(auto item:val){
                    vecStr.push_back(LexicalCast<T,std::string>()(item));
                }
                Json res(vecStr);
                return res.dump();
            }
        }
        return "";
    }
};

/**
 * @description: unordered_set<-->std::string
 * @return {*}
 */
template<class T>
class LexicalCast<std::string,std::unordered_set<T> >{
public:
    std::unordered_set<T> operator()(const std::string& str){
        std::unordered_set<T> usetT;
        Json val = Json::parse(str);
        for(size_t i=0;i<val.size();++i){
            std::stringstream ss;
            ss<<val[i];
            usetT.insert(LexicalCast<std::string,T>()(ss.str()));
        }
        return usetT;
    }
};

template<class T>
class LexicalCast<std::unordered_set<T>,std::string >{
public:
    std::string operator()(const std::unordered_set<T>& val){
        if(!val.empty()){
            if(IsBaseType(*(val.begin()))){
                Json res(val);
                return res.dump();
            }else{
                std::vector<std::string> vecStr;
                for(auto item:val){
                    vecStr.push_back(LexicalCast<T,std::string>()(item));
                }
                Json res(vecStr);
                return res.dump();
            }
        }
        return "";
    }
};

/**
 * @description: map<-->std::string
 * @return {*}
 */
template<class T>
class LexicalCast<std::string,std::map<std::string,T> >{
public:
    std::map<std::string,T> operator()(const std::string& str){
        std::map<std::string,T> mapStrT;
        Json val = Json::parse(str);
        std::stringstream ss1;
        std::stringstream ss2;
        for(auto it = val.begin();it!=val.end();++it){
            ss1.str("");
            ss2.str("");
            ss1<<it.key();
            ss2<<it.value();
            mapStrT.insert(std::make_pair(ss1.str(),LexicalCast<std::string,T>()(ss2.str())));
        }
        return mapStrT;
    }
};

template<class T>
class LexicalCast<std::map<std::string,T>,std::string >{
public:
    std::string operator()(const std::map<std::string,T>& val){
        if(!val.empty()){
            if(IsBaseType((val.begin())->second)){
                Json res(val);
                return res.dump();
            }else{
                std::map<std::string,std::string> mapStrT;
                for(auto it=val.begin();it!=val.end();++it){
                    mapStrT[it->first] = LexicalCast<T,std::string>()(it->second);
                }
                Json res(mapStrT);
                return res.dump();
            }
        }
        return "";
    }
};

/**
 * @description: unordered_map<-->std::string
 * @return {*}
 */
template<class T>
class LexicalCast<std::string,std::unordered_map<std::string,T> >{
public:
    std::unordered_map<std::string,T> operator()(const std::string& str){
        std::unordered_map<std::string,T> umapStrT;
        Json val = Json::parse(str);
        std::stringstream ss1;
        std::stringstream ss2;
        for(auto it = val.begin();it!=val.end();++it){
            ss1.str("");
            ss2.str("");
            ss1<<it.key();
            ss2<<it.value();
            umapStrT.insert(std::make_pair(ss1.str(),LexicalCast<std::string,T>()(ss2.str())));
        }
        return umapStrT;
    }
};

template<class T>
class LexicalCast<std::unordered_map<std::string,T>,std::string >{
public:
    std::string operator()(const std::unordered_map<std::string,T>& val){
            std::unordered_map<std::string,T> m = val;
        if(!val.empty()){
            if(IsBaseType((val.begin())->second)){
                Json res(m);
                return res.dump();
            }else{
                std::unordered_map<std::string,std::string> umapStrT;
                for(auto it=val.begin();it!=val.end();++it){
                    umapStrT[it->first] = LexicalCast<T,std::string>()(it->second);
                }
                Json res(umapStrT);
                return res.dump();
            }
        }
        return "";
    }
};


template<class T ,class ToStr = LexicalCast<T,std::string>
                 ,class FromStr = LexicalCast<std::string,T> >
class ConfigVar : public ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    /**
     * @description: 回调函数
     * @return {*}
     * @param {T&} old_val
     * @param {T&} new_val
     */    
    typedef std::function<void(const T& old_val,const T& new_val)> ListenerFunc;
    /**
     * @description: 构造函数
     * @return {*}
     * @param {T&} value 配置值
     * @param {string&} name 配置名称
     * @param {string} description 配置描述
     */    
    ConfigVar(const T& value,const std::string& name,const std::string description)
        :m_value(value),ConfigVarBase(name,description){

    }
    //获取配置变量
    const T& getValue() const {return m_value;}
    //设置变量
    void setValue(const T& value){
        {
            ReadLock lock(m_mutex);
            if(m_value == value){
                return ;
            }
            //在变量修改后调用回调函数
            for(auto& i : m_listeners) {
                i.second(m_value, value);
            }
        }
        WriteLock lock1(m_mutex);
        m_value = value;
    }

    //获取类型名称
    std::string getTypeName() override{
        return typeid(T).name();
    }
    //变量转string
    const std::string tostring() override{
        std::string strRes = "";
        try{
            ReadLock lock(m_mutex);
            strRes = ToStr()(m_value);                
        }catch(std::exception& e){
            FEPOH_LOG_DEBUG(g_log_root)<<"ConfigVar::tostring() error." << "Turn "
                    << getTypeName() << " to string error.";
        }
        return strRes;
    }
    //字符串转变量
    bool fromstring(const std::string& val) override {
        try{
            T new_value = FromStr()(val);
            //此处调用回调函数
            setValue(new_value);
            return true;
        }catch(std::exception& e){
            FEPOH_LOG_DEBUG(g_log_root)<<"ConfigVar::tostring() error." << "Turn string "
                    "to " << getTypeName() << " error.";
        }
        return false;
    }
    /**
     * @description: 添加回调函数,当值发生改变时,调用回调函数
     * @return {*}
     * @param {ListenerFunc&} func
     */    
    uint64_t addListener(const ListenerFunc& func){
        WriteLock lock(m_mutex);
        static uint64_t index = 0;
        m_listeners.insert(std::make_pair(index,func));
        return index++;
    }
    /**
     * @description: 删除回调函数
     * @return {*}
     * @param {uint64_t} index
     */    
    void delListender(const uint64_t index){
        WriteLock lock(m_mutex);
        auto it = m_listeners.find(index);
        if(it != m_listeners.end()){
            m_listeners.erase(it);
        }
    }
    /**
     * @description: 清空回调函数
     * @return {*}
     */    
    void clrListener(){
        WriteLock lock(m_mutex);
        m_listeners.clear();
    }
    /**
     * @description: 获取回调函数
     * @return {*}
     * @param {uint64_t} index
     */    
    ListenerFunc getListener(const uint64_t index){
        ReadLock lock(m_mutex);
        return m_listeners[index];
    }

private:
    //回调函数
    std::map<uint64_t,ListenerFunc> m_listeners;
    T m_value;      //值
};
/**
 * @description: 配置管理
 * @return {*}
 */
class Config{
public:
    /**
     * @description: 加载目录,只支持Json文件
     * @return {*}
     * @param {string&} path 路径
     */    
    static bool LoadFromDir(const std::string& path);
    /**
     * @description: 通过文件加载配置
     * @return {*}
     * @param {string&} filepath
     */    
    static void LoadFromJson(const std::string& filepath);
    /**
     * @description: 添加配置
     * @return {*}
     */    
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const T& val,
    const std::string name,const std::string& discription = ""){
        if(name.find_first_not_of("abcdefghikjlmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._012345678")
            != std::string::npos) {
            FEPOH_LOG_ERROR(g_log_root) << "Config invalid name: " << name;
            return nullptr;
        }
        ReadLock lock(m_mutex);
        auto it = GetData().find(name);
        if(it != GetData().end()){
            FEPOH_LOG_ERROR(g_log_root) << "Config::Lookup error. The value:"<<name <<" is already exist";
            return 0;
            //return std::dynamic_pointer_cast<ConfigVar<T>>(GetData()[name]);
        }
        typename ConfigVar<T>::ptr var(new ConfigVar<T>(val,name,discription));
        lock.unlock();
        WriteLock lock1(m_mutex);
        GetData()[name] = var;
        return var;
    }
    /**
     * @description: 查找配置
     * @return {*} 失败返回nullptr
     */    
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
        ReadLock lock(m_mutex);
        ConfigVarBase::ptr pBase = LookupBase(name);
        if(pBase){
            return std::dynamic_pointer_cast<ConfigVar<T>>(pBase);
        }
        return nullptr;
    }
    /**
     * @description: 查找配置
     * @return {*} 失败返回nullptr,成功返回基类指针
     */   
    static ConfigVarBase::ptr LookupBase(const std::string& name){
        ReadLock lock(m_mutex);
        auto it = GetData().find(name);
        if(it == GetData().end()){
            return nullptr;
        }
        return it->second;
    }
    /**
     * @description: 格式化输出,方便debug
     * @return {*}
     */    
    static std::string dump();
private:
    static RWMutex m_mutex;
private:
    //处理跨编译单元初始化次序问题(尝试)
    static std::map<std::string,ConfigVarBase::ptr>& GetData(){
        static std::map<std::string,ConfigVarBase::ptr> m_data;
        return m_data;
    }
};





//vector list
//set  uset map umap
// template<class SRC,class DST>
// class LexicalCast{
// public:
//     DST operator()(const SRC& val){
//         return boost::lexical_cast<DST>(val);
//     }
// };
// //vector
// template<class T>
// class LexicalCast<std::vector<T>,std::string>{
// public:
//     std::string operator()(const std::vector<T>& val){
//         YAML::Node seq(YAML::NodeType::Sequence);
//         for(int i=0;i<val.size();++i){
//             seq.push_back(YAML::Load(LexicalCast<T,std::string>()(val[i])));
//         }
//         std::stringstream ss;
//         ss<<seq;
//         return ss.str();
//     }
// };
// template<class T>
// class LexicalCast<std::string,std::vector<T> >{
// public:
//     T operator()(const std::string& str){
//         YAML::Node node=YAML::Load(str);
//         typename std::vector<T> vec;
//         for(int i=0;i<node.size();++i){
//             vec.push_back(LexicalCast<std::string,T>()(node[i].as<std::string>()));
//         }
//         return vec;
//     }
// };
// template<class T>
// class LexicalCast<std::string,std::list<T> >{
// public:
//     std::list<T> operator()(const std::string& str){
//         YAML::Node node=YAML::Load(str);
//         typename std::list<T> arr;
//         for(int i=0;i<node.size();++i){
//             arr.push_back(LexicalCast<std::string,T>()(node[i]));
//         }
//         return arr;
//     }
// };
// template<class T>
// class LexicalCast<std::list<T>,std::string>{
// public:
//     std::string operator()(std::list<T> val){
//         YAML::Node node(YAML::NodeType::Sequence);
//         typename std::list<T> arr;
//         for(int i=0;i<node.size();++i){
//             arr.push_back(LexicalCast<std::string,T>()(node[i]));
//         }
//         return arr;
//     }
// };
// /**
//  * @description: 配置变量
//  * ToStr:将T转为std::string
//  * FromStr:将std::string转为T
//  */
// template<class T,class ToStr=LexicalCast<T,std::string>
//                 ,class FromStr=LexicalCast<std::string,T> >
// template<class T,class ToStr = std::string,class FromStr = std::string>
// class ConfigVar:public ConfigVarBase{
// public:
//     typedef std::shared_ptr<ConfigVar> ptr;
//     /**
//      * @description: 回调函数类型
//      * 参数分别为:旧值和新值
//      */
//     typedef std::function<void(const T&,const T&)> on_change_cb;
//     ConfigVar(const std::string& name,const T& val,const std::string& description="")
//         :ConfigVarBase(name,description),m_value(val){
//     }
//     ~ConfigVar(){
//     }
//     const std::string toString()override{
//         try{
//             return ToStr()(m_value);
//         }catch(...){
//             FEPOH_LOG_ERROR(g_log_root)<<"ConfigVar toString error:from T="
//                 <<getTypeName()<<" to string error";
//         }
//         return nullptr;
//     }
//     bool fromString(const std::string& str)override{
//         try{
//             //不直接赋值,通过setVal方便使用回调函数
//             setVal(FromStr()(str));
//             return true;
//         }catch(...){
//             FEPOH_LOG_ERROR(g_log_root)<<"ConfigVar toString error:from string to T="
//                 <<getTypeName()<<" error";
//         }
//         return false;
//     }
//     //获取类型
//     const std::string getTypeName()override{
//         return typeid(T).name();
//     }
//     //获取配置变量
//     T getValue()const{return m_value;}
//     //设置配置变量
//     void setVal(const T& val){
//         if(m_value!=val){
//             if(!m_cbs.empty()){
//                 for(auto it=m_cbs.begin();it!=m_cbs.end();++it){
//                     it->second(m_value,val);
//                 }
//             }else{
//                 m_value=val;
//             }
//         }
//     }
//     //添加回调函数
//     uint64_t addCb(const std::function<void(T,T)>& cb){
//         static uint64_t s_cb_key=1;
//         uint64_t tmp=s_cb_key;
//         m_cbs[s_cb_key++]=cb;
//         return s_cb_key;
//     }
//     //删除回调函数
//     bool delCb(uint64_t key){
//         auto it=m_cbs.find(key);
//         if(it!=m_cbs.end()){
//             m_cbs.erase(it);
//             return true;
//         }
//         return false;
//     }
//     //清除回调函数
//     void clrCb(){
//         m_cbs.clear();
//     }
// protected:
//     T m_value;        //配置变量
//     std::map<uint64_t, on_change_cb> m_cbs; //回调函数
// };

// //配置管理类
// class ConfigManager{
// public:
//     template<class T>
//     static typename ConfigVar<T>::ptr Lookup(const std::string& name
//             ,const T& val,const std::string& description=""){
//         auto res=Lookup(name);
//         if(res){
//             try{
//                 //auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
//                 return std::dynamic_pointer_cast<ConfigVar<T> >(res);
//             }catch(...){
//                 FEPOH_LOG_ERROR(g_log_root)<<"ConfigVar Lookup error:"<<
//                     "dynamic_cast ConfigVarBase to ConfigVar error";
//             }
//             return nullptr;
//         }
//         typename ConfigVar<T>::ptr var(new ConfigVar<T>(name,val,description));
//         GetData()[name]=var;
//         return var;
//     }
//     static ConfigVarBase::ptr Lookup(const std::string& name);

//     static void LoadFromYaml();
// private:
//     //回调数据,采取静态函数,使之尽快初始化
//     static std::map<std::string,ConfigVarBase::ptr> GetData(){
//         static std::map<std::string,ConfigVarBase::ptr> s_data;
//         return s_data;
//     }
// };
}//namespace