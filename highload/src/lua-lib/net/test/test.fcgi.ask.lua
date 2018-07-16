--

local net = require "net"
local trace = require "trace"

--

local req = "\1\1\0\1\0\8\0\0\0\1\1\0\0\0\0\0\1\4\0\1\3+\5\0\12\9QUERY_STRINGa=1&b=lol\14\3REQUEST_METHODGET\12\0CONTENT_TYPE\14\0CONTENT_LENGTH\11\7SCRIPT_NAME/lua/op\11\17REQUEST_URI/lua/op?a=1&b=lol\12\7DOCUMENT_URI/lua/op\13\9DOCUMENT_ROOT/srv/http\15\8SERVER_PROTOCOLHTTP/1.1\17\7GATEWAY_INTERFACECGI/1.1\15\11SERVER_SOFTWAREnginx/1.8.0\11\3REMOTE_ADDR::1\11\5REMOTE_PORT37430\11\3SERVER_ADDR::1\11\4SERVER_PORT8090\11\0SERVER_NAME\15\3REDIRECT_STATUS200\15\16SCRIPT_FILENAME/srv/http/lua/op\13\9DOCUMENT_ROOT/srv/http\9\14HTTP_HOSTlocalhost:8090\15\x0AHTTP_CONNECTIONkeep-alive\18\9HTTP_CACHE_CONTROLmax-age=0\11JHTTP_ACCEPTtext/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\30\1HTTP_UPGRADE_INSECURE_REQUESTS1\15hHTTP_USER_AGENTMozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.73 Safari/537.36\20\19HTTP_ACCEPT_ENCODINGgzip, deflate, sdch\20\23HTTP_ACCEPT_LANGUAGEen-US,en;q=0.8,ru;q=0.6\0\0\0\0\0\1\4\0\1\0\0\0\0\1\5\0\1\0\0\0\0"

local sock = assert(net.ip4.tcp.socket())
assert(sock:connect("127.0.0.1", 9000))

trace(sock:send(req))

while true do
    msg = assert(sock:recv())

    trace(msg)

    if #msg==0 then
        break
    end
end
