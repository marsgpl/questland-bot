--

local thread = require "thread"
local zmq = require "zmq"

--

print(thread.id(), zmq.context():debug())
