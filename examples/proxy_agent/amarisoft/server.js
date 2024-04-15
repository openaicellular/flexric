const { performance } = require('perf_hooks');
const fs = require('fs');
const WebSocket = require('ws');
const server = new WebSocket.Server({
  port: 9001
});

let sockets = [];

const json_msg_conf = fs.readFileSync('config_get.json', 'utf8');

var obj_json_conf = JSON.parse(json_msg_conf);

const json_msg_stats = fs.readFileSync('msg_stats.json', 'utf8');
var obj_json_stats = JSON.parse(json_msg_stats);

const json_msg_ue = fs.readFileSync('msg_ue_get.json', 'utf8');
var obj_json_ue = JSON.parse(json_msg_ue);

server.on('connection', function(socket) {
  sockets.push(socket);
  socket.send('{ "message": "ready", "type": "ENB", "name": "com_name" }');
  console.log('Connection received');
  // When you receive a message, send that message to every socket.
  socket.on('message', function(msg) {
    const t0 = performance.now()
    const obj = JSON.parse(msg);
    if(obj["message"] == "config_get"){
      obj_json_conf["message_id"] = obj["message_id"]
     // console.log('Sending msg: %s', json_msg);
      sockets.forEach(s => s.send(JSON.stringify(obj_json_conf)));

      console.log(`Send config_get`)
    } else if (obj["message"] == "stats"){
      obj_json_stats["message_id"] = obj["message_id"]
     // console.log('Sending msg: %s', json_msg);
      sockets.forEach(s => s.send(JSON.stringify(obj_json_stats)));
      
      console.log(`Send stats`)
    } else if (obj["message"] == "ue_get"){
      obj_json_ue["message_id"] = obj["message_id"]
    //  console.log('Sending msg: %s', json_msg);
      sockets.forEach(s => s.send(JSON.stringify(obj_json_ue)));

      console.log(`Send ue_get`)
    }
    
    const t1 = performance.now()
    console.log(`Call to doSomething took ${t1 - t0} milliseconds`)
    //console.log('Message received: %s', msg);
  });

  socket.on('ping', function(msg) {
    console.log('Ping received: %s', msg);
    // pong is automatically sent (I think...)
  });


  // When a socket closes, or disconnects, remove it from the array.
  socket.on('close', function() {

    console.log('Closing connection');
    sockets = sockets.filter(s => s !== socket);
  });
});

