//

const send = require("./lib/send")

send({
    url: "/user/watchadviewed/",
    body: {
        spin: 1,
    },
}).then(console.log)
