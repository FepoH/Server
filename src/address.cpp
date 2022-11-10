#include "address.h"
#include "hook.h"
#include "log/log.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
// #include "endian.h"

namespace fepoh{

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

template<class T>
static T CreateRevMask(uint32_t bits){
    return (1 << (sizeof(T) * 8 - bits)) -1;
}

template<class T>
static uint32_t CountBytes(T value){
    uint32_t res = 0;
    for(;value;++res){
        value &= value -1;
    }
    return res;
}

Address::ptr Address::Create(const struct sockaddr* addr ,socklen_t socklen){
    Address::ptr address = nullptr;
    switch(addr->sa_family){
        case AF_INET:
            address.reset(new IPv4Address(*(sockaddr_in*)addr));
            break;
        case AF_INET6:
            address.reset(new IPv6Address(*(sockaddr_in6*)addr));
            break;
        case AF_UNIX:
            address.reset(new UnixAddress(*(sockaddr_un*)addr,socklen));
        default:
            address.reset((new UnkownAddress(*addr)));
            break;
    }
    return address;
}

IPv4Address::ptr IPv4Address::Create(const std::string& address ,uint32_t port){
    struct sockaddr_in addr;
    bzero(&addr ,sizeof(addr));
    int rt = inet_pton(AF_INET ,address.c_str() ,&addr.sin_addr.s_addr);
    if(rt != 1){
        FEPOH_LOG_ERROR(s_log_system) << "IPv4Address Create error.inet_pton error.addr = " << address
                <<",errno = " <<errno <<":" <<strerror(errno);
        return nullptr;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    return IPv4Address::ptr(new IPv4Address(addr));
}

IPv6Address::ptr IPv6Address::Create(const std::string& address ,uint32_t port){
    struct sockaddr_in6 addr;
    bzero(&addr ,sizeof(addr));
    int rt = inet_pton(AF_INET6 ,address.c_str() ,&addr.sin6_addr.s6_addr);
    if(rt != 1){
        FEPOH_LOG_ERROR(s_log_system) << "IPv6Address Create error.inet_pton error.addr = " << address
                <<",errno = " <<errno <<":" <<strerror(errno);
        return nullptr;
    }
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    return IPv6Address::ptr(new IPv6Address(addr));
}

static bool Lookup(std::vector<IPAddress::ptr>& vecIPaddr,const std::string& host ,
                int family,int type,int protocol);



IPAddress::ptr IPAddress::Lookup(const std::string& node ,uint32_t port){
    std::vector<IPAddress::ptr> vecIPAddr;
    return Lookup(vecIPAddr,node,port,false);
}

bool IPAddress::Lookup(std::vector<IPAddress::ptr>& vecIPAddr,const std::string& node,uint32_t port){
    Lookup(vecIPAddr,node,port,true);
    return !vecIPAddr.empty();
}

IPAddress::ptr IPAddress::Lookup(std::vector<IPAddress::ptr>& vecIPAddr,const std::string& node ,uint32_t port,
            bool rtvec,int family,int type,int protocol){
    vecIPAddr.clear();
    struct addrinfo hints;
    struct addrinfo* res,*rp;
    bzero(&hints ,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = protocol;
    hints.ai_socktype = type;
    
    std::string tmpNode;
    std::string tmpService;
    if(!node.empty() && node[0] == '['){
        int index = node.find(']');
        if(index != std::string::npos){
            tmpNode = node.substr(1,index - 1);
        }
    }
    int index = node.rfind(':');
    if(index != std::string::npos && index >= 0 && index < node.size() -1){
        tmpService = node.substr(index+1);
        if(tmpNode.empty()){
            tmpNode = node.substr(0,index);
        }
    }
    if(tmpNode.empty()){
        tmpNode = node;
    }

    int rt = getaddrinfo(tmpNode.c_str() ,tmpService.c_str() ,&hints ,&res);
    if(rt != 0){
        FEPOH_LOG_ERROR(s_log_system) << "IPAddress::Create error.getaddrinfo error.rt = "
                << rt <<",node = " << tmpNode <<",servive = " <<tmpService
                <<",errno = " <<errno <<":" <<strerror(errno);
        return nullptr; 
    }
    IPAddress::ptr ipAddr;
    for(rp = res; rp != NULL;rp = rp->ai_next){
        Address::ptr addr = Address::Create(rp->ai_addr,rp->ai_addrlen);
        try{
            ipAddr = std::dynamic_pointer_cast<IPAddress>(addr);
            if(!ipAddr){
                continue;
            }
            if(port != 0){
                ipAddr->setPort(port);
            }
        }catch(...){
            ipAddr.reset();
            continue;
        }
        if(!rtvec){
            break;
        }
        vecIPAddr.push_back(ipAddr);
    }
    freeaddrinfo(res);
    if(!vecIPAddr.empty()){
        return vecIPAddr[0];
    }
    return ipAddr;
}


bool Address::GetInterfaceAddresses(std::multimap<std::string
        ,std::pair<Address::ptr,uint32_t>>& res){
    res.clear();
    struct ifaddrs *ifaddr,*ifa;
    if (getifaddrs(&ifaddr) == -1) {
        FEPOH_LOG_ERROR(s_log_system) <<"Address::GetInterfaceAddresses error.errno = "
                << errno <<":" <<strerror(errno);
        return false;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL){
            continue;
        }
        
        socklen_t tmpSocklen = 0;
        switch (ifa->ifa_addr->sa_family){
            case AF_INET:
                tmpSocklen = sizeof(struct sockaddr_in);
                break;
            case AF_INET6:
                tmpSocklen = sizeof(struct sockaddr_in6);
                break;
            default:
                tmpSocklen = -1;
                break;
        }
        if(tmpSocklen == -1){
            continue;
        }
        Address::ptr tmpAddr = Address::Create(ifa->ifa_addr,tmpSocklen);
        try{
            IPAddress::ptr tmp = std::dynamic_pointer_cast<IPAddress>(tmpAddr);
            if(!tmp){
                continue;
            }
        }catch(...){
            continue;
        }
        res.insert(std::make_pair(ifa->ifa_name,std::make_pair(tmpAddr,tmpSocklen)));
    }

    freeifaddrs(ifaddr);
    return !res.empty();
}

bool Address::GetInterfaceAddresses(std::vector<std::pair<Address::ptr,uint32_t>>& res
        ,const std::string& itfc){
    std::multimap<std::string,std::pair<Address::ptr,uint32_t>> mapRes;
    bool flag = GetInterfaceAddresses(mapRes);
    if(!flag){
        return false;
    }
    res.clear();
    for(auto it = mapRes.begin();it!=mapRes.end();++it){
        res.push_back(it->second);
    }
    return !res.empty();
}

//Address
int Address::getFamily() const{
    return getAddr()->sa_family;
}

std::string Address::tostring(){
    std::stringstream ss;
    insert(ss);
    return ss.str();
}

bool Address::operator<(const Address& rhs) const{
    socklen_t len = std::min(getAddrLen() ,rhs.getAddrLen());
    int res = memcmp(getAddr() ,rhs.getAddr() ,len);
    if(res < 0){
        return true;
    }else if(res > 0){
        return false;
    }else if(getAddrLen() < rhs.getAddrLen()){
        return true;
    }
    return false;
}

bool Address::operator==(const Address& rhs) const{
    return getAddrLen() == rhs.getAddrLen()
         &&memcmp(getAddr() ,rhs.getAddr() ,getAddrLen()) == 0;
}

bool Address::operator>(const Address& rhs) const{
    if(*this < rhs){
        return false;
    }else if(*this == rhs){
        return false;
    }
    return true;
}

bool Address::operator!=(const Address& rhs) const{
    return !(*this == rhs);
}

IPv4Address::IPv4Address(struct sockaddr_in& addr){
    m_addr = addr;
}

IPv4Address::IPv4Address(uint32_t address ,uint32_t port){
    bzero(&m_addr,sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    m_addr.sin_addr.s_addr = htonl(address);
}

int IPv4Address::getPort() const{
    return ntohs(m_addr.sin_port);
}

void IPv4Address::setPort(int val){
    m_addr.sin_port = htons(val);
}

const sockaddr* IPv4Address::getAddr() const{
    return (sockaddr*)&m_addr;
}
socklen_t IPv4Address::getAddrLen() const{
    return sizeof(m_addr);
}

std::ostream& IPv4Address::insert(std::ostream& os) const{
    uint32_t addr = ntohl(m_addr.sin_addr.s_addr);
    os << ((addr >> 24) & 0xFF) << "."
       << ((addr >> 16) & 0xFF) << "."
       << ((addr >> 8)  & 0xFF) << "."
       << ((addr >> 0)  & 0xFF);
    os << ":" << ntohs(m_addr.sin_port);
    return os;
}

IPv6Address::IPv6Address(struct sockaddr_in6& addr):m_addr(addr){

}

IPv6Address::IPv6Address(const char* addr ,uint32_t port){
    bzero(&m_addr ,sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
    m_addr.sin6_port = htons(port);
    memcpy(&m_addr.sin6_addr.s6_addr,addr,strlen(addr));
}

const sockaddr* IPv6Address::getAddr() const{
    return (sockaddr*)&m_addr;
}
socklen_t IPv6Address::getAddrLen() const{
    return sizeof(m_addr);
}
int IPv6Address::getPort() const{
    return ntohs(m_addr.sin6_port);
}
void IPv6Address::setPort(int val){
    m_addr.sin6_port = htons(val);
}

IPAddress::ptr IPv4Address::broadcastAddress(uint32_t prefix_len){
    if(prefix_len > 32){
        return nullptr;
    }
    sockaddr_in baddr(m_addr);
    baddr.sin_family = AF_INET;
    baddr.sin_addr.s_addr |= htonl(CreateRevMask<uint32_t>(prefix_len));
    return IPv4Address::ptr(new IPv4Address(baddr));
}
IPAddress::ptr IPv4Address::networdAddress(uint32_t prefix_len){
    if(prefix_len > 32){
        return nullptr;
    }
    sockaddr_in nwaddr(m_addr);
    nwaddr.sin_family = AF_INET;
    nwaddr.sin_addr.s_addr |= htonl(CreateRevMask<uint32_t>(prefix_len));
    return IPv4Address::ptr(new IPv4Address(nwaddr));
}

IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len){
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ~htonl(CreateRevMask<uint32_t>(prefix_len));
    return IPv4Address::ptr(new IPv4Address(addr));
}

IPAddress::ptr IPv6Address::broadcastAddress(uint32_t prefix_len) {
    struct sockaddr_in6 baddr(m_addr);
    baddr.sin6_addr.s6_addr[prefix_len / 8] |=
        CreateRevMask<uint8_t>(prefix_len % 8);
    for(int i = prefix_len / 8 + 1; i < 16; ++i) {
        baddr.sin6_addr.s6_addr[i] = 0xff;
    }
    return IPv6Address::ptr(new IPv6Address(baddr));
}

IPAddress::ptr IPv6Address::networdAddress(uint32_t prefix_len) {
    struct sockaddr_in6 baddr(m_addr);
    baddr.sin6_addr.s6_addr[prefix_len / 8] &=
        CreateRevMask<uint8_t>(prefix_len % 8);
    for(int i = prefix_len / 8 + 1; i < 16; ++i) {
        baddr.sin6_addr.s6_addr[i] = 0x00;
    }
    return IPv6Address::ptr(new IPv6Address(baddr));
}

IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len) {
    struct sockaddr_in6 subnet;
    memset(&subnet, 0, sizeof(subnet));
    subnet.sin6_family = AF_INET6;
    subnet.sin6_addr.s6_addr[prefix_len /8] =
        ~CreateRevMask<uint8_t>(prefix_len % 8);

    for(uint32_t i = 0; i < prefix_len / 8; ++i) {
        subnet.sin6_addr.s6_addr[i] = 0xff;
    }
    return IPv6Address::ptr(new IPv6Address(subnet));
}

std::ostream& IPv6Address::insert(std::ostream& os) const{
    os << "[";
    uint16_t* addr = (uint16_t*)(m_addr.sin6_addr.s6_addr);
    int used_zero = 0;
    for(size_t i = 0 ; i < 16 ; ++i){
        if(addr[i] == 0 && !used_zero){
            continue;
        }
        if(i && addr[i-1] == 0 && !used_zero){
            os << ":";
            used_zero = true;
        }
        if(i){
            os << ":";
        }
        os << std::hex << ntohs(addr[i]) <<std::dec;
    }
    if(!used_zero && addr[7] == 0){
        os << "::";
    }
    os << "]:" << ntohs(m_addr.sin6_port);
    return os;
}

static const size_t MAX_PATH_LEN = sizeof((sockaddr_un*)0)->sun_path -1;

// UnixAddress::UnixAddress(){
//     bzero(&m_addr,sizeof(m_addr));
//     m_addr.sun_family = AF_UNIX;
//     m_length = offsetof(sockaddr_un ,sun_path) + MAX_PATH_LEN;
// }

UnixAddress::UnixAddress(const struct sockaddr_un& addr,socklen_t socklen){
    bzero(&m_addr,sizeof(m_addr));
    m_addr = addr;
    m_length = socklen;
}

UnixAddress::UnixAddress(const std::string& path){
    bzero(&m_addr ,sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_length = path.size() + 1;
    if(!path.empty() && path[0] == '\0'){
        --m_length;
    }
    if(m_length > sizeof(m_addr.sun_path)){
        throw std::logic_error("UnixAddress::UnixAddress error.the length of path is too long");
    }
    memcpy(m_addr.sun_path ,path.c_str() ,m_length);
    m_length += offsetof(sockaddr_un ,sun_path);
}

const sockaddr* UnixAddress::getAddr() const {
    return (sockaddr*)&m_addr;
}
socklen_t UnixAddress::getAddrLen() const {
    return m_length;
}
std::ostream& UnixAddress::insert(std::ostream& os) const{
    if(m_length == offsetof(sockaddr_un ,sun_path) && m_addr.sun_path[0] == '\0'){
        return os << "\\0" <<std::string(m_addr.sun_path + 1, 
                    m_length - offsetof(sockaddr_un,sun_path) - 1);
    }
    return os << m_addr.sun_path;
}

UnkownAddress::UnkownAddress(int family){
    bzero(&m_addr,sizeof(m_addr));
    m_addr.sa_family = family;
}

UnkownAddress::UnkownAddress(const struct sockaddr& addr){
    bzero(&m_addr,sizeof(m_addr));
    m_addr = addr;
}

const sockaddr* UnkownAddress::getAddr() const{
    return (sockaddr*)&m_addr;
}
socklen_t UnkownAddress::getAddrLen() const{
    return sizeof(m_addr);
}
std::ostream& UnkownAddress::insert(std::ostream& os) const{
    os << "[UnkownAddress family = "<< m_addr.sa_family << "]";
    return os;
}

} // namespace fepoh
