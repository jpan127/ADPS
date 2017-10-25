var express = require("express");
var app = new express();
var http = require("http").Server(app);
var io = require("socket.io")(http);
 
var Log = require('log'),
    log = new Log('debug')
 
var port = process.env.PORT || 3000;

app.use(express.static(__dirname + "/public" ));
 
app.get('/', function(req,res){
    res.redirect('index.html');
});
 
io.on('connection',function(socket){ 
	socket.on('stream',function(image){
		socket.broadcast.emit('stream', image);
	});
	socket.on('event',function(data){
		console.log('A client sent us this SMART message:', data.message);
	});
    socket.on('exit', function(ignore) {
        socket.disconnect(true);
        console.log("Server disconnecting.");
        process.exit();
    });
    socket.on('packet', function(args) {
        console.log("Sending to", args.ip, ":", args.body);
        // Needs support here to send to IP address
        // Maybe the ESP32 has to be the server and the computer the client?
    })
});

http.listen(port,function(){
    log.info('Server listening through port %s', port);
});