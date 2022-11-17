#pragma once

#include <memory>
#include <string>
#include <stdint.h>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>

namespace fepoh{

class ByteArray {
public:
    typedef std::shared_ptr<ByteArray> ptr;

    struct Node {
        Node(size_t s);
        ~Node();
        char* ptr;
        Node* next;
    };


    ByteArray(size_t base_size = 4096);
    ~ByteArray();

    void writeFint8  (int8_t value);
    void writeFuint8 (uint8_t value);
    void writeFint16 (int16_t value);
    void writeFuint16(uint16_t value);
    void writeFint32 (int32_t value);
    void writeFuint32(uint32_t value);
    void writeFint64 (int64_t value);
    void writeFuint64(uint64_t value);
    void writeInt32  (int32_t value);
    void writeUint32 (uint32_t value);
    void writeInt64  (int64_t value);
    void writeUint64 (uint64_t value);
    void writeFloat  (float value);
    void writeDouble (double value);
    void writeStringF16(const std::string& value);
    void writeStringF32(const std::string& value);
    void writeStringF64(const std::string& value);
    void writeStringVint(const std::string& value);
    void writeStringWithoutLength(const std::string& value);

    int8_t   readFint8();
    uint8_t  readFuint8();
    int16_t  readFint16();
    uint16_t readFuint16();
    int32_t  readFint32();
    uint32_t readFuint32();  
    int64_t  readFint64();
    uint64_t readFuint64();
    int32_t  readInt32();
    uint32_t readUint32();
    int64_t  readInt64();
    uint64_t readUint64();
    float    readFloat();
    double   readDouble();
    std::string readStringF16();
    std::string readStringF32();
    std::string readStringF64();
    std::string readStringVint();

    void clear();
    void write(const void* buf, size_t size);
    void read(void* buf, size_t size);
    void read(void* buf, size_t size, size_t position) const;
    bool writeToFile(const std::string& name) const;
    bool readFromFile(const std::string& name);
    size_t getBaseSize() const { return m_baseSize;}
    size_t getReadSize() const { return m_size - m_readSize;}
    std::string toString() const;
    std::string toHexString() const;
    uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len = ~0ull) const;
    uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len, uint64_t position) const;
    uint64_t getWriteBuffers(std::vector<iovec>& buffers, uint64_t len);
    size_t getSize() const { return m_size;}
    size_t getReadedSize() const{return m_readSize;}
    void setSize(size_t v) {m_size = v;}
    void setReadPos(size_t v);
private:
    void addCapacity(size_t size);

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