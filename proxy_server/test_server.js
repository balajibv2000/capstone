const express = require('express')
const coap = require('coap') // or coap
let cors = require("cors")
const { response } = require('express')

const app = express()
app.use(cors())

const port = 8080
const COAP_SERVER_IP = 'fd00::216:16:16:16'

const QUEUE_SIZE = 10

let critical_node = {
	motes: [],
	cache: {}
}

let mid_critical_node = {
	motes : [],
	cache : {}
}

let periodic_node ={
	motes : [],
	cache: {}
}

app.get('/critical/:id', function(req, res) {

	res.setHeader('Content-Type', 'text/event-stream')

	console.log("id: ", req.params.id)
	getCache(res , critical_node , req.params.id)

	const interval = setInterval(() => {	
		getCache(res , critical_node , req.params.id)
	}, 15000)

	res.on('close',() => {
		console.log('Client closed connection')
		clearInterval(interval)
		res.end()
	})
   
});




app.listen(port, () => {
  console.log(`Proxy server listening on port ${port}`)
})


// function definitions

const getData = async (response , type , route) => {
	const critical_data = []
	var payload = ''
	
	const node = coap.request(`coap://[${COAP_SERVER_IP}]/node/${route}`)

	node.on('response', (res) => {
		console.log(`Fetching ${route} data from coap server...`)
		res.on('data' , (chunck) => {
				payload += chunck.toString()	
			})
			
		res.on('end' , () => {
			const arr = payload.split(';')
			arr.pop()
			for( x in arr){
				const data = arr[x].split(' ')
				const id = data[0]
				let obj = {}
				if(route == 'periodic'){
					obj = {lid: parseInt(data[0]) , temp: parseInt(data[1]) , humidity: parseInt(data[2]) , uv: parseInt(data[3]) , pressure: parseInt(data[4])}
				}
				else{
					obj = {pid: parseInt(data[0]) , oxy: parseInt(data[1]) , temp: parseInt(data[2]) , pulse: parseInt(data[3]) , bp: parseInt(data[4])}
				}
				
				critical_data.push(obj)
				if(type.motes.indexOf(id) == -1){
					type.motes.push(id)
					type.cache[id] = []
					type.cache[id].push(obj)
				}
				else{
					if(type.cache[id].length < QUEUE_SIZE){
						type.cache[id].push(obj)
					}
					else {
						type.cache[id].shift()
						type.cache[id].push(obj)
					}
					//console.log(route ," " , type.cache[id].length )
				}
			}
			console.log("Replying back with data...")
			const str = JSON.stringify(critical_data)
			response.write(`data:${str}\n\n`);
		})
	})
	node.end()
}

const getCache = (response , type , id) => {
	//const str = JSON.stringify(type.cache[id])
	//console.log(str)
	const str = 'This is data'
	response.write(`data:${str}\n\n`);
}
