//

const md5 = require("md5")

const BaseBot = require("./BaseBot")

const BOTS = require("./const/bots")
const GUILDS = require("./const/guilds")
const SEX = require("./const/sex")
const CHESTS = require("./const/chests")

const CHEST_OPEN_METHODS = ["free","key"]

module.exports = class extends BaseBot {
    // node qbot.js CANADA ping REFORGE "hey"
    async cmd_ping(params) {
        let [ to, msg ] = params

        let toBot = BOTS[to.toUpperCase()]

        if ( !toBot ) throw {
            reason: `bot "${to}" not found`,
        }

        console.log(`ping: to: ${to}, msg: ${JSON.stringify(msg)}`)

        return this.query({
            url: "/chat/sendmessage/",
            body: {
                kind: "private",
                msg,
                target_id: toBot.id,
            },
        })
    }

    // node qbot.js "" reg Brazil
    async cmd_reg(params) {
        this.initBarber()

        let [ name ] = params

        let json

        this.name = name.toUpperCase() // не палимся

        this.bot = {
            new: true,
            device: md5(`device:${this.name}`),
        }

        this.extern(this.bot, BOTS[this.name] || {})

        if ( !this.bot.token ) {
            json = await this.query({
                url: "/user/connectdevice/",
                body: {
                    deviceID: this.bot.device,
                    chat_last_uts: 0,
                },
            })

            this.bot.token = json.data.token

            console.log("NEW TOKEN", this.bot.token)
        }

        console.log("NAME", this.name)
        console.log("DEVICE", this.bot.device)
        console.log("TOKEN", this.bot.token)

        json = await this.query({
            url: "/user/setdeviceinfo/",
            body: {
                phone_model: "Nokia 3310",
                os_version: "Android OS 7.0/API-24",
                platform: "android",
                graphics_quality: "hd",
                country_code: "GB",
                language_code: "en",
                is_legal_apk: 1,
                gdpr: 1,
                chat_last_uts: 0,
            },
        })

        json = await this.query({
            url: "/client/checkstaticdata/",
            body: {
                graphics_quality: "hd_android",
                chat_last_uts: 0,
            },
        })

        json = await this.query({
            url: "/user/checkname/",
            body: {
                name: this.name,
                chat_last_uts: 0,
            },
        })

        if ( json.data.name_error ) throw {
            reason: json.data.name_error,
        }

        let sex = SEX[this.rand(0, SEX.length-1)]

        let body = {
            name: this.name,
            sex,
            chat_last_uts: 0,
            skin_color: this.rand(this.barber.skin.min, this.barber.skin.max),
            hair_color: this.rand(this.barber.hair.min, this.barber.hair.max),
        }

        let types = this.barber.body[sex]

        Object.keys(types).forEach(type => {
            body["body_" + type] = types[type][this.rand(0, types[type].length - 1)]
        })

        json = await this.query({
            url: "/user/createuser/",
            body,
        })

        this.bot.id = json.data.hero_id

        console.log("ID", this.bot.id)

        json = await this.query({
            url: "/client/init/",
            body: {
                utc_offset: 25200, // +7
                chat_last_uts: 0,
            },
        })

        console.log(`
    "${this.name}": {
        id: ${this.bot.id},
        device: "${this.bot.device}",
        token: "${this.bot.token}",
    },
        `)

        return this.bot
    }

    // node qbot.js REFORGE random_face
    async cmd_random_face(params) {
        this.initBarber()

        await this.resetReqId()

        let json = await this.query({
            url: "/user/getprofile/",
            body: {
                hero_id: this.bot.id,
                req_id: 10,
            },
        })

        let sex = json.data.profile.sex

        let url = "/barbershop/buymulti/"

        let keepGender = !!this.rand(0,1)

        if ( !keepGender ) {
            sex = (sex===SEX[0]) ? SEX[1] : SEX[0]
            url = "/user/changesex/"
        }

        let skinColor = this.rand(this.barber.skin.min, this.barber.skin.max)
        let hairColor = this.rand(this.barber.hair.min, this.barber.hair.max)

        let productIDs = []

        let order = ["hair","eyes","eyebrows","nose","facespecial","facialhair"]
        let types = this.barber.body[sex]

        order.forEach(type => {
            if ( !types[type] ) { return }
            productIDs.push(types[type][this.rand(0, types[type].length - 1)])
        })

        return this.query({
            url,
            body: {
                productIDs,
                skinColor,
                hairColor,
                req_id: 11,
            },
        })
    }

    // node qbot.js CANADA join_guild UN
    async cmd_join_guild(params) {
        let [ to ] = params

        let toGuild = GUILDS[to.toUpperCase()]

        if ( !toGuild ) throw {
            reason: `guild "${to}" not found`,
        }

        console.log(`join_guild: ${to} ${toGuild}`)

        return this.query({
            url: "/guild/sendjoinrequest/",
            body: {
                guild_id: toGuild,
            },
        })
    }

    // node qbot.js CANADA accept_join_guild VENEZUELA
    async cmd_accept_join_guild(params) {
        let [ botName ] = params

        let bot = BOTS[botName.toUpperCase()]

        if ( !bot ) throw {
            reason: `bot "${botName}" not found`,
        }

        console.log(`gm_accept_join: ${botName} ${bot.id}`)

        return this.query({
            url: "/guild/handlejoinrequest/",
            body: {
                hero_id: bot.id,
                accept: 1,
            },
        })
    }

    // node qbot.js CANADA edit_guild UN
    async cmd_edit_guild(params) {
        let [ guildName ] = params

        let guildId = GUILDS[guildName.toUpperCase()]

        if ( !guildId ) throw {
            reason: `guild "${guildName}" not found`,
        }

        console.log(`edit_guild: ${guildName} ${guildId}`)

        return this.query({
            url: "/guild/editguild/",
            body: {
                name: "United Nations",
                description: "International Questland Cooperation",
                country: "world",
                is_private: 1,
                req_level: 0,
                req_heropower: 0,
                req_vip: 0,
                banner_tpls: [ // reforge banner
                    3737,
                    3637,
                    3589,
                    3387,
                    3287,
                    3521,
                ],
                guild_id: guildId,
            },
        })
    }

    // node qbot.js LEMIX ad_gems
    async cmd_ad_gems(params) {
        return this.query({
            url: "/user/watchadviewed/",
        })
    }

    // node qbot.js LEMIX ad_bank
    async cmd_ad_bank(params) {
        return this.query({
            url: "/user/watchadviewed/",
            body: {
                bank_extra_gold: 1,
            },
        })
    }

    // node qbot.js LEMIX ad_energy
    async cmd_ad_energy(params) {
        return this.query({
            url: "/user/watchadviewed/",
            body: {
                energy: 1,
            },
        })
    }

    // node qbot.js LEMIX ad_journey
    async cmd_ad_journey(params) {
        return this.query({
            url: "/user/watchadviewed/",
            body: {
                shorten_journey_id: 1,
            },
        })
    }

    // node qbot.js LEMIX ad_jungle
    async cmd_ad_jungle(params) {
        return this.query({
            url: "/user/watchadviewed/",
            body: {
                spin: 1,
            },
        })
    }

    // node qbot.js LEMIX tavern_energy
    async cmd_tavern_energy(params) {
        return this.query({
            url: "/user/gettavernenergy/",
        })
    }

    // node qbot.js LEMIX chest eternal free
    // node qbot.js LEMIX chest gold free
    // node qbot.js LEMIX chest silver free
    async cmd_chest(params) {
        let [ chestID, method ] = params

        if ( !CHESTS[chestID] ) throw {
            reason: `chest id not found: "${chestID}"`,
        }

        if ( CHEST_OPEN_METHODS.indexOf(method) < 0 ) throw {
            reason: `chest open method "${method}" not found`,
        }

        return this.query({
            url: "/chestshop/buy/",
            body: {
                chestID: CHESTS[chestID],
                method,
            },
        })
    }
}

/*
    /guild/giveowner/
    hero_id=1902401
*/
