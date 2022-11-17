#pragma once

#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>
#include <vector>
#include <map>

namespace fepoh{
    
class Address{
public:
    typedef std::shared_ptr<Address> ptr;
    virtual ~Address() {}
    //获取单个IP地址(IPv4和IPv6都有可能)
    static Address::ptr LookupIPAddr(const std::string& node ,uint16_t port);
    //获取全部IP地址,成功返回true
    static bool LookupIPAddr(std::vector<Address::ptr>& vecIPaddr
                ,const std::string& node,uint16_t port);
                
    static bool GetInterfaceAddresses(std::multimap<std::string
            ,std::pair<Address::ptr,uint32_t>>& res);
    static bool GetInterfaceAddresses(std::vector<std::pair<Address::ptr,uint32_t>>& res
            ,const std::string& itfc);
    //根据sockaddr信息创建相应子类
    static Address::ptr Create(const sockaddr* addr ,socklen_t socklen);
    //获取协议族
    int getFamily() const;
    //获取sockaddr
    virtual const sockaddr* getAddr() const = 0;
    virtual sockaddr* getAddr() = 0;
    //获取地址长度
    virtual socklen_t getAddrLen() const = 0;
    //序列化
    virtual std::ostream& insert(std::ostream& os) const = 0;
    std::string tostring();
    //重载运算符函数,方便stl容器
    bool operator<(const Address& rhs) const;
    bool operator==(const Address& rhs) const;
    bool operator!=(const Address& rhs) const;
    bool operator>(const Address& rhs) const;
};

class IPAddress : public Address{
public:
    typedef std::shared_ptr<IPAddress> ptr;
    //获取单个IP地址(IPv4和IPv6都有可能)
    static IPAddress::ptr Lookup(const std::string& node ,uint16_t port);
    //获取全部IP地址,成功返回true
    static bool Lookup(std::vector<IPAddress::ptr>& vecIPaddr
                ,const std::string& node,uint16_t port);
    //为上面两个集合,节省代码空间
    static IPAddress::ptr Lookup(std::vector<IPAddress::ptr>& vecIPAddr,const std::string& node ,uint16_t port,
                bool rtvec,int family = AF_UNSPEC,int type = 0,int protocol = 0);
    //端口获取与设置
    virtual int getPort() const = 0;
    virtual void setPort(int val) = 0;
    //创建子网掩码,广播地址,网段
    virtual IPAddress::ptr broadcastAddress(uint32_t prefix_len) = 0;
    virtual IPAddress::ptr networdAddress(uint32_t prefix_len) = 0;
    virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;
};

class IPv4Address : public IPAddress{
public:
    typedef std::shared_ptr<IPv4Address> ptr;

    static IPv4Address::ptr Create(const std::string& address ,uint16_t port);

    IPv4Address(struct sockaddr_in& addr);
    IPv4Address(uint32_t address = INADDR_ANY ,uint16_t port = 0);

    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;

    int getPort() const override;
    void setPort(int val) override;
    
    IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
    IPAddress::ptr networdAddress(uint32_t prefix_len) override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;

    std::ostream& insert(std::ostream& os) const override;

private:
    struct sockaddr_in m_addr;     //地址
};

class IPv6Address : public IPAddress{
public:
    typedef std::shared_ptr<IPv6Address> ptr;

    static IPv6Address::ptr Create(const std::string& address ,uint16_t port);

    IPv6Address(struct sockaddr_in6& addr);
    IPv6Address(const char* addr ,uint16_t port = 0);

    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    int getPort() const override;
    void setPort(int val) override;
    IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
    IPAddress::ptr networdAddress(uint32_t prefix_len) override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;
    std::ostream& insert(std::ostream& os) const override;
private:
    struct sockaddr_in6 m_addr;     //地址
};

class UnixAddress : public Address{
public:
    typedef std::shared_ptr<UnixAddress> ptr;

    UnixAddress();
    UnixAddress(const struct sockaddr_un& addr,socklen_t socklen);
    UnixAddress(const std::string& path);

    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;
private:
    struct sockaddr_un m_addr;
    socklen_t m_length;
};

class UnkownAddress : public Address{
public:
    typedef std::shared_ptr<UnkownAddress> ptr;

    UnkownAddress(int family);
    UnkownAddress(const struct sockaddr& addr);

    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

private:
    struct sockaddr m_addr;
};

} // namespace fepoh
