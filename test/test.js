//

const send = require("./lib/send")

send({
    url: "/client/init/",
    body: {
        utc_offset: 10800,
    },
}).then(console.log)
