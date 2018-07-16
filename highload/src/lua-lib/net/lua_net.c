// net

#include "lua_net.h"

#include "lua_net_epoll.c"
#include "lua_net_ip4_tcp.c"
#include "lua_net_ip6_tcp.c"
#include "lua_net_unix.c"

//

LUAMOD_API int luaopen_net( lua_State *L ) {
    lua_newmt(L, LUA_MT_NET_IP4_TCP_SOCKET, __ip4_tcp_socket_index, lua_net_ip4_tcp_socket_gc);
    lua_newmt(L, LUA_MT_NET_IP6_TCP_SOCKET, __ip6_tcp_socket_index, lua_net_ip6_tcp_socket_gc);
    lua_newmt(L, LUA_MT_NET_UNIX_SOCKET, __unix_socket_index, lua_net_unix_socket_gc);
    lua_newmt(L, LUA_MT_NET_EPOLL, __epoll_index, lua_net_epoll_gc);

    luaL_newlib(L, __index);

    lua_newtable(L);
        #include "lua_net_flags.c"
    lua_setfield(L, -2, "f");

    lua_newtable(L);
        #include "lua_net_errors.c"
    lua_setfield(L, -2, "e");

    luaL_newlib(L, __ip4_index);
        luaL_newlib(L, __ip4_tcp_index);
        lua_setfield(L, -2, "tcp");
    lua_setfield(L, -2, "ip4");

    luaL_newlib(L, __ip6_index);
        luaL_newlib(L, __ip6_tcp_index);
        lua_setfield(L, -2, "tcp");
    lua_setfield(L, -2, "ip6");

    luaL_newlib(L, __unix_index);
    lua_setfield(L, -2, "unix");

    return 1;
}

//

static uint64_t inc_id( void ) {
    static volatile uint64_t id = 0;
    return __sync_add_and_fetch(&id, 1);
}

long int oct2dec( long int octal ) {
    long int decimal = 0;
    int i = 0;
    while ( octal ) {
        decimal += (octal % 10) * pow(8, i++);
        octal /= 10;
    }
    return decimal;
}
