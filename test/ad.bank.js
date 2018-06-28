//

const send = require("./lib/send")

send({
    url: "/user/watchadviewed/",
    body: {
        bank_extra_gold: 1,
    },
}).then(console.log)
