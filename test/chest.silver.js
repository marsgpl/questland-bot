//

const send = require("./lib/send")

send({
    url: "/chestshop/buy/",
    body: {
        chestID: 4,
        method: "free",
    },
}).then(console.log)
