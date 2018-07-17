//

const { URLSearchParams } = require("url")
//~ const querystring = require("querystring")

const fetch = require("node-fetch")

const auth = require("./auth")

const RANDOM_STRING_POSSIBLE_CHARS = "abcdefghijklmnopqrstuvwxyz"

let rand = function(min, max) {
    return min
        ? Math.floor(Math.random() * (max - min + 1)) + min
        : Math.floor((Math.random() * max) + 0)
}

let randomString = function(len) {
    let str = []

    for ( let i=0; i<len; ++i ) {
        str.push(RANDOM_STRING_POSSIBLE_CHARS[rand(0, RANDOM_STRING_POSSIBLE_CHARS.length - 1)])
    }

    return str.join("")
}

let extern = function(base, data) {
    for ( let k in data ) {
        if ( typeof data[k] === "object" ) {
            return extern(base[k], data[k])
        } else {
            base[k] = data[k]
        }
    }
}

module.exports = async function(conf, customAuth) {
    extern(conf, customAuth || auth)

    conf.body.chat_last_uts = String(Date.now() / 1000) + String(rand(100,999))
    conf.body.req_id = conf.body.req_id || Math.floor(Date.now() / 1000)
    conf.body.time_spent_in_game = rand(1,100)

    let url = "http://" + conf.host + conf.url + "?rand=" + randomString(8)

    const params = new URLSearchParams()

    for ( let k in conf.body ) {
        params.append(k, conf.body[k])
    }

    return await fetch(url, {
        method: "POST",
        headers: {
            "Content-Type": "application/x-www-form-urlencoded",
            "questlanddc": conf.questlanddc,
            "Accept": "application/json",
            "User-Agent": "",
            "Accept-Encoding": "identity",
        },
        body: params,
        //~ body: querystring.stringify(conf.body),
    }).then(res => res.json())
}
