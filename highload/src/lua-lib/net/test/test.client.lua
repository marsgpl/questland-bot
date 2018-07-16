--

local net = require "net"

--

local s = assert(net.ip4.tcp.socket(0))

local host = "google.com"
local ips = net.ip4.tcp.nslookup(host)
local ip = ips[1]

assert(s:connect(ip, 80))

assert(s:send("GET / HTTP/1.0\r\nHost: "..host.."\r\n\r\n"))

while true do
    local msg = assert(s:recv())

    if #msg==0 then
        break
    else
        io.write(msg):flush()
    end
end
