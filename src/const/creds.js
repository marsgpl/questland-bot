//

const VERSION = "1.10.3.8"

const REGION = "eu2"
const HOST = "gs-eu2-wrk-05.api-ql.com" // 01 - 05

const LANG = "en"
const PLATFORM = "hd_android"

module.exports = {
    method: "POST",
    host: HOST,
    headers: {
        "Content-Type": "application/x-www-form-urlencoded",
        "questlanddc": REGION,
        "Accept": "application/json",
        "User-Agent": "",
        "Accept-Encoding": "identity",
    },
    body: {
        lang: LANG,
        version: VERSION,
        client_platform: PLATFORM,
    },
}
