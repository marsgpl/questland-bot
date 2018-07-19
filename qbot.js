//

const Bot = require("./src/Bot")

const BOT_NAME = process.argv[2]

const qbot = new Bot

qbot.auth(BOT_NAME)

qbot.action(process.argv)
    .then(console.log)
    .catch(console.error)
