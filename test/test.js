//

const send = require("./lib/send")

send({
    url: "/boss/battleeventfightend/",
    body: {
        battle_moves: "0-1.0-1.0-1.0-1.0-1.0-1.0-1.",
    },
}).then(console.log)
