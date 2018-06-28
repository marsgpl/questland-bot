//

const send = require("./lib/send")

send({
    url: "/user/watchadviewed/",
    body: {
        shorten_journey_id: 1,
    },
}).then(console.log)
