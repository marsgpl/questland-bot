--

local thread = require "thread"
local trace = require "trace"
local zmq = require "zmq"

--

print("worker #"..thread.id().." started")

local ctx = assert(zmq.context())

local income = assert(ctx:socket(zmq.f.ZMQ_PULL))
assert(income:connect("inproc://tasks"))

local outcome = assert(ctx:socket(zmq.f.ZMQ_PUSH))
assert(outcome:connect("inproc://tasksback"))

--

while true do
    local msg = assert(income:recv())

    if msg=="kill" then
        outcome:send(msg.."!")
        break
    else
        outcome:send(msg:reverse())
    end
end

print("worker #"..thread.id().." ended")
