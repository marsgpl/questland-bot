--

local thread = require "thread"
local zmq = require "zmq"

--

zmq.context()

local threads = {}

for i=1,10 do
    local t, tid = thread.start("test.slave.lua", { wtf=1, b="lol" }, { zmq.__get_ctx_mf() })
    threads[tid] = t
end

print("master end")
