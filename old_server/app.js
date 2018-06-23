var express = require("express");
var app = new express();
var http = require("http").Server(app);
var io = require("socket.io")(http); 
var net = require('net');
var jsonfile = require('jsonfile');
var Log = require('log'),
    log = new Log('debug')
 
var port = process.env.PORT || 3000;

var debug_on = 1; // 0 deactivates print statements, 1 turns them on

var serverListenPort = 5002;
var serverListenAddress = '192.168.43.12';

var clientPort = 5000;
var clientAddress = "192.168.43.250";

const dummyValue = 0xFF;

app.use(express.static(__dirname + "/public" ));
 
app.get('/', function(req,res){
    res.redirect('index.html');
});

require('events').EventEmitter.prototype._maxListeners = 100; //max listeners

var PythonShell = require('python-shell');

var client = new net.Socket();  

client.on('err', function(err){
    // handle the error safely
    debug_log("Client error");
    debug_log(err)
});

io.on('connection',function(socket){ 

    //////////////////////////////////////////////////////////////////
    // Logging server stuff //
    var server = net.createServer(function(sock) {
        sock.on('err', function(err){
            debug_log(err);
            server.close();
            server.listen(serverListenPort, serverListenAddress);  
        });
        sock.on('data', function(data) {
            //Packet Type
            var packetType = data.slice(0,1).toString();
            packetType = "0x" + ascii_to_hexa(packetType);
            packetType = parseInt(packetType);
            var payloadLength = data.slice(1,2).toString();
            payloadLength = "0x" + ascii_to_hexa(payloadLength);
            payloadLength = parseInt(payloadLength);
            //Payload
            var payload = data.slice(2,payload).toString();
            //console.log(payload);
            payload = payload.split(":");
            debug_log(payload);            
            
            switch(packetType)
            {
                case 0: socket.broadcast.emit('infoDiag', payload); break;
                case 1: socket.broadcast.emit('errorDiag', payload); break;
                case 2: socket.broadcast.emit('statusDiag', payload); break;
                case 3: socket.broadcast.emit('log', payload); break;
            }
        });
    });

    server.listen(serverListenPort, serverListenAddress); 
    ///////////////////////////////////////////////////////////////// 


    var connected = false;
    socket.on('stream',function(image){
        socket.broadcast.emit('stream', image);
    });
    socket.on('err', function(err){
            // handle the error safely
            debug_log(err)
        });
    socket.on('event',function(data){
        
        if(data.message == 20) {
            clearInterval(repeat);
            data.message = 14;
        }

        clientConnect(data.message, 0xA);
    });
    socket.on('dirEvent',function(data){
        clientConnect(data.message, 0x32);
    });    
    socket.on('dutyEvent',function(data){
        clientConnect(12, data.message);
    });
    socket.on('exit', function(ignore) { 
        socket.disconnect(true);
        debug_log("Server disconnecting.");
        process.exit(); 
    });
});

io.on('err', function(err){
    // handle the error safely
    debug_log(err)
})

http.listen(port,function(){
    log.info('Server listening through port %s', port);
});

http.on('err', function(err){
   // handle the error safely
    debug_log(err)
});

process.on('uncaughtException', function(err) {
  debug_log('Caught exception: ' + err);
});


const state = {"NAVIGATING_SIDEWALK":0, "LOOKING_FOR_PATH":1 ,"FOUND_PATH_TO_HOUSE":2, "NAVIGATING_PATH":3 ,"REACHED_DOOR":4, "DELIVER_PACKAGE":5 }
var current_state;
var repeat;
var state_repeat;
call_number_recognition();
call_video_stream();
//init_state_machine();

function init_state_machine() {
    // Initial state Navigating Sidewalk

    clientConnect(14, 0x4);

    repeat = setInterval(navigating_sidewalk, 500);
}

function restart_state_machine(){
    stop_state_machine();
    init_state_machine();
}

function stop_state_machine(){
    clearInterval(repeat);
    clearInterval(state_repeat);
}

function call_video_stream() {
    debug_log("Calling video stream script.\n");
    var pyshell = new PythonShell('../sidewalk/python/video_stream.py')
}

function navigating_sidewalk(){
    //Read
    current_state = state["NAVIGATING_SIDEWALK"];

    debug_log("navigating sidewalk");

    var file = 'front.json';
    jsonfile.readFile(file, function(err, obj) {
        //console.log(obj.angle);

        if(obj.angle >= 85 && obj.angle <= 95 ){
            clientConnectSendState(0,0x1E,0);       
        } else if (obj.angle > 85 ) {
            clientConnectSendState(7,0x2,0);                 
        } else if (obj.angle < 95 ) {
            clientConnectSendState(6,0x2,0);                 
        } else {
            clientConnectSendState(14,0x4B,0);    
        }
        return 0;

    })
}

function looking_for_path() {
    current_state = state["LOOKING_FOR_PATH"];

    debug_log("Looking for path");
                //STOP
    clientConnectSendState(14,0x4,1); 
    setTimeout(found_path_to_house, 1000);                 

}

function found_path_to_house() {
    current_state = state["FOUND_PATH_TO_HOUSE"];

    debug_log("found path to house");
    clientConnectSendState(17,0x4,2);  
    setTimeout(navigating_path, 1000);      
    // Pivot 90 degrees and go down new path
}


function navigating_path() {
    current_state = state["NAVIGATING_PATH"];
    debug_log("navigate path");
    clientConnectSendState(0,0x32,3);  
    debug_log("navigating_path");
    // continue until sensors reach door
}

function reached_door() {
    current_state = state["REACHED_DOOR"];

    debug_log("reached_door");
    // deliver package
}

function deliver_package() {
    current_state = state["DELIVER_PACKAGE"];

    debug_log("deliver package");
    // deliver package
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

function debug_log(str, str2 = 0) {
    if (debug_on) {
        if (!str2) {
            console.log(str, str2);
        } else {
            console.log(str);
        }
        
    }
}

function clientConnect(opcode, inputValue) {

    client.connect(clientPort, clientAddress, function() { 
        debug_log('Connected to ', clientAddress); 
        debug_log('Local client sending:', data.message);
        buf = new Buffer([opcode, inputValue, dummyValue]); 
        client.write(buf);
        client.destroy();
        client = new net.Socket();
        client.on('err', function(err){
            // handle the error safely
            console.log(err)
        });
    });
}

function clientConnectSendState(opcode, inputValue, curState) {

    client.connect(clientPort, clientAddress, function() {
        debug_log('Connected to ', clientAddress); 
        debug_log('Local client sending:', data.message);
        buf = new Buffer([opcode, inputValue, dummyValue, 18, curState, dummyValue]);
        client.write(buf);
        client.destroy();
        client = new net.Socket();
        client.on('err', function(err){
            // handle the error safely
            console.log(err)
        });
    });
}
