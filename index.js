const coap = require('coap') // or coap
const server = coap.createServer({ type: 'udp6' })

server.on('request', (req, res) => {
    res.end('Hello this is server' + '\n')
})

server.listen(() => {
    console.log('server started')
})
