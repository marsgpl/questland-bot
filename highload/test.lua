--

package.path = package.path .. ";./lib/?.lua"
package.cpath = package.cpath .. ";./lib/?.so"

local class = require "class"
local net = require "net"
local std = require "std"
local thread = require "thread"
local trace = require "trace"
local zmq = require "zmq"

math.randomseed(os.time())

local req_id = 100

local function randstr(len)
	local res = ""
	for i=1,len do
		res = res .. string.char(math.random(97, 122))
	end
	return res
end

local HOSTS = {
    "gs-eu2-wrk-01.api-ql.com",
    "gs-eu2-wrk-02.api-ql.com",
    "gs-eu2-wrk-03.api-ql.com",
    "gs-eu2-wrk-04.api-ql.com",
}

local CONNECTIONS_PER_HOST = 100

local IPS = {}

for i=1,#HOSTS do
    IPS[i] = net.ip4.tcp.nslookup(HOSTS[i])[1]
end

local ep = assert(net.epoll())
local conns = {}
local clients = {}

for i=1,#HOSTS do
    for i2=1,CONNECTIONS_PER_HOST do
        local s = assert(net.ip4.tcp.socket(1))
        local fd = s:fd()

        conns[#conns+1] = s
        clients[fd] = {
            s = s,
            id = i .. ":" .. i2,
            host = HOSTS[i],
            sent = false,
            rcvd = false,
        }

        s:connect(IPS[i], 80)

        assert(ep:watch(fd, net.f.EPOLLET | net.f.EPOLLIN | net.f.EPOLLOUT | net.f.EPOLLRDHUP))
    end
end

local onread = function(fd)
    local client = clients[fd]
    local s = client.s

    if client.rcvd then
        return
    end

    client.response = assert(s:recv())

    print(client.id, client.response)

    client.rcvd = true
end

local onwrite = function(fd)
    local client = clients[fd]
    local s = client.s

    if client.sent then
        return
    end

    req_id = req_id + 1

    local body = "req_id="..req_id.."&lang=en&device=1f73f46e2f4a618dd787dae225a01053&token=b305f99a4baa9330cc50cc82cb7ba402&version=1.10.2.13&client_platform=sd_android&chat_last_uts=1531417271.562783&time_spent_in_game=10"

    local payload = {
        "POST /user/watchadviewed/?rand=" .. randstr(8) .. " HTTP/1.0",
        "Content-Type: application/x-www-form-urlencoded",
        "Accept: application/json",
        "questlanddc: eu2",
        "Content-Length: " .. #body,
        "Connection: keep-alive",
        "Host: " .. client.host,
        "",
        body,
    }

    payload = table.concat(payload, "\r\n")

    assert(s:send(payload))

    client.sent = true
end

local ontimeout = function()
    trace("ontimeout")
end

local onerror = function(fd, es, en)
    local client = clients[fd]

    trace("onerror", client.id, es, en)
end

local onhup = function(fd)
    local client = clients[fd]

    trace("onhup", client.id)
end

assert(ep:start(5000, onread, onwrite, ontimeout, onerror, onhup))
