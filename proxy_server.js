const express = require('express')
const coap = require('coap') // or coap
let cors = require("cors")

const app = express()
app.use(cors())

const port = 5000

app.get('/', (req, response) => {

	const node = coap.request('coap://[fd00::202:2:2:2]/node/critical')

	var data = ""

	node.on('response', (res) => {
		  res.on('data' , (chunck) => {
				data += chunck.toString()			
			})
			
			res.on('end' , () => {
				console.log("Data: " + data)
				response.send(data)
			})
	})

	node.end()	
})

app.listen(port, () => {
  console.log(`Proxy server listening on port ${port}`)
})
