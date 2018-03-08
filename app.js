var express = require("express");
var app = new express();
var http = require("http").Server(app);
var io = require("socket.io")(http);

var net = require('net');
 
var Log = require('log'),
    log = new Log('debug')
 
var port = process.env.PORT || 3000;

app.use(express.static(__dirname + "/public" ));
 
app.get('/', function(req,res){
    res.redirect('index.html');
});
 
var client = new net.Socket();

io.on('connection',function(socket){ 
    var connected = false;
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
            connected = true;
            client.connect(1337, args.ip, function() {
                console.log('Connected'); 
                client.write(args.body);
                client.destroy();
            });

            // client.on('data', function(data) {
            //    console.log('Received: ' + data);
            //   client.destroy(); // kill client after server's response
            //});

            client.on('close', function() {
                console.log('Connection closed');
            });
        //TODO: Flag for connected status
        //If still connected, send another packet (TEST)
        //If not connected, reconnect and send (test?)
        //If different IP, keep first open and open another client connection
    })
});

io.on('err', function(err){
    // handle the error safely
    console.log(err)
})

http.listen(port,function(){
    log.info('Server listening through port %s', port);
});