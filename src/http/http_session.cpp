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
    int max_head_size = HttpResponseParser::GetResponseHeadBufSize();
    int max_size = HttpRequestParser::GetRequestBodyBufSize() + max_head_size;
    HttpRequestParser::ptr parser(new HttpRequestParser());
    std::shared_ptr<char> buffer(new char[max_head_size],[](char* ptr){
        delete[] ptr;
    });
    char* data = buffer.get();
    int offset = 0;
    int total_length = 0;
    do{
        int len = read(data + offset,max_head_size - offset);
        if(len <= 0 ){
            close();
            return nullptr;
        }
        total_length += len;
        len += offset;
        size_t nparser = parser->execute(data,len);
        if(parser->getError()){
            close();
            return nullptr;
        }
        offset = len - nparser;
        if(offset == max_size){
            FEPOH_LOG_ERROR(s_log_system) << "offset = max_size";
            close();
            return nullptr;
        }
        if(parser->getError() & HPE_INVALID_CONSTANT){
            FEPOH_LOG_ERROR(s_log_system) << "totol length = " << total_length 
                            << ",max size" << max_size; 
            close();
            return nullptr;
        }

        if((!parser->getHeadFinish())&&(total_length > max_head_size * 2)){
            FEPOH_LOG_ERROR(s_log_system) << "totol length = " << total_length 
                            << ",max head size" << max_head_size; 
            close();
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

