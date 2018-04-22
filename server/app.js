/***********************************************/
/************ Init & Dependencies **************/
/***********************************************/

var express = require("express");
var app = express()
var http = require("http").Server(app);
var io = require("socket.io")(http); 
var jsonfile = require('jsonfile')
var net = require('net');
var Log = require('log'),
    log = new Log('debug')
var spawn = require("child_process").spawn;
var port = process.env.PORT || 3000;
var PythonShell = require('python-shell');
app.use(express.static(__dirname + "/public" ));
require('events').EventEmitter.prototype._maxListeners = 100; // Set max listeners

// Set home page as index.html
app.get('/', function(req,res){
    res.redirect('index.html');
});



var client = new net.Socket();  
  
client.on('err', function(err){
    // handle the error safely
    console.log("Client error");
    console.log(err)
});

/***********************************************/
/**************** Server Code  *****************/
/***********************************************/

var serverPort = 5002;
var espIP = "192.168.43.250";
var espPort = 5000;

/* 'io' - socket listening on 3000 for webserver */
io.on('connection',function(socket){ 

/* 'server' - socket listening on 5002 for ESP comms */
    var server = net.createServer(function(sock) {
        
/***********************************************/
/************ Receive data from ESP & **********/
/************ send to data logging table *******/
/***********************************************/

        /* Discovers what data ESP is sending */
        sock.on('data', function(data) {
            // Packet Type
            var packetType = data.slice(0,1).toString();
            packetType = "0x" + ascii_to_hexa(packetType);
            packetType = parseInt(packetType);        
            // Payload Length
            var payloadLength = data.slice(1,2).toString();
            payloadLength = "0x" + ascii_to_hexa(payloadLength);
            payloadLength = parseInt(payloadLength);
            // Payload
            var payload = data.slice(2,payload).toString();
            var payload = payload.split(":");
            console.log(packetType , payloadLength, payload);
            /* Based on packet type, 
            send data to visualizer.html to populate logging table */               
            if(packetType == 0 )
            {
                socket.broadcast.emit('infoDiag', payload);
            }
            if(packetType == 1)
            {
                socket.broadcast.emit('errorDiag', payload);
            }
            if(packetType == 2)
            {
                socket.broadcast.emit('statusDiag', payload);
            }
            if(packetType == 3)
            {
                socket.broadcast.emit('log', payload);
            }
        });

        /* Handles ERRADDRINUSE and ERRINVAL resets server after catching */
        sock.on('err', function(err){
            console.log(err)
            server.close();
            server.listen(serverPort, '192.168.43.12');
        });
    });

    /********* Activates Server ***********/
    server.listen(serverPort, '192.168.43.12');

/***********************************************************/
/************ Receive data from data logging page **********/
/***************** sends data to ESP32 *********************/
/***********************************************************/

    /* Stream images from webcam to visualizer page */
	socket.on('stream',function(image){
		socket.broadcast.emit('stream', image);
	});

    /* Depending on type of command, send correct packet to ESP */
	socket.on('event',function(data){
        client.connect(espPort, espIP, function() { 
            console.log('Local client sending:', data.message);
            buf = new Buffer([data.message, 0xA, 0xFF]); // 0xA is temp value
            client.write(buf);
            client.destroy();
            client = new net.Socket();
        });
	});
    socket.on('dirEvent',function(data){
        client.connect(espPort, espIP, function() { 
            console.log('Local client sending:', data.message);
            buf = new Buffer([data.message, 0x4B, 0xFF]); // 0x4B is temp value 75
            client.write(buf);
            client.destroy();
            client = new net.Socket();
        });
    });    
    socket.on('dutyEvent',function(data){
        client.connect(espPort, espIP, function() { 
            console.log('Local client sending duty cycle:', data.message);
            buf = new Buffer([12, data.message, 0xFF]);  
            client.write(buf);
            client.destroy();
            client = new net.Socket();
        });
    });
    socket.on('exit', function(ignore) { 
        socket.disconnect(true);
        console.log("Server disconnecting.");
        process.exit(); 
    });
    socket.on('packet', function(args) {
        client.connect(espPort, args.ip, function() { //192.168.43.250 port 5000
            console.log('Connected'); 
            console.log("Sending packet");
            buf = new Buffer([0x41, 0x42, 0x43, 0x44, 0x0]); // Example buffer
            client.write(buf);
            client.destroy();
            client = new net.Socket();
        });
    });   
    socket.on('err', function(err){
            console.log(err)
    });   

    /* Invalid for now */
    socket.on('packet', function(args) {
        console.log("Sending packet");
        buf = new Buffer([0x41, 0x42, 0x43, 0x44, 0x0]);
        client.write(buf);
        client.destroy();
        client = new net.Socket();

    })
});

/* Catch any webserver (port 3000) errors */
io.on('err', function(err){
    console.log(err)
})

/* Webserver listening on port 3000 */
http.listen(port,function(){
    log.info('Server listening through port %s', port);
});

http.on('err', function(err){
    console.log(err)
});

process.on('uncaughtException', function(err) {
  console.log('Caught exception: ' + err);
});

state_machine();

/***********************************************************/
/********************** Javascript functions ***************/
/***********************************************************/

function state_machine() {
    var state = ["NAVIGATING_SIDEWALK", "LOOKING_FOR_PATH" ,"FOUND_PATH_TO_HOUSE", "NAVIGATING_PATH" ,"REACHED_DOOR" , "DELIVER_PACKAGE" ]
    // Initial state Navigating Sidewalk
    var current_state = state[0];
    call_video_stream();
    //Read json file every half second
    setInterval(read_json_file, 500);
    read_json_file();
    
}

/* Calls Avi's edge detection script that writes to a json file */
function call_video_stream() {
    var pyshell = new PythonShell('../sidewalk/python/video_stream.py')
}


/* This json file should contain the degree of confidence and vanishing point angle */
function read_json_file(){
    var file = 'read.json'
    jsonfile.readFile(file, function(err, obj) {
        console.log(obj.angle);

        /* Control code that determines what to do from vanishing point */
        if(obj.angle > 84 && obj.angle < 96 ){
            client.connect(espPort, espIP, function() { //192.168.43.250 port 5000
                console.log('Connected to 192.168.43.250'); 
                console.log('Local client sending:', 0);
                buf = new Buffer([0, 0x4B, 0xFF]); // 0x4B is temp value 75
                client.write(buf);
                client.destroy();
                client = new net.Socket();
            });      
        } else {
            client.connect(espPort, espIP, function() {
                console.log('Connected to 192.168.43.250'); 
                console.log('Local client sending:', 0);
                buf = new Buffer([14, 0x4B, 0xFF]); // 0x4B is temp value 75
                client.write(buf);
                client.destroy();
                client = new net.Socket();
            });   
        }
    })
}

function ascii_to_hexa(str) {
    var arr1 = [];
    for (var n = 0; n < str.length; n ++) 
     {
        var hex = Number(str.charCodeAt(n)).toString(16);
        arr1.push(hex);
     }
    return arr1.join('');
} 
    

