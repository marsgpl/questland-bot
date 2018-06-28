//

const send = require("./lib/send")

send({
    url: "/user/watchadviewed/",
    body: {
        energy: 1,
    },
}).then(console.log)
