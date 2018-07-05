//

const send = require("./lib/send")

;(async function() {
    console.log(await send({
        url: "/user/setdeviceinfo/",
        body: {
            phone_model: "\x00",
            os_version: "\x00",
            platform: "ios",
            graphics_quality: "\x00",
            country_code: "\x00",
            language_code: "\x00",
            is_legal_apk: 1,
            gdpr: 1,
        },
    }))
})()
