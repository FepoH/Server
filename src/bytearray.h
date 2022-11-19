/*
 * @Author: fepo_h
 * @Date: 2022-11-17 18:02:41
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 23:27:41
 * @FilePath: /fepoh/workspace/fepoh_server/src/bytearray.h
 * @Description: 序列化和反序列化
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <memory>
#include <string>
#include <stdint.h>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>

namespace fepoh{

/*
    压缩算法:ZigZag压缩算法
*/


class ByteArray {
public:
    typedef std::shared_ptr<ByteArray> ptr;
    /**
     * @description: 数据块节点(数据结构为单向链表)
     * @return {*}
     */    
    struct Node {
        Node(size_t s);
        ~Node();
        char* ptr;
        Node* next;
    };

    /**
     * @description: 构造函数
     * @return {*}
     * @param {size_t} base_size 数据块基础大小,后续数据块基于这个大小,不能更改
     */    
    ByteArray(size_t base_size = 4096,uint8_t endian = (uint8_t)BIG_ENDIAN);
    /**
     * @description: 析构函数,释放内存
     * @return {*}
     */    
    ~ByteArray();
    //写数据
    void writeFint8  (int8_t value);
    void writeFuint8 (uint8_t value);
    void writeFint16 (int16_t value);
    void writeFuint16(uint16_t value);
    void writeFint32 (int32_t value);
    void writeFuint32(uint32_t value);
    void writeFint64 (int64_t value);
    void writeFuint64(uint64_t value);

    void writeFloat  (float value);
    void writeDouble (double value);
    void writeStringF16(const std::string& value);
    void writeStringF32(const std::string& value);
    void writeStringF64(const std::string& value);
    void writeStringVint(const std::string& value);
    void writeStringWithoutLength(const std::string& value);
    //写,压缩数据(经测试,感觉压缩的比实际还要大...)
    void writeInt32  (int32_t value);
    void writeUint32 (uint32_t value);
    void writeInt64  (int64_t value);
    void writeUint64 (uint64_t value);


    //读数据
    int8_t   readFint8();
    uint8_t  readFuint8();
    int16_t  readFint16();
    uint16_t readFuint16();
    int32_t  readFint32();
    uint32_t readFuint32();  
    int64_t  readFint64();
    uint64_t readFuint64();

    float    readFloat();
    double   readDouble();
    std::string readStringF16();
    std::string readStringF32();
    std::string readStringF64();
    std::string readStringVint();
    //读,压缩数据
    int32_t  readInt32();
    uint32_t readUint32();
    int64_t  readInt64();
    uint64_t readUint64();
    /**
     * @description: 清空数据
     * @return {*}
     */    
    void clear();
    /**
     * @description: 写操作,其余写操作都基于此函数
     * @return {*}
     * @param {void*} buf
     * @param {size_t} size 大小
     */    
    void write(const void* buf, size_t size);
    /**
     * @description:  读操作,其余读操作都基于此函数
     * @return {*}
     * @param {void*} buf
     * @param {size_t} size 大小
     */    
    void read(void* buf, size_t size);
    /**
     * @description: 读操作,不改变已读数据大小和读指针指向
     * @return {*}
     * @param {void*} buf
     * @param {size_t} size 要读的数据大小
     * @param {size_t} position 从该位置开始读
     */    
    void read(void* buf, size_t size, size_t position) const;
    /**
     * @description: 将数据写入文件:不改变已读数据大小和读指针指向
     * @return {*}
     * @param {string&} name 文件路径
     */    
    bool writeToFile(const std::string& path) const;
    /**
     * @description: 将文件数据写入
     * @return {*}
     * @param {string&} path
     */    
    bool readFromFile(const std::string& path);
    /**
     * @description: 获取基础数据大小
     * @return {*}
     */    
    size_t getBaseSize() const { return m_baseSize;}
    /**
     * @description: 获取可读数据大小
     * @return {*}
     */    
    size_t getReadSize() const { return m_size - m_readSize;}
    /**
     * @description: 数据转字符串
     * @return {*}
     */    
    std::string toString() const;
    /**
     * @description: 数据转16进制:用于二进制文件
     * @return {*}
     */    
    std::string toHexString() const;
    /**
     * @description: 将数据写入iovec
     * @return {*}
     * @param {uint64_t} len
     */    
    uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len = ~0ull) const;
    /**
     * @description: 将数据从postion位置开始写入iovec
     * @return {*}
     * @param {uint64_t} len
     * @param {uint64_t} position
     */    
    uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len, uint64_t position) const;
    /**
     * @description: 将iovec指针指向ByteArray的内存位置,iovec获取的数据存放位置就是ByteArray的数据位置
     * @return {*}
     * @param {uint64_t} len
     */    
    uint64_t getWriteBuffers(std::vector<iovec>& buffers, uint64_t len);
    /**
     * @description: 获取数据大小
     * @return {*}
     */    
    size_t getSize() const { return m_size;}
    /**
     * @description: 获取已读数据大小
     * @return {*}
     */    
    size_t getReadedSize() const{return m_readSize;}
    /**
     * @description: 设置数据大小
     * @return {*}
     * @param {size_t} v
     */
    void setSize(size_t v) {m_size = v;}
    /**
     * @description: 设置已读数据位置:可读数据以为改变
     * @return {*}
     * @param {size_t} v
     */    
    void setReadPos(size_t v);
private:
    /**
     * @description: 增加容量:若当前容量 >= size,则不会增加容量
     * @return {*}
     * @param {size_t} size 将要增加的容量
     */    
    void addCapacity(size_t size);
    /**
     * @description: 获取当前可存储空间大小
     * @return {*}
     */    
    size_t getCapacity() const { return m_capacity - m_size;}
private:
    /// 内存块的大小
    size_t m_baseSize;
    /// 已读数据大小
    size_t m_readSize;
    /// 总容量大小
    size_t m_capacity;
    /// 总数据的大小
    size_t m_size;
    /// 字节序,默认大端
    int8_t m_endian;
    /// 第一个内存块指针
    Node* m_root;
    /// 当前写指针
    Node* m_write;
    /// 当前读指针
    Node* m_read;
};





}//fepoh