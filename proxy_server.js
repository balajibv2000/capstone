const express = require('express')
const coap = require('coap') // or coap
let cors = require("cors")

const app = express()
app.use(cors())

const port = 5000

app.get('/critical', async (request, response) => {

	const critical_data = [] 
	var payload = ''

	const node = coap.request('coap://[fd00::201:1:1:1]/node/critical')

	node.on('response', (res) => {
		res.on('data' , (chunck) => {
				payload += chunck.toString()			
			})
			
		res.on('end' , () => {
			const arr = payload.split(';')
			arr.pop()
			for( x in arr){
				const data = arr[x].split(' ')
				const obj = {pid: parseInt(data[0]) , oxy: parseInt(data[1]) , temp: parseInt(data[2]) , pulse: parseInt(data[3]) , bp: parseInt(data[4])}
				critical_data.push(obj)
			}
			response.send(critical_data)
		})
	})
	node.end()

})

app.get('/mid-critical', async (request, response) => {

	const mid_critical_data = []
	var payload = ''

	const node = coap.request('coap://[fd00::201:1:1:1]/node/mid-critical')

	node.on('response', (res) => {
		res.on('data' , (chunck) => {
				payload += chunck.toString()			
			})
			
		res.on('end' , () => {
			const arr = payload.split(';')
			for( x in arr){
				const data = arr[x].split(' ')
				const obj = {pid: data[0] , data: data[1]}
				if(obj.data != null) mid_critical_data.push(obj)
			}
			response.send(mid_critical_data)
		})
	})
	node.end()

})

app.get('/periodic', async (request, response) => {

	const periodic_data = []
	var payload = ''

	const node = coap.request('coap://[fd00::201:1:1:1]/node/periodic')

	node.on('response', (res) => {
		res.on('data' , (chunck) => {
				payload += chunck.toString()			
			})
			
		res.on('end' , () => {
			const arr = payload.split(';')
			for( x in arr){
				const data = arr[x].split(' ')
				const obj = {pid: data[0] , data: data[1]}
				if(obj.data != null) periodic_data.push(obj)
			}
			response.send(periodic_data)
		})
	})
	node.end()

})

app.listen(port, () => {
  console.log(`Proxy server listening on port ${port}`)
})
