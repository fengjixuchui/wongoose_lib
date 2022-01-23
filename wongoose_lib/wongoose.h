#pragma once
#include<stdint.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

struct wg_iobuf {
	//CRITICAL_SECTION lock;
	unsigned char* buf;  // Pointer to stored data
	size_t len;          // Current number of bytes
	size_t size;         // Total size available
};

int wg_iobuf_init(wg_iobuf*, size_t);
bool wg_iobuf_resize(wg_iobuf*, size_t);
void wg_iobuf_free(wg_iobuf*);
size_t wg_iobuf_add(wg_iobuf* io, size_t ofs, const void* buf, size_t len, size_t chunk_size);
size_t wg_iobuf_del(wg_iobuf*, size_t ofs, size_t len);


struct wg_addr {
	uint16_t port;    // TCP or UDP port in network byte order
	uint32_t ip;      // IP address in network byte order
};

union wg_data {
	void* ptr;
	int fd;
	uint32_t u32;
	uint64_t u64;
	SOCKET sock; /* Windows specific */
	HANDLE hnd;  /* Windows specific */
};

struct wg_connection;

typedef void (*wg_event_handler_t)(wg_connection*, int ev, void* ev_data, void* fn_data);

struct wg_connection
{
	HANDLE mgr;
	wg_data data;
	wg_iobuf recv;
	struct wg_addr addr;
	wg_event_handler_t fn;       // User-specified event handler function
	void* fn_data;               // User-specified function parameter
	wg_event_handler_t pfn;      // Protocol-specific handler function
	void* pfn_data;              // Protocol-specific function parameter
	bool is_client;
	bool is_websocket;
	bool is_udp;
};
void wg_call(wg_connection* c, int ev, void* ev_data);
void wg_error(wg_connection* c, const char* fmt, ...);

enum {
	WG_EV_ERROR,       // Error                        char *error_message
	WG_EV_RESOLVE,     // Host name is resolved        NULL
	WG_EV_LISTEN,      // Listen on local port success wg_addr
	WG_EV_CONNECT,     // Connection established       NULL
	WG_EV_ACCEPT,      // Connection accepted          NULL
	WG_EV_RECV,        // Data received from socket    mg_str *
	WG_EV_CLOSE,       // Connection closed            NULL
	WG_EV_HTTP_MSG,    // HTTP request/response        mg_http_message *
	WG_EV_HTTP_CHUNK,  // HTTP chunk (partial msg)     mg_http_message *
	WG_EV_WS_OPEN,     // Websocket handshake done     mg_http_message *
	WG_EV_WS_MSG,      // Websocket msg, text or bin   mg_ws_message *
	WG_EV_WS_CTL,      // Websocket control msg        mg_ws_message *
};

HANDLE wg_mgr_init();
void wg_mgr_runloop(HANDLE mgr);
void wg_mgr_close(HANDLE mgr);
void wg_close_conn(wg_connection* c);
wg_connection* wg_listen(HANDLE mgr, const char* url, wg_event_handler_t fn, void* fn_data);
wg_connection* wg_connect(HANDLE mgr, const char* url, wg_event_handler_t fn, void* fn_data);
void wg_straddr(wg_connection* conn, char* buf, size_t len);
bool wg_send(wg_connection* conn, const void* buf, size_t len);
int wg_printf(wg_connection* conn, const char* fmt, ...);
void wg_ntoa(wg_addr* addr, char* buf, size_t len);

struct wg_str {
	const char* ptr;  // Pointer to string data
	size_t len;       // String len
};
#define WG_NULL_STR \
  { NULL, 0 }
// Using macro to avoid shadowing C++  constructor, see #1298
#define wg_str(s) wg_str_s(s)

wg_str wg_str(const char* s);
wg_str wg_str_n(const char* s, size_t n);
int wg_lower(const char* s);
int wg_ncasecmp(const char* s1, const char* s2, size_t len);
int wg_casecmp(const char* s1, const char* s2);
int wg_vcmp(const  wg_str* s1, const char* s2);
int wg_vcasecmp(wg_str* str1, const char* str2);
int wg_strcmp(const  wg_str str1, const  wg_str str2);
wg_str wg_strstrip(const  wg_str s);
wg_str wg_strdup(const  wg_str s);
const char* wg_strstr(const  wg_str haystack, const  wg_str needle);

uint16_t wg_ntohs(uint16_t net);

#define wg_htons(x) wg_ntohs(x)
#define wg_htonl(x) wg_ntohl(x)

#define WG_MAX_HTTP_HEADERS 40


struct wg_http_header {
	wg_str name;
	wg_str value;
};

struct wg_http_message
{
	wg_str method, uri, query, proto;             // Request/response line
	wg_http_header headers[WG_MAX_HTTP_HEADERS];  // Headers
	wg_str body;                                  // Body
	wg_str head;                                  // Request + headers
	wg_str chunk;    // Chunk for chunked encoding,  or partial body
	wg_str message;  // Request + headers + body
};



wg_connection* wg_http_connect(HANDLE mgr, const char* url, wg_event_handler_t fn, void* fn_data);
void wg_http_request(wg_connection*, const char* url, const char* headers, const char* body_fmt, ...);
void wg_http_request(wg_connection*, const char* url, const char* headers, const char* body, int size);
void wg_http_bauth(wg_connection*, const char* url, const char* headers, const char* user, const char* pass, const char* fmt, ...);
void wg_http_bauth(wg_connection*, const char* url, const char* headers, const char* user, const char* pass, const char* body, int size);
int wg_http_parse(const char* s, size_t len, wg_http_message*);
int wg_http_get_request_len(const unsigned char* buf, size_t buf_len);
wg_connection* wg_http_listen(HANDLE mgr, const char* url, wg_event_handler_t fn, void* fn_data);
void wg_http_serve_file(wg_connection*, wg_http_message* hm, const char* path);
void wg_http_serve_dir(wg_connection*, wg_http_message* hm, const char* path);
void wg_http_reply(wg_connection*, int status_code, const char* headers, const char* body_fmt, ...);
void wg_http_reply(wg_connection*, int status_code, const char* headers, const char* body, int size);
wg_str* wg_http_get_header(wg_http_message*, const char* name);
int wg_http_get_var(const wg_str*, const char* name, char*, size_t);
int wg_url_decode(const char* s, size_t n, char* to, size_t to_len, int form);
size_t wg_url_encode(const char* s, size_t n, char* buf, size_t len);
void wg_http_creds(wg_http_message*, char*, size_t, char*, size_t);
bool wg_http_match_uri(const wg_http_message*, const char* glob);
wg_str wg_http_get_header_var(wg_str s, wg_str v);


struct wg_ws_message {
	wg_str data;		 // Websocket message data
	uint8_t flags;       // Websocket message flags
};

#define WEBSOCKET_OP_CONTINUE 0
#define WEBSOCKET_OP_TEXT 1
#define WEBSOCKET_OP_BINARY 2
#define WEBSOCKET_OP_CLOSE 8
#define WEBSOCKET_OP_PING 9
#define WEBSOCKET_OP_PONG 10

wg_connection* wg_ws_connect(HANDLE mgr, const char* url, wg_event_handler_t fn, void* fn_data, const char* fmt, ...);
void wg_ws_upgrade(wg_connection*, wg_http_message*, const char* fmt, ...);
size_t wg_ws_send(wg_connection*, const char* buf, size_t len, int op);


