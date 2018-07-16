--

local net = require "net"

--

local rep = "\1\7\0\1\0\23\1\0Primary script unknown\x0A\0\1\6\0\1\0Q\7\0Status: 404 Not Found\13\x0AContent-type: text/html; charset=UTF-8\13\x0A\13\x0AFile not found.\x0A\0\0\0\0\0\0\0\1\3\0\1\0\8\0\0\0\0\0\0\0\0\0\0"

local rep_esc = (string.format("%q", rep):gsub("\n", "x0A"))

local s = assert(net.ip4.tcp.socket())

assert(s:set(net.f.SO_REUSEADDR, 1))
assert(s:bind("0.0.0.0", 12345))
assert(s:listen(100))

local client, msg

while true do
    print("w8 for clients ... ")

    client = assert(s:accept())

    --print("client caught, fd:", client:fd())

    while true do
        msg = assert(client:recv())

        print("<<", (string.format("%q", msg):gsub("\n", "x0A")))

        if #msg==0 then
            break
        else
            assert(client:send(rep))

            print(">>", rep_esc)
        end
    end

    client:close()
end
