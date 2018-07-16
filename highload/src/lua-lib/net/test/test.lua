--

local net = require "net"
local trace = require "trace"

--

local domain = "fuck.world"

trace(net.ip4.tcp.nslookup(domain))
trace(net.ip6.tcp.nslookup(domain))
