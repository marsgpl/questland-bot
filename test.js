//

const Bot = require("./src/Bot")
const BOTS = require("./src/const/bots")

let names = Object.keys(BOTS)

let n = 1000

names.forEach(name => {
    if ( name === "LEMIX" ) { return }
    if ( name === "REFORGE" ) { return }

    let qbot = new Bot

    qbot.auth(name)

    setTimeout(() => {
        qbot.action([0,0,0,"join_guild", "REFORGY"])
            .then(r => console.log(r.status))
            .catch(console.error)
    }, n+=1000)
})
