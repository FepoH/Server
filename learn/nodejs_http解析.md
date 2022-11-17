# nodejs http解析工具使用


## 主要结构体:


### http_parser
```C++
struct http_parser {
  /** PRIVATE **/
  uint64_t content_length;  //长度,如果没有Content-Length,则值为-1

  /** READ-ONLY **/
  unsigned short http_major;    //大版本
  unsigned short http_minor;    //小版本
  unsigned int status_code;     //状态码,响应报文可用
  unsigned int method;          //请求方法,请求报文可用
  unsigned int http_errno;      //错误码
  unsigned int upgrade;         //...


  void *data;                   //数据,可指向socket文件描述符或自定义数据结构
};
```
### http_parser_settings
用于设置回调函数,在exec接口中可用
```C++
struct http_parser_settings {
  http_cb      on_message_begin;    //...
  http_data_cb on_url;              //解析url
  http_data_cb on_status;           //解析状态码
  http_data_cb on_header_field;     //解析首部行key
  http_data_cb on_header_value;     //解析首部行value
  http_cb      on_headers_complete; //头部解析完成回调
  http_data_cb on_body;             //解析body
  http_cb      on_message_complete; //数据解析完成回调
  /* When on_chunk_header is called, the current chunk length is stored
   * in parser->content_length.
   */
  http_cb      on_chunk_header;     //Content-Length解析完成,可取parser中的body长度
  http_cb      on_chunk_complete;   //...
};

```
 
### http_parser_url
```C++
enum http_parser_url_fields
  { UF_SCHEMA           = 0
  , UF_HOST             = 1
  , UF_PORT             = 2
  , UF_PATH             = 3
  , UF_QUERY            = 4
  , UF_FRAGMENT         = 5
  , UF_USERINFO         = 6
  , UF_MAX              = 7
  };

struct http_parser_url {
  uint16_t field_set;           /* Bitmask of (1 << UF_*) values */
  uint16_t port;                /* Converted UF_PORT string */

  struct {
    uint16_t off;               /* Offset into buffer in which field starts */
    uint16_t len;               /* Length of run in buffer */
  } field_data[UF_MAX];
};

//使用
static int on_request_url_cb(http_parser *p, const char *buf, size_t len) {
    struct http_parser_url url_parser;
    http_parser_url_init(&url_parser);
    http_parser_parse_url(buf, len, 0, &url_parser);
    if (url_parser.field_set & (1 << UF_PATH)) {
        //path
        std::string(buf + url_parser.field_data[UF_PATH].off,url_parser.field_data[UF_PATH].len);
    }
    if (url_parser.field_set & (1 << UF_QUERY)) {
        //query
        std::string(buf + url_parser.field_data[UF_QUERY].off,url_parser.field_data[UF_QUERY].len);
    }
    if (url_parser.field_set & (1 << UF_FRAGMENT)) {
        //fragment
        std::string(buf + url_parser.field_data[UF_FRAGMENT].off,url_parser.field_data[UF_FRAGMENT].len);
    }
}
```
### API
```C++
int http_parser_parse_url(const char *buf, size_t buflen,int is_connect,struct http_parser_url *u);
//type:HTTP_REQUEST  HTTP_RESPONSE
void http_parser_init(http_parser *parser, enum http_parser_type type);
//初始化为0
void http_parser_settings_init(http_parser_settings *settings);
//返回0:close,返回非零:keep-live
int http_should_keep_alive(const http_parser *parser);
//初始化为0
void http_parser_url_init(struct http_parser_url *u);
//检查是否完毕
int http_body_is_final(const http_parser *parser);
//执行,主逻辑函数
size_t http_parser_execute(http_parser *parser,
                           const http_parser_settings *settings,
                           const char *data,
                           size_t len);
//还有其他函数,详见https://github.com/nodejs/http-parser/blob/main/http_parser.h
```




