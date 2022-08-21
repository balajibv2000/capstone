const coap = require('coap') // or coap
//const req = coap.request('coap://[fd00::202:2:2:2]/node/critical')

const req = coap.request('coap://[::1]");

req.on('response', (res) => {
    res.pipe(process.stdout)
})

req.end()

