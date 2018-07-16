// net
// TODO: socket:get, epoll:*, udp:*, ip6:*

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "lua_pd.h"

//

#define LUA_MT_NET_IP4_TCP_SOCKET "mt.net.ip4.tcp.socket"
#define LUA_MT_NET_IP6_TCP_SOCKET "mt.net.ip6.tcp.socket"
#define LUA_MT_NET_UNIX_SOCKET "mt.net.unix.socket"
#define LUA_MT_NET_EPOLL "mt.net.epoll"

#define IP4_LOCALHOST "127.0.0.1"
#define IP4_TCP_SOCKET_BIND_DEFAULT_INTERFACE IP4_LOCALHOST
#define IP4_TCP_SOCKET_BIND_DEFAULT_PORT 0
#define IP4_TCP_SOCKET_LISTEN_DEFAULT_BACKLOG 1024
#define IP4_TCP_SOCKET_RECV_BUFFER_LEN 4096

#define IP6_LOCALHOST "::1"
#define IP6_TCP_SOCKET_BIND_DEFAULT_INTERFACE IP6_LOCALHOST
#define IP6_TCP_SOCKET_BIND_DEFAULT_PORT 0
#define IP6_TCP_SOCKET_LISTEN_DEFAULT_BACKLOG 1024
#define IP6_TCP_SOCKET_RECV_BUFFER_LEN 4096

#define UNIX_SOCKET_BIND_DEFAULT_ADDR "/tmp/lua-default.sock"
#define UNIX_SOCKET_BIND_DEFAULT_MODE 666
#define UNIX_SOCKET_LISTEN_DEFAULT_BACKLOG 1024
#define UNIX_SOCKET_RECV_BUFFER_LEN 4096

#define LUA_EPOLL_DEFAULT_QUEUE_SIZE 65535

//

#define lua_pushsockerr( L, fd ) { \
    int optval; \
    socklen_t optlen = sizeof(optval); \
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &optval, &optlen); \
    lua_pushstring(L, strerror(optval)); \
    lua_pushnumber(L, optval); \
}

#define lua_epoll_pcall( L, argsn ) { \
    int r = lua_pcall(L, argsn, 0, 0); /* nresults=0, no results needed */ \
    if ( r != LUA_OK ) { /* callback call error, index -1 ⇒ es */ \
        lua_pushvalue(L, 6); /* arg #6 is onerror */ \
        lua_insert(L, -2); \
        lua_pushnil(L); /* nil */ \
        lua_insert(L, -2); \
        lua_pushnumber(L, -1); /* en */ \
        lua_call(L, 3, 0); /* onerror(nil, es, en) */ \
    } \
}

//

typedef struct lua_ud_socket {
    int fd;
    uint64_t id; // socket unique id (unique in process scope)
} lua_ud_socket;

typedef struct lua_ud_unix_socket {
    int fd;
    uint64_t id;
    char *addr;
} lua_ud_unix_socket;

//

LUAMOD_API int luaopen_net( lua_State *L );

static int lua_net_epoll( lua_State *L );
static int lua_net_epoll_start( lua_State *L );
static int lua_net_epoll_gc( lua_State *L );
static int lua_net_epoll_watch( lua_State *L );
static int lua_net_epoll_unwatch( lua_State *L );

static int lua_net_ip4_tcp_nslookup( lua_State *L );
static int lua_net_ip6_tcp_nslookup( lua_State *L );

static int lua_net_ip4_tcp_socket( lua_State *L );
static int lua_net_ip4_tcp_socket_gc( lua_State *L );
static int lua_net_ip4_tcp_socket_fd( lua_State *L );
static int lua_net_ip4_tcp_socket_id( lua_State *L );
static int lua_net_ip4_tcp_socket_bind( lua_State *L );
static int lua_net_ip4_tcp_socket_connect( lua_State *L );
static int lua_net_ip4_tcp_socket_listen( lua_State *L );
static int lua_net_ip4_tcp_socket_accept( lua_State *L );
static int lua_net_ip4_tcp_socket_recv( lua_State *L );
static int lua_net_ip4_tcp_socket_send( lua_State *L );
static int lua_net_ip4_tcp_socket_shutdown( lua_State *L );
static int lua_net_ip4_tcp_socket_set( lua_State *L );

static int lua_net_ip6_tcp_socket( lua_State *L );
static int lua_net_ip6_tcp_socket_gc( lua_State *L );
static int lua_net_ip6_tcp_socket_fd( lua_State *L );
static int lua_net_ip6_tcp_socket_id( lua_State *L );
static int lua_net_ip6_tcp_socket_bind( lua_State *L );
static int lua_net_ip6_tcp_socket_connect( lua_State *L );
static int lua_net_ip6_tcp_socket_listen( lua_State *L );
static int lua_net_ip6_tcp_socket_accept( lua_State *L );
static int lua_net_ip6_tcp_socket_recv( lua_State *L );
static int lua_net_ip6_tcp_socket_send( lua_State *L );
static int lua_net_ip6_tcp_socket_shutdown( lua_State *L );
static int lua_net_ip6_tcp_socket_set( lua_State *L );

static int lua_net_unix_socket( lua_State *L );
static int lua_net_unix_socket_gc( lua_State *L );
static int lua_net_unix_socket_fd( lua_State *L );
static int lua_net_unix_socket_id( lua_State *L );
static int lua_net_unix_socket_bind( lua_State *L );
static int lua_net_unix_socket_connect( lua_State *L );
static int lua_net_unix_socket_listen( lua_State *L );
static int lua_net_unix_socket_accept( lua_State *L );
static int lua_net_unix_socket_recv( lua_State *L );
static int lua_net_unix_socket_send( lua_State *L );
static int lua_net_unix_socket_shutdown( lua_State *L );
static int lua_net_unix_socket_set( lua_State *L );

static uint64_t inc_id( void );
long int oct2dec( long int octal );

//

static const luaL_Reg __index[] = {
    {"epoll", lua_net_epoll},
    {NULL, NULL}
};

static const luaL_Reg __epoll_index[] = {
    {"start", lua_net_epoll_start},
    {"stop", lua_net_epoll_gc},
    {"watch", lua_net_epoll_watch},
    {"unwatch", lua_net_epoll_unwatch},
    {NULL, NULL}
};

static const luaL_Reg __ip4_index[] = {
    {NULL, NULL}
};

static const luaL_Reg __ip4_tcp_index[] = {
    {"nslookup", lua_net_ip4_tcp_nslookup},
    {"socket", lua_net_ip4_tcp_socket},
    {NULL, NULL}
};

static const luaL_Reg __ip4_tcp_socket_index[] = {
    {"close", lua_net_ip4_tcp_socket_gc},
    {"fd", lua_net_ip4_tcp_socket_fd},
    {"id", lua_net_ip4_tcp_socket_id},
    {"bind", lua_net_ip4_tcp_socket_bind},
    {"connect", lua_net_ip4_tcp_socket_connect},
    {"listen", lua_net_ip4_tcp_socket_listen},
    {"accept", lua_net_ip4_tcp_socket_accept},
    {"recv", lua_net_ip4_tcp_socket_recv},
    {"send", lua_net_ip4_tcp_socket_send},
    {"shutdown", lua_net_ip4_tcp_socket_shutdown},
    {"set", lua_net_ip4_tcp_socket_set},
    {NULL, NULL}
};

static const luaL_Reg __ip6_index[] = {
    {NULL, NULL}
};

static const luaL_Reg __ip6_tcp_index[] = {
    {"nslookup", lua_net_ip6_tcp_nslookup},
    {"socket", lua_net_ip6_tcp_socket},
    {NULL, NULL}
};

static const luaL_Reg __ip6_tcp_socket_index[] = {
    {"close", lua_net_ip6_tcp_socket_gc},
    {"fd", lua_net_ip6_tcp_socket_fd},
    {"id", lua_net_ip6_tcp_socket_id},
    {"bind", lua_net_ip6_tcp_socket_bind},
    {"connect", lua_net_ip6_tcp_socket_connect},
    {"listen", lua_net_ip6_tcp_socket_listen},
    {"accept", lua_net_ip6_tcp_socket_accept},
    {"recv", lua_net_ip6_tcp_socket_recv},
    {"send", lua_net_ip6_tcp_socket_send},
    {"shutdown", lua_net_ip6_tcp_socket_shutdown},
    {"set", lua_net_ip6_tcp_socket_set},
    {NULL, NULL}
};

static const luaL_Reg __unix_index[] = {
    {"socket", lua_net_unix_socket},
    {NULL, NULL}
};

static const luaL_Reg __unix_socket_index[] = {
    {"close", lua_net_unix_socket_gc},
    {"fd", lua_net_unix_socket_fd},
    {"id", lua_net_unix_socket_id},
    {"bind", lua_net_unix_socket_bind},
    {"connect", lua_net_unix_socket_connect},
    {"listen", lua_net_unix_socket_listen},
    {"accept", lua_net_unix_socket_accept},
    {"recv", lua_net_unix_socket_recv},
    {"send", lua_net_unix_socket_send},
    {"shutdown", lua_net_unix_socket_shutdown},
    {"set", lua_net_unix_socket_set},
    {NULL, NULL}
};
