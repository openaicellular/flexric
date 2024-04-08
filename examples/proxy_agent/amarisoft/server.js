const fs = require('fs');
const WebSocket = require('ws');
const server = new WebSocket.Server({
  port: 9001
});

let sockets = [];
server.on('connection', function(socket) {
  sockets.push(socket);
  socket.send('{ "message": ready, "type": ENB, "name": com_name }');
  console.log('Connection received');
  // When you receive a message, send that message to every socket.
  socket.on('message', function(msg) {
    const obj = JSON.parse(msg);
    if(obj["message"] == "config_get"){
      const json_msg = fs.readFileSync('config_get.json', 'utf8');
      console.log('Sending msg: %s', json_msg);
      sockets.forEach(s => s.send(json_msg));
    }
    
    console.log('Message received: %s', msg);
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

