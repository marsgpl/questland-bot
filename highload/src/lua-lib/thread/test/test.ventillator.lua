--

local thread = require "thread"
local trace = require "trace"
local zmq = require "zmq"

--

local ctx = assert(zmq.context())

local sock = assert(ctx:socket(zmq.f.ZMQ_PUSH))
assert(sock:bind("inproc://tasks"))

--

local t, tid
local workers = {}
local workers_n = 2

for i=1,workers_n do
    t, tid = assert(thread.start("test.worker.lua", {}, { zmq.__get_ctx_mf() }))
    workers[tid] = t
end

t, tid = assert(thread.start("test.sink.lua", {}, { zmq.__get_ctx_mf() }))
workers[tid] = t

local n = 0
local ts = zmq.microtime()

while true do
    n = n + 1

    sock:send("task #"..n)

    if n >= 10000 then
        break
    end
end

for tid,t in pairs(workers) do
    sock:send("kill")
end

print("ventillator end")
