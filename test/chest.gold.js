//

const send = require("./lib/send")

send({
    url: "/chestshop/buy/",
    body: {
        chestID: 3,
        method: "free",
    },
}).then(console.log)
