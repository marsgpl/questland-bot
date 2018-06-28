//

const send = require("./lib/send")

send({
    url: "/user/gettavernenergy/",
    body: {
    },
}).then(console.log)
