//

const send = require("./lib/send")

send({
    url: "/chestshop/buy/",
    body: {
        chestID: 18,
        method: "free",
    },
}).then(console.log)
