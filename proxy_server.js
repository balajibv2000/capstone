const express = require('express')
const coap = require('coap') // or coap
let cors = require("cors")

const app = express()
app.use(cors())

const port = 5002

const pid = [1,2,3,4,5]


app.get('/critical', async (request, response) => {

	const critical_data = []
	count = 0;

	for (x in pid){
		const id = pid[x]
		const payload = {pid: id , data: ''}

		const node = coap.request(`coap://[fd00::20${id}:${id}:${id}:${id}]/node/critical`)
		node.on('response', (res) => {
			res.on('data' , (chunck) => {
					payload.data += chunck.toString()			
				})
				
			res.on('end' , () => {
				count++
				critical_data.push(payload)
				if(count === pid.length) response.send(critical_data)
			})
		})
		node.end()
	}

})

app.listen(port, () => {
  console.log(`Proxy server listening on port ${port}`)
})
