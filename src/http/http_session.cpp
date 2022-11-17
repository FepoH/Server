#include "http_session.h"
#include "socket_stream.h"
#include "http_parser.h"
#include "log/log.h"


namespace fepoh{
namespace http{

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

HttpSession::HttpSession(Socket::ptr sock,bool owner)
    :SocketStream(sock,owner){

}

HttpRequest::ptr HttpSession::recvRequrest(){
    //TODO
    int max_size = HttpRequestParser::GetRequestHeadBufSize() * 3;
    HttpRequestParser::ptr parser(new HttpRequestParser());
    std::shared_ptr<char> buffer(new char[max_size],[](char* ptr){
        delete[] ptr;
    });
    char* data = buffer.get();
    int offset = 0;
    do{
        int len = read(data + offset,max_size - offset);
        if(len <= 0 ){
            return nullptr;
        }
        len += offset;
        size_t nparser = parser->execute(data,len);
        if(parser->getError()){
            return nullptr;
        }
        offset = len - nparser;
        if(offset == max_size){
            FEPOH_LOG_ERROR(s_log_system) << "offset = max_size";
            return nullptr;
        }
        if(parser->isFinished()){
            break;
        }
    }while(true);
 
    return parser->getData();
}

int HttpSession::sendResponse(HttpResponse::ptr response){
    std::stringstream ss;
    response->dump(ss);
    std::string str = ss.str();
    return writeFixSize(str.c_str(),str.size());

}



}
}//namespace