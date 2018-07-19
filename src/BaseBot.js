//

//~ const { URLSearchParams } = require("url")
const fetch = require("node-fetch")

const BOTS = require("./const/bots")
const CREDS = require("./const/creds")
const BARBER_MALE = require("./const/barber.male")
const BARBER_FEMALE = require("./const/barber.female")

const RANDOM_STRING_POSSIBLE_CHARS = "abcdefghijklmnopqrstuvwxyz"

module.exports = class {
    constructor() {
        this.initBarber()
    }

    auth(botName) {
        this.name = botName

        if ( !this.name ) {
            console.log(`auth: anonymous`)

            return this
        }

        this.bot = BOTS[this.name.toUpperCase()]

        if ( !this.bot ) throw {
            reason: `auth: bot "${this.name}" not found`,
        }

        console.log(`auth: ${this.name} ${this.bot.id}`)

        return this
    }

    async action(args) {
        let cmd = args.slice(3,4)[0]
        let params = args.slice(4)

        let method = this["cmd_" + cmd]

        if ( typeof method !== "function" ) throw {
            reason: `cmd "${cmd}" not found`,
        }

        return method.call(this, params)
    }

    rand(min, max) {
        return Math.floor(Math.random() * (max - min + 1)) + min
    }

    randomStringForQuery(len = 8) {
        let str = []

        for ( let i=0; i<len; ++i ) {
            str.push(RANDOM_STRING_POSSIBLE_CHARS[
                this.rand(0, RANDOM_STRING_POSSIBLE_CHARS.length - 1)])
        }

        return str.join("")
    }

    extern(base, data) {
        for ( let k in data ) {
            if ( Array.isArray(data[k]) ) {
                base[k] = base[k] || []
                this.extern(base[k], data[k])
            } else if ( typeof data[k] === "object" ) {
                base[k] = base[k] || {}
                this.extern(base[k], data[k])
            } else {
                base[k] = data[k]
            }
        }
    }

    getQueryString(params) {
        let qs = []

        Object.keys(params).forEach(k => {
            if ( Array.isArray(params[k]) ) {
                qs.unshift(params[k].map(val => `${encodeURIComponent(k)}%5b%5d=${encodeURIComponent(val)}`).join('&'))
            } else {
                qs.push(`${encodeURIComponent(k)}=${encodeURIComponent(params[k])}`)
            }
        })

        return qs.join("&")
    }

    async query(params) {
        let conf = {}

        this.extern(conf, CREDS)

        conf.body.chat_last_uts = String(Date.now() / 1000) + String(this.rand(100,999))
        conf.body.req_id = String(Date.now()).substr(3).replace(/^0+/, "")
        conf.body.time_spent_in_game = this.rand(1,20)

        if ( this.bot ) {
            conf.body.device = this.bot.device
            conf.body.token = this.bot.token

            if ( this.bot.player_id ) {
                conf.body.player_id = this.bot.player_id
            }

            if ( !this.bot.new && (!conf.body.device || !conf.body.token) ) throw {
                reason: `query: bad bot conf`,
            }
        }

        this.extern(conf, params)

        conf.url = "http://" + conf.host + conf.url + "?rand=" + this.randomStringForQuery()

        //~ const usp = new URLSearchParams()

        //~ Object.keys(conf.body).forEach(k => {
            //~ usp.append(k, conf.body[k])
        //~ })

        return fetch(conf.url, {
            method: conf.method,
            headers: conf.headers,
            //~ body: usp,
            body: this.getQueryString(conf.body),
        }).then(res => res.json())
    }

    async wait(ms = 1) {
        return new Promise((resolve, reject) => {
            setTimeout(resolve, ms)
        })
    }

    initBarber() {
        let src = {
            male: BARBER_MALE,
            female: BARBER_FEMALE,
        }

        this.barber = {
            skin: { min:0, max:11 },
            hair: { min:0, max:18 },
            body: {},
        }

        Object.keys(src).forEach(sex => {
            src[sex].forEach(data => {
                let [ id,_,__,name,type ] = data

                let types = this.barber.body[sex]

                if ( !types ) types = this.barber.body[sex] = {}
                if ( !types[type] ) types[type] = []

                types[type].push(id)
            })
        })
    }

    async resetReqId() {
        await this.query({
            url: "/client/init/",
            body: {
                utc_offset: 25200, // +7
                chat_last_uts: 0,
                req_id: 1,
            },
        })

        //~ await this.query({
            //~ url: "/user/setdeviceinfo/",
            //~ body: {
                //~ phone_model: "Nokia 3310",
                //~ os_version: "Android OS 7.0/API-24",
                //~ platform: "android",
                //~ graphics_quality: "hd",
                //~ country_code: "GB",
                //~ language_code: "en",
                //~ is_legal_apk: 1,
                //~ gdpr: 1,
                //~ chat_last_uts: 0,
                //~ req_id: 2,
            //~ },
        //~ })

        //~ await this.query({
            //~ url: "/client/checkstaticdata/",
            //~ body: {
                //~ graphics_quality: "hd_android",
                //~ chat_last_uts: 0,
                //~ req_id: 3,
            //~ },
        //~ })
    }
}
