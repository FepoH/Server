#include "http_parser.h"
#include "log/log.h"
#include "config.h"


namespace fepoh{

namespace http{


static fepoh::Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

//请求大小
static fepoh::ConfigVar<uint64_t>::ptr g_http_request_buffer_size =
    fepoh::Config::Lookup<uint64_t>((uint64_t)(4 * 1024), 
                "http.request.buffer_size","http request buffer size");
//请求body大小
static fepoh::ConfigVar<uint64_t>::ptr g_http_request_max_body_size =
    fepoh::Config::Lookup<uint64_t>((uint64_t)(64 * 1024 * 1024),
                "http.request.max_body_size", "http request max body size");
//响应大小
static fepoh::ConfigVar<uint64_t>::ptr g_http_response_buffer_size =
    fepoh::Config::Lookup<uint64_t>((uint64_t)(4 * 1024),
               "http.response.buffer_size", "http response buffer size");
//效应body大小
static fepoh::ConfigVar<uint64_t>::ptr g_http_response_max_body_size =
    fepoh::Config::Lookup<uint64_t>((uint64_t)(64 * 1024 * 1024), 
                "http.response.max_body_size","http response max body size");

static uint64_t s_http_request_buffer_size = 0;
static uint64_t s_http_request_max_body_size = 0;
static uint64_t s_http_response_buffer_size = 0;
static uint64_t s_http_response_max_body_size = 0;

struct __HttpInit__{
        __HttpInit__() {
        s_http_request_buffer_size = g_http_request_buffer_size->getValue();
        s_http_request_max_body_size = g_http_request_max_body_size->getValue();
        s_http_response_buffer_size = g_http_response_buffer_size->getValue();
        s_http_response_max_body_size = g_http_response_max_body_size->getValue();

        g_http_request_buffer_size->addListener(
                [](const uint64_t& ov, const uint64_t& nv){
                s_http_request_buffer_size = nv;
        });

        g_http_request_max_body_size->addListener(
                [](const uint64_t& ov, const uint64_t& nv){
                s_http_request_max_body_size = nv;
        });

        g_http_response_buffer_size->addListener(
                [](const uint64_t& ov, const uint64_t& nv){
                s_http_response_buffer_size = nv;
        });

        g_http_response_max_body_size->addListener(
                [](const uint64_t& ov, const uint64_t& nv){
                s_http_response_max_body_size = nv;
        });
    }
};

static __HttpInit__ __http_init_;



HttpRequestParser::HttpRequestParser():m_error(0)
            ,m_data(new HttpRequest()),m_isFinished(false){
    http_parser_init(&m_parser,HTTP_REQUEST);
    m_parser.data = this;
}

//????
int request_message_begin_cb (http_parser *p)
{
    FEPOH_LOG_DEBUG(s_log_system) << "request_header_field_cb";
    return 0;
}

//首部行:key
int request_header_field_cb (http_parser *p, const char *buf, size_t len)
{
    FEPOH_LOG_DEBUG(s_log_system) << "request_header_field_cb";
    HttpRequestParser* hrp = static_cast<HttpRequestParser*>(p->data);
    std::string field(buf,len);
    hrp->setField(field);
    return 0;
}

//首部行value
int request_header_value_cb (http_parser *p, const char *buf, size_t len)
{
    FEPOH_LOG_DEBUG(s_log_system) << "request_header_value_cb, value is:" << buf;
    std::string value(buf, len);
    HttpRequestParser *parser = static_cast<HttpRequestParser*>(p->data);
    parser->getData()->setHeader(parser->getField(), value);
    return 0;
}

//url
int request_request_url_cb (http_parser *p, const char *buf, size_t len){
    FEPOH_LOG_DEBUG(s_log_system) << "request_request_url_cb, url is:" << buf;
    HttpRequestParser* hrp = static_cast<HttpRequestParser*>(p->data);
    struct http_parser_url uil_parser;
    http_parser_url_init(&uil_parser);
    int rt = http_parser_parse_url(buf,len,0,&uil_parser);
    if(rt != 0){
        FEPOH_LOG_ERROR(s_log_system) << "request_request_url_cb error.uil = " << buf;
    }
    if(uil_parser.field_set &(1 << UF_PATH)){
        hrp->getData()->setPath(std::string(buf + uil_parser.field_data[UF_PATH].off
                    ,uil_parser.field_data[UF_PATH].len));
    }
    if(uil_parser.field_set & (1 << UF_QUERY)){
        hrp->getData()->setQuery(std::string(buf + uil_parser.field_data[UF_QUERY].off
                    ,uil_parser.field_data[UF_QUERY].len));
    }
    if(uil_parser.field_set & (1 << UF_FRAGMENT)){
        hrp->getData()->setFragment(std::string(buf + uil_parser.field_data[UF_FRAGMENT].off,
                    uil_parser.field_data[UF_FRAGMENT].len));
    }
    return 0;
}

//请求没有状态
int request_response_status_cb (http_parser *p, const char *buf, size_t len){
    FEPOH_LOG_ERROR(s_log_system) << "request response status call back error.";
    return 1;
}

//body
int request_body_cb (http_parser *p, const char *buf, size_t len){
    FEPOH_LOG_DEBUG(s_log_system) << "request response status call back";
    HttpRequestParser* hrp = static_cast<HttpRequestParser*>(p->data);
    std::string body(buf,len);
    hrp->getData()->setBody(body); 
    return 0;
}

//头部解析完成
int request_headers_complete_cb (http_parser *p){
    FEPOH_LOG_DEBUG(s_log_system) << "on_request_headers_complete_cb";
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(p->data);
    parser->getData()->setVersion(((p->http_major) << 4) | (p->http_minor));
    parser->getData()->setMethod((HttpMethod)(p->method));
    if(http_should_keep_alive(p)){
        parser->getData()->setClose(false);
    }else{
        parser->getData()->setClose(true);
    }
    parser->setHeadFinish(true);
    //parser->getData()->
    return 0;
}

int request_message_complete_cb (http_parser *p){
    FEPOH_LOG_DEBUG(s_log_system) << "request_message_complete_cb";
    HttpRequestParser* hrp = static_cast<HttpRequestParser*>(p->data);
    hrp->setFinished(true);
    return 0;
}

int request_chunk_header_cb (http_parser *p){
    FEPOH_LOG_DEBUG(s_log_system) << "request_chunk_header_cb";
    return 0;
}


int request_chunk_complete_cb (http_parser *p){
    FEPOH_LOG_DEBUG(s_log_system) << "request_chunk_complete_cb";
    return 0;
}


// static http_parser_settings http_request_setting ={
//     .on_message_begin    = request_message_begin_cb,
//     .on_header_field     = request_header_field_cb,
//     .on_header_value     = request_header_value_cb,
//     .on_url              = request_request_url_cb,
//     .on_status           = request_response_status_cb,
//     .on_body             = request_body_cb,
//     .on_headers_complete = request_headers_complete_cb,
//     .on_message_complete = request_message_complete_cb,
//     .on_chunk_header     = request_chunk_header_cb,
//     .on_chunk_complete   = request_chunk_complete_cb};

static http_parser_settings http_request_setting = {
    .on_message_begin    = request_message_begin_cb,
    .on_url              = request_request_url_cb,
    .on_status           = request_response_status_cb,
    .on_header_field     = request_header_field_cb,
    .on_header_value     = request_header_value_cb,
    .on_headers_complete = request_headers_complete_cb,
    .on_body             = request_body_cb,
    .on_message_complete = request_message_complete_cb,
    .on_chunk_header     = request_chunk_header_cb,
    .on_chunk_complete   = request_chunk_complete_cb};

int HttpRequestParser::execute(char *data, size_t len){
    setError(0);
    m_isFinished = false;
    http_parser_execute(&m_parser,&http_request_setting,data,len);
    size_t nparsed = http_parser_execute(&m_parser, &http_request_setting,data, len);
    if (m_parser.http_errno != 0) {
        FEPOH_LOG_ERROR(s_log_system) << "parse response fail: " << http_errno_name(HTTP_PARSER_ERRNO(&m_parser));
        setError(m_parser.http_errno);
    } else {
        if (nparsed < len) {
            memmove(data, data + nparsed, (len - nparsed));
        }
    }
    return nparsed;
}



HttpResponseParser::HttpResponseParser():m_error(0)
            ,m_data(new HttpResponse()),m_isFinished(false){
    http_parser_init(&m_parser,HTTP_RESPONSE);
    m_parser.data = this;
}

int response_message_begin_cb (http_parser *p){
    FEPOH_LOG_DEBUG(s_log_system) << "response_message_begin_cb";
    return 0;
}

int response_header_field_cb (http_parser *p, const char *buf, size_t len){
    FEPOH_LOG_DEBUG(s_log_system) << "response_header_field_cb";
    HttpResponseParser* hrp = static_cast<HttpResponseParser*>(p->data);
    std::string field(buf,len);
    hrp->setField(field);
    return 0;
}

int response_header_value_cb (http_parser *p, const char *buf, size_t len){
    FEPOH_LOG_DEBUG(s_log_system) << "response_header_value_cb";
    HttpResponseParser* hrp = static_cast<HttpResponseParser*>(p->data);
    std::string value(buf,len);
    hrp->getData()->setHeader(hrp->getField(),value);
    return 0;
}

int response_request_url_cb (http_parser *p, const char *buf, size_t len){
    FEPOH_LOG_DEBUG(s_log_system) << "response_request_url_cb";
    return 0;
}

int response_response_status_cb (http_parser *p, const char *buf, size_t len){
    FEPOH_LOG_DEBUG(s_log_system) << "response_response_status_cb";
    HttpResponseParser* hrp = static_cast<HttpResponseParser*>(p->data);
    hrp->getData()->setStatus(HttpStatus(p->status_code));
    return 0;
}

int response_body_cb (http_parser *p, const char *buf, size_t len){
    FEPOH_LOG_DEBUG(s_log_system) << "response_response_status_cb";
    HttpResponseParser* hrp = static_cast<HttpResponseParser*>(p->data);
    std::string body(buf,len);
    hrp->getData()->setBody(body);
    return 0;
}

int response_headers_complete_cb (http_parser *p){
    HttpResponseParser* hrp = static_cast<HttpResponseParser*>(p->data);
    hrp->getData()->setVersion((p->http_major << 4) | p->http_minor);
    int rt = http_should_keep_alive(p);
    if(rt){
        hrp->getData()->setClose(false);
    }else{
        hrp->getData()->setClose(true);
    }
    return 0;
}


int response_message_complete_cb (http_parser *p){
    FEPOH_LOG_DEBUG(s_log_system) << "response_message_complete_cb";
    HttpResponseParser* hrp = static_cast<HttpResponseParser*>(p->data);
    hrp->setFinished(true);
    return 0;
}

int response_chunk_header_cb (http_parser *p){
    FEPOH_LOG_DEBUG(s_log_system) << "response_chunk_header_cb";
    return 0;
}


int response_chunk_complete_cb (http_parser *p){
    FEPOH_LOG_DEBUG(s_log_system) << "response_chunk_complete_cb";
    return 0;
}


// static http_parser_settings http_response_setting ={
//     .on_message_begin    = response_message_begin_cb,
//     .on_header_field     = response_header_field_cb,
//     .on_header_value     = response_header_value_cb,
//     .on_status           = response_response_status_cb,
//     .on_body             = response_body_cb,
//     .on_headers_complete = response_headers_complete_cb,
//     .on_message_complete = response_message_complete_cb,
//     .on_chunk_header     = response_chunk_header_cb,
//     .on_chunk_complete   = response_chunk_complete_cb,
//     .on_url              = response_request_url_cb
// };



static http_parser_settings http_response_setting = {
    .on_message_begin    = response_message_begin_cb,
    .on_url              = response_request_url_cb,
    .on_status           = response_response_status_cb,
    .on_header_field     = response_header_field_cb,
    .on_header_value     = response_header_value_cb,
    .on_headers_complete = response_headers_complete_cb,
    .on_body             = response_body_cb,
    .on_message_complete = response_message_complete_cb,
    .on_chunk_header     = response_chunk_header_cb,
    .on_chunk_complete   = response_chunk_complete_cb};

int HttpResponseParser::execute(char *data, size_t len){
    size_t offset = http_parser_execute(&m_parser,&http_response_setting ,data, len);
    memmove(data, data + offset, (len - offset));
    return offset;
}




}
}



/*request*/

// void OnRequestUri(void *data, const char *at, size_t length){
// }

// void OnRequestHeaderDone(void *data, const char *at, size_t length){
// }

// void OnRequestFragment(void *data, const char *at, size_t length){
//     HttpRequestParser* hrp = (HttpRequestParser*)data;
//     std::string fr(at,length);
//     hrp->getData()->setFragment(fr);
// }

// void OnRequestPath(void *data, const char *at, size_t length){
//     HttpRequestParser* hrp = (HttpRequestParser*)data;
//     std::string path(at,length);
//     hrp->getData()->setPath(path);
// }

// void OnRequestHttpVersion(void *data, const char *at, size_t length){
//     HttpRequestParser* hrp = (HttpRequestParser*)data;
//     uint8_t version;
//     if(strncmp("HTTP/1.1",at,length) == 0){
//         version = 0x11;
//     }else if(strncmp("HTTP/1.0",at,length)){
//         version = 0x10;
//     }else{
//         FEPOH_LOG_ERROR(s_log_system) << "parser http version error.at = "
//                 << at;
//         hrp->setError(1001);
//         return ;
//     }
//     hrp->getData()->setVersion(version);
// }

// void OnRequestQueryStr(void *data, const char *at, size_t length){
//     HttpRequestParser* hrp = (HttpRequestParser*)data;
//     std::string query(at,length);
//     hrp->getData()->setQuery(query);
// }

// void OnRequestField(void *data, const char *field, size_t flen, const char *value, size_t vlen){
//     HttpRequestParser* hrp = (HttpRequestParser*)data;
//     if(flen <= 0){
//         FEPOH_LOG_WARN(s_log_system) << "invalid field len = " << flen; 
//         return ;
//     }
//     std::string key(field,flen);
//     std::string val(value,vlen);
//     hrp->getData()->setHead(key,val);
// }

// HttpRequestParser::HttpRequestParser():m_error(0),m_data(new HttpRequest()){
//     http_parser_init(&m_parser,HTTP_REQUEST);
//     m_parser.data = this;
//     m_parser.http_field = OnRequestField;
//     m_parser.request_method = OnRequestMethod;
//     m_parser.request_uri = OnRequestUri;
//     m_parser.fragment = OnRequestFragment;
//     m_parser.request_path = OnRequestPath;
//     m_parser.query_string = OnRequestQueryStr;
//     m_parser.http_version = OnRequestHttpVersion;
//     m_parser.header_done = OnRequestHeaderDone;
// }


/*response*/
// void OnResponsReasonPhrase(void *data, const char *at, size_t length){
//     HttpResponseParser* hcp = static_cast<HttpResponseParser*>(data);
//     std::string reason(at,length);
//     hcp->getData()->setReason(reason);
// }

// void OnResponsStatusCode(void *data, const char *at, size_t length){
//     HttpResponseParser* hcp = static_cast<HttpResponseParser*>(data);
//     HttpStatus status = (HttpStatus)(atoi(at));
//     hcp->getData()->setStatus(status);
// }

// void OnResponsChunkSize(void *data, const char *at, size_t length){
// }

// void OnResponsHttpVersion(void *data, const char *at, size_t length){
//     HttpResponseParser* hrp = static_cast<HttpResponseParser*>(data);
//     uint8_t version;
//     if(strncmp("HTTP/1.1",at,length) == 0){
//         version = 0x11;
//     }else if(strncmp("HTTP/1.0",at,length)){
//         version = 0x10;
//     }else{
//         FEPOH_LOG_ERROR(s_log_system) << "parser http version error.at = "
//                 << at;
//         hrp->setError(1001);
//         return ;
//     }
//     hrp->getData()->setVersion(version);
// }

// void OnResponsHeaderDone(void *data, const char *at, size_t length){
// }

// void OnResponsLastChunk(void *data, const char *at, size_t length){
// }


// void OnResponseField(void *data, const char *field, size_t flen, const char *value, size_t vlen){
//     HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
//     if(flen == 0) {
//         FEPOH_LOG_WARN(s_log_system) << "invalid http response field length == 0";
//         //parser->setError(1002);
//         return;
//     }
//     parser->getData()->setHead(std::string(field, flen)
//                                 ,std::string(value, vlen));
// }

// HttpResponseParser::HttpResponseParser():m_error(0),m_data(new HttpResponse()){
//     httpclient_parser_init(&m_parser,HTTP_RESPONSE);
//     m_parser.data = this;
//     m_parser.http_field = OnResponseField;

//     m_parser.reason_phrase = OnResponsReasonPhrase;
//     m_parser.status_code = OnResponsStatusCode;
//     m_parser.chunk_size = OnResponsChunkSize;
//     m_parser.http_version = OnResponsHttpVersion;
//     m_parser.header_done = OnResponsHeaderDone;
//     m_parser.last_chunk = OnResponsLastChunk;
// }