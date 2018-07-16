--

local trace = require "trace"
local net = require "net"

--

local sock = assert(net.ip6.tcp.socket())
assert(sock:connect("::1", 11223))

sock:send("Hey!")
sock:send("bye =(")
sock:close()
