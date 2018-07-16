--

local net = require "net"
local trace = require "trace"

--

local ep = assert(net.epoll())

trace(getmetatable(ep))
