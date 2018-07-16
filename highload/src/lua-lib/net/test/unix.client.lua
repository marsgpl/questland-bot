--

local trace = require "trace"
local net = require "net"

--

local sock = assert(net.unix.socket())
assert(sock:connect("/tmp/lolwtf.sock"))

sock:send("Hey!")
sock:send("bye =(")
sock:close()
