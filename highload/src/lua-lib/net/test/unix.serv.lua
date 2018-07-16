--

local trace = require "trace"
local net = require "net"

--

local sock = assert(net.unix.socket())
assert(sock:bind("/tmp/lolwtf.sock", 640))
assert(sock:listen(128))

while true do
    print "w8 for client ... "
    local client = assert(sock:accept())
    print("client caught:", client:fd(), client:id())

    while true do
        local str = assert(client:recv())
        trace(str)

        if #str==0 then
            print("client disconnected:", client:fd(), client:id())
            break
        end
    end
end
