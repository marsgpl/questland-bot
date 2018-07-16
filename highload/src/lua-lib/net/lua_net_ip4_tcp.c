//

static int lua_net_ip4_tcp_nslookup( lua_State *L ) {
    const char *node = luaL_checkstring(L, 1); // domain or ip

    struct addrinfo hints, *res, *rp;
    char ip[INET_ADDRSTRLEN];
    int r, i;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    r = getaddrinfo(node, NULL, &hints, &res);
    if ( r != 0 ) lua_fail(L, gai_strerror(r), r);

    lua_newtable(L);

    for ( rp=res, i=1; rp != NULL; ++i, rp=rp->ai_next ) {
        inet_ntop(hints.ai_family, &((struct sockaddr_in *)(rp->ai_addr))->sin_addr, ip, INET_ADDRSTRLEN);
        lua_pushnumber(L, i);
        lua_pushstring(L, ip);
        lua_settable(L, -3);
    }

    freeaddrinfo(res);

    return 1;
}

static int lua_net_ip4_tcp_socket( lua_State *L ) {
    int nonblock = luaL_optinteger(L, 1, 0) ? SOCK_NONBLOCK : 0;
    int cloexec = luaL_optinteger(L, 2, 1) ? SOCK_CLOEXEC : 0;

    lua_ud_socket *sock = (lua_ud_socket *)lua_newuserdata(L, sizeof(lua_ud_socket));

    if ( !sock ) {
        lua_fail(L, "lua_ud_socket alloc failed", 0);
    }

    sock->id = inc_id();
    sock->fd = socket(AF_INET, SOCK_STREAM | nonblock | cloexec, 0);

    if ( sock->fd < 0 ) {
        lua_errno(L);
    }

    luaL_setmetatable(L, LUA_MT_NET_IP4_TCP_SOCKET);

    lua_pushinteger(L, sock->fd);

    return 2;
}

static int lua_net_ip4_tcp_socket_gc( lua_State *L ) {
    lua_ud_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_IP4_TCP_SOCKET);

    if ( sock->fd > -1 ) {
        close(sock->fd);
        sock->fd = -1;
    }

    return 0;
}

static int lua_net_ip4_tcp_socket_fd( lua_State *L ) {
    lua_ud_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_IP4_TCP_SOCKET);

    lua_pushinteger(L, sock->fd);
    return 1;
}

static int lua_net_ip4_tcp_socket_id( lua_State *L ) {
    lua_ud_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_IP4_TCP_SOCKET);

    lua_pushnumber(L, sock->id);
    return 1;
}

static int lua_net_ip4_tcp_socket_bind( lua_State *L ) {
    lua_ud_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_IP4_TCP_SOCKET);

    const char *interface = luaL_optstring(L, 2, IP4_TCP_SOCKET_BIND_DEFAULT_INTERFACE);
    int port = luaL_optinteger(L, 3, IP4_TCP_SOCKET_BIND_DEFAULT_PORT);

    if ( sock->fd < 0 ) {
        lua_fail(L, "socket is dead", 0);
    }

    int r;
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    r = inet_pton(addr.sin_family, interface, (void *)&addr.sin_addr);

    if ( r == -1 ) {
        lua_errno(L);
    } else if ( r == 0 ) {
        lua_fail(L, "arg#1 must contain a valid interface", 0);
    }

    r = bind(sock->fd, (struct sockaddr *)&addr, sizeof(addr));

    if ( r == -1 ) {
        lua_errno(L);
    } else {
        lua_settop(L, 1);
        return 1;
    }
}

static int lua_net_ip4_tcp_socket_connect( lua_State *L ) {
    lua_ud_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_IP4_TCP_SOCKET);

    const char *interface = luaL_optstring(L, 2, IP4_TCP_SOCKET_BIND_DEFAULT_INTERFACE);
    int port = luaL_optinteger(L, 3, IP4_TCP_SOCKET_BIND_DEFAULT_PORT);

    if ( sock->fd < 0 ) {
        lua_fail(L, "socket is dead", 0);
    }

    int r;
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    r = inet_pton(addr.sin_family, interface, (void *)&addr.sin_addr);

    if ( r == -1 ) {
        lua_errno(L);
    } else if ( r == 0 ) {
        lua_fail(L, "arg#1 must contain a valid ip address", 0);
    }

    r = connect(sock->fd, (struct sockaddr *)&addr, sizeof(addr));

    if ( r == -1 ) {
        lua_errno(L);
    } else {
        lua_settop(L, 1);
        return 1;
    }
}

static int lua_net_ip4_tcp_socket_listen( lua_State *L ) {
    lua_ud_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_IP4_TCP_SOCKET);

    int backlog = luaL_optinteger(L, 2, IP4_TCP_SOCKET_LISTEN_DEFAULT_BACKLOG);

    if ( sock->fd < 0 ) {
        lua_fail(L, "socket is dead", 0);
    }

    int r = listen(sock->fd, backlog);

    if ( r == -1 ) {
        lua_errno(L);
    } else {
        lua_settop(L, 1);
        return 1;
    }
};

static int lua_net_ip4_tcp_socket_accept( lua_State *L ) {
    lua_ud_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_IP4_TCP_SOCKET);
    int nonblock = luaL_optinteger(L, 2, 0) ? SOCK_NONBLOCK : 0;

    if ( sock->fd < 0 ) {
        lua_fail(L, "socket is dead", 0);
    }

    int fd = accept(sock->fd, NULL, NULL);

    if ( fd < 0 ) {
        lua_errno(L);
    }

    if ( nonblock ) { // crunch for missing accept4
        if ( fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1 ) {
            lua_errno(L);
        }
    }

    lua_ud_socket *client = (lua_ud_socket *)lua_newuserdata(L, sizeof(lua_ud_socket));

    if ( !client ) {
        close(fd);
        lua_fail(L, "lua_ud_socket alloc failed", 0);
    }

    client->id = inc_id();
    client->fd = fd;

    luaL_setmetatable(L, LUA_MT_NET_IP4_TCP_SOCKET);

    lua_pushinteger(L, fd);

    return 2;
};

static int lua_net_ip4_tcp_socket_recv( lua_State *L ) {
    lua_ud_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_IP4_TCP_SOCKET);

    if ( sock->fd < 0 ) {
        lua_fail(L, "socket is dead", 0);
    }

    char read_buff[IP4_TCP_SOCKET_RECV_BUFFER_LEN];
    ssize_t n;

    do { n = recv(sock->fd, (void *)read_buff, IP4_TCP_SOCKET_RECV_BUFFER_LEN, MSG_NOSIGNAL); }
    while ( n < 0 && errno==EINTR );

    if ( n < 0 ) {
        lua_errno(L);
    } else {
        lua_pushlstring(L, read_buff, n);
        lua_pushnumber(L, n);
        return 2;
    }
}

static int lua_net_ip4_tcp_socket_send( lua_State *L ) {
    lua_ud_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_IP4_TCP_SOCKET);

    if ( sock->fd < 0 ) {
        lua_fail(L, "socket is dead", 0);
    }

    size_t msg_len;
    const char *msg = luaL_checklstring(L, 2, &msg_len);
    ssize_t n;

    do { n = send(sock->fd, msg, msg_len, MSG_NOSIGNAL); }
    while ( n < 0 && errno==EINTR );

    if ( n < 0 ) {
        lua_errno(L);
    } else {
        lua_pushnumber(L, n);
        return 1;
    }
}

static int lua_net_ip4_tcp_socket_shutdown( lua_State *L ) {
    lua_ud_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_IP4_TCP_SOCKET);
    int how = luaL_checkinteger(L, 2);

    if ( sock->fd < 0 ) {
        lua_fail(L, "socket is dead", 0);
    }

    int r = shutdown(sock->fd, how);

    if ( r == -1 ) {
        lua_errno(L);
    } else {
        lua_settop(L, 1);
        return 1;
    }
}

static int lua_net_ip4_tcp_socket_set( lua_State *L ) {
    lua_ud_socket *sock = luaL_checkudata(L, 1, LUA_MT_NET_IP4_TCP_SOCKET);

    if ( sock->fd < 0 ) {
        lua_fail(L, "socket is dead", 0);
    }

    int optname = luaL_checkinteger(L, 2);

    int fd = sock->fd;
    int r, optval, flags;
    double optdval;
    struct linger optlinger;
    struct timeval opttv;

    switch ( optname ) {
        case O_NONBLOCK:
            optval = luaL_checkinteger(L, 3);
            flags = fcntl(fd, F_GETFL);
            r = fcntl(fd, F_SETFL, optval ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK));
            break;
        case SO_RCVTIMEO:
        case SO_SNDTIMEO:
            optdval = luaL_optnumber(L, 3, 0);
            opttv.tv_sec = (int)optdval;
            opttv.tv_usec = (int)((optdval-opttv.tv_sec)*1e6);
            r = setsockopt(fd, SOL_SOCKET, optname, &opttv, sizeof(opttv));
            break;
        case SO_LINGER:
            optlinger.l_onoff = luaL_checkinteger(L, 3);
            optlinger.l_linger = luaL_checkinteger(L, 4);
            r = setsockopt(fd, SOL_SOCKET, optname, &optlinger, sizeof(optlinger));
            break;
        default:
            optval = luaL_checkinteger(L, 3);
            r = setsockopt(fd, SOL_SOCKET, optname, &optval, sizeof(optval));
            break;
    }

    if ( r == -1 ) {
        lua_errno(L);
    } else {
        lua_settop(L, 1);
        return 1;
    }
}
