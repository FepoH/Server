#include "http/http_parser.h"
#include "log/log.h"

using namespace fepoh;
static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test_request(){
    const char data[] = "POST / HTTP/1.1\r\n"
                        "Host:www.baidu.com\r\n"
                        "Content-Length:10\r\n"
                        "Connection: keep-alive\r\n\r\n"
                        "0123456789";
    http::HttpRequestParser hrp;
    std::string tmp = data;
    hrp.execute(&tmp[0],strlen(data));
    http_parser hp;
    hrp.execute(&tmp[0],strlen(data));
    hrp.getData()->dump(std::cout);
}

void test_response(){
    const char data[] = "HTTP/1.1 200 OK"
                        "Accept-Ranges: bytes\r\n"
                        "Cache-Control: no-cache\r\n"
                        "Connection: close\r\n"
                        "Content-Length: 9508\r\n"
                        "Content-Type: text/html\r\n"
                        "Pragma: no-cache\r\n"
                        "Server: BWS/1.1\r\n\r\n"
                        "0123456789\r\n";
    http::HttpResponseParser hrp;
    std::string tmp = data;
    hrp.execute(&tmp[0],strlen(data));
    hrp.getData()->dump(std::cout);
}

int main(){

    test_request();
    std::cout << "--------------------------" <<std::endl;
    test_response();
}