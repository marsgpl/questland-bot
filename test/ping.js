//

const send = require("./lib/send")
const BOTS = require("./lib/bots")
const AUTH = require("./lib/authBase")

const FROM = "CANADA"
const TO = "REFORGE"

const MSG = "nice name"

const AUTH = {
    host: "gs-eu2-wrk-05.api-ql.com",
    questlanddc: "eu2",
    body: {
        lang: "en",
        version: "1.10.3.8",
        client_platform: "hd_android",
    },
}

AUTH.body.device = BOTS[CURRENT_BOT].device
AUTH.body.token = BOTS[CURRENT_BOT].token

send({
    url: "/chat/sendmessage/",
    body: {
        kind: "private",
        msg: "Hey!",
        target_id: TARGET_USER_ID,
    },
}, AUTH).then(console.log)
