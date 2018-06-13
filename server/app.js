var express = require("express");
var app = new express();
var http = require("http").Server(app);
var io = require("socket.io")(http); 

var net = require('net');

var jsonfile = require('jsonfile');
var Log = require('log'),
    log = new Log('debug')
 
var port = process.env.PORT || 3000;

app.use(express.static(__dirname + "/public" ));
 
app.get('/', function(req,res){
    res.redirect('index.html');
});

require('events').EventEmitter.prototype._maxListeners = 100; //max listeners

var PythonShell = require('python-shell');

var client = new net.Socket();  

client.on('err', function(err){
            // handle the error safely
            console.log("Client error");
            console.log(err)
        });

io.on('connection',function(socket){ 

    //////////////////////////////////////////////////////////////////
    // Logging server stuff //
    var server = net.createServer(function(sock) {
        sock.on('err', function(err){
            console.log("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n\n");
            console.log(err);
            console.log("\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
            server.close();
            server.listen(5002, '192.168.43.12');  
        });
        sock.on('data', function(data) {
            //console.log(data);
            //Packet Type
            var packetType = data.slice(0,1).toString();
            packetType = "0x" + ascii_to_hexa(packetType);
            packetType = parseInt(packetType);
            //console.log("Packet Type: ", packetType);
            //Payload Length
            var payloadLength = data.slice(1,2).toString();
            payloadLength = "0x" + ascii_to_hexa(payloadLength);
            payloadLength = parseInt(payloadLength);
            //console.log("Payload Len: ", payloadLength);
            //Payload
            var payload = data.slice(2,payload).toString();
            //console.log(payload);
            var payload = payload.split(":");

            //console.log("Payload: ", payload[0]);
            //console.log("Payload: ", payload[1]);
            //console.log("Payload: ", payload[2]);
            console.log(packetType , payloadLength, payload);            
            
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
            //var payload = data.slice(,1).toString(); 
            //var result = message.split(":");
            //console.log(result[0]);
            //console.log(result[1]);
            //console.log(result[2]);


            //byte = "0x" + ascii_to_hexa(byte);
            //byte = parseInt(byte);
            //socket.end(eval(data));
        });
    });

    server.listen(5002, '192.168.43.12'); 
    ///////////////////////////////////////////////////////////////// 


    var connected = false;
    socket.on('stream',function(image){
        socket.broadcast.emit('stream', image);
    });
    socket.on('err', function(err){
            // handle the error safely
            console.log(err)
        });
    socket.on('event',function(data){
        
        if(data.message == 20) {
            clearInterval(repeat);
            data.message = 14;
        }

        client.connect(5000, "192.168.43.250", function() { //192.168.43.250 port 5000
            console.log('Connected to 192.168.43.250'); 
            console.log('Local client sending:', data.message);
            buf = new Buffer([data.message, 0xA, 0xFF]); // 0xA is temp value
            client.write(buf);
            client.destroy();
            client = new net.Socket();
            client.on('err', function(err){
                // handle the error safely
                console.log(err)
            });
        });
    });
    socket.on('dirEvent',function(data){
        client.connect(5000, "192.168.43.250", function() { //192.168.43.250 port 5000
            console.log('Connected to 192.168.43.250'); 
            console.log('Local client sending:', data.message);
            buf = new Buffer([data.message, 0x32, 0xFF]); // 032
            client.write(buf);
            client.destroy();
            client = new net.Socket();
            client.on('err', function(err){
                // handle the error safely
                console.log(err)
            });
        });
    });    
    socket.on('dutyEvent',function(data){
        client.connect(5000, "192.168.43.250", function() { //192.168.43.250 port 5000
            console.log('Connected to 192.168.43.250'); 
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
        connected = true;
        client.connect(5000, args.ip, function() { //192.168.43.250 port 5000
            console.log('Connected'); 
            console.log("Sending packet");
            buf = new Buffer([0x41, 0x42, 0x43, 0x44, 0x0]); // Example buffer
            client.write(buf);
            client.destroy();
            client = new net.Socket();
        });
        client.on('err', function(err){
            // handle the error safely
            console.log(err)
        });
    });      
    socket.on('packet', function(args) {
        
        console.log("Sending packet");
        //buf = new Buffer(["40"]);  
        buf = new Buffer([0x41, 0x42, 0x43, 0x44, 0x0]);
        client.write(buf);
        //client.write(args.body);
        //process.exit(); 
        client.destroy();
        client = new net.Socket();
    })
 
});

io.on('err', function(err){
    // handle the error safely
    console.log(err)
})

http.listen(port,function(){
    log.info('Server listening through port %s', port);
});

http.on('err', function(err){
   // handle the error safely
    console.log(err)
});

process.on('uncaughtException', function(err) {
  console.log('Caught exception: ' + err);
});


var state = ["NAVIGATING_SIDEWALK", "LOOKING_FOR_PATH" ,"FOUND_PATH_TO_HOUSE", "NAVIGATING_PATH" ,"REACHED_DOOR" , "DELIVER_PACKAGE" ]
var current_state;
var repeat;
var state_repeat;
call_number_recognition();
call_video_stream();
//init_state_machine();


function init_state_machine() {
    // Initial state Navigating Sidewalk

                client.connect(5000, "192.168.43.250", function() { //192.168.43.250 port 5000
                    console.log('Connected to 192.168.43.250'); 
                    console.log('Local client sending:', 0);
                    buf = new Buffer([14, 0x4, 0xFF]); // 0x4B is temp value 75
                    client.write(buf);
                    client.destroy();
                    client = new net.Socket();
                    client.on('err', function(err){
                        // handle the error safely
                        console.log(err)
                    });
                });

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
    console.log("Calling video stream script.\n");
    var pyshell = new PythonShell('../sidewalk/python/video_stream.py')
}

function navigating_sidewalk(){
    //Read
    current_state = state[0];

    console.log("navigating sidewalk");

    var file = 'front.json';
    jsonfile.readFile(file, function(err, obj) {
        //console.log(obj.angle);

        if(obj.angle >= 85 && obj.angle <= 95 ){
                client.connect(5000, "192.168.43.250", function() { //192.168.43.250 port 5000
                    console.log('Connected to 192.168.43.250'); 
                    console.log('Local client sending:', 0);
                    buf = new Buffer([0, 0x1E, 0xFF,18, 0, 0xFF]); // 0x4B is temp value 75
                    client.write(buf);

                    client.destroy();
                    client = new net.Socket();
                    client.on('err', function(err){
                        // handle the error safely
                        console.log(err)
                    });
                });        
        } else if (obj.angle > 85 ) {
                client.connect(5000, "192.168.43.250", function() { //192.168.43.250 port 5000
                    console.log('Connected to 192.168.43.250'); 
                    console.log('Local client sending:', 0);
                    buf = new Buffer([7, 0x2, 0xFF, 18, 0, 0xFF]); // 0x4B is temp value 75
                    client.write(buf);

                    client.destroy();
                    client = new net.Socket();
                    client.on('err', function(err){
                        // handle the error safely
                        console.log(err)
                    });
                });                  
        } else if (obj.angle < 95 ) {
                client.connect(5000, "192.168.43.250", function() { //192.168.43.250 port 5000
                    console.log('Connected to 192.168.43.250'); 
                    console.log('Local client sending:', 0);
                    buf = new Buffer([6, 0x2, 0xFF, 18, 0, 0xFF]); // 0x4B is temp value 75
                    client.write(buf);

                    client.destroy();
                    client = new net.Socket();
                    client.on('err', function(err){
                        // handle the error safely
                        console.log(err)
                    });
                });                  
        } else {
                client.connect(5000, "192.168.43.250", function() { //192.168.43.250 port 5000
                    console.log('Connected to 192.168.43.250'); 
                    console.log('Local client sending:', 14);
                    buf = new Buffer([14, 0x4B, 0xFF, 18, 0, 0xFF]); // 0x4B is temp value 75
                    client.write(buf);

                    client.destroy();
                    client = new net.Socket();
                    client.on('err', function(err){
                        // handle the error safely
                        console.log(err)
                    });
                });        
        }
        return 0;

    })
    //output
}

function looking_for_path() {
    current_state = state[1];

    console.log("HLooking for path");
                //STOP
                
                client.connect(5000, "192.168.43.250", function() { //192.168.43.250 port 5000
                    console.log('Connected to 192.168.43.250'); 
                    console.log('Local client sending:', 0);
                    buf = new Buffer([14, 0x4, 0xFF, 18, 1, 0xFF]); // 0x4B is temp value 75
                    client.write(buf);
                    client.destroy();
                    client = new net.Socket();
                    client.on('err', function(err){
                        // handle the error safely
                        console.log(err)
                    });
                });  
                setTimeout(found_path_to_house, 1000);                 

}

function found_path_to_house() {
    current_state = state[2];

    console.log("found path to house");
                    client.connect(5000, "192.168.43.250", function() { //192.168.43.250 port 5000
                    console.log('Connected to 192.168.43.250'); 
                    console.log('Local client sending:', 0);
                    buf = new Buffer([17, 0x4, 0xFF, 18, 2, 0xFF]); // 0x4B is temp value 75
                    client.write(buf);
                    client.destroy();
                    client = new net.Socket();
                    client.on('err', function(err){
                        // handle the error safely
                        console.log(err)
                    });
                });  
                setTimeout(navigating_path, 1000);      
    // Pivot 90 degrees and go down new path
}


function navigating_path() {
    current_state = state[4];
    console.log("navigate path");
                    client.connect(5000, "192.168.43.250", function() { //192.168.43.250 port 5000
                    console.log('Connected to 192.168.43.250'); 
                    console.log('Local client sending:', 0);
                    buf = new Buffer([0,0x32, 0xFF, 18, 3, 0xFF]);
                    client.write(buf);
                    client.destroy();
                    client = new net.Socket();
                    client.on('err', function(err){
                        // handle the error safely
                        console.log(err)
                    });
                });  


    console.log("navigating_path");
    // continue until sensors reach door
}

function reached_door() {
    current_state = state[5];

    console.log("reached_door");
    // deliver package
}

function deliver_package() {
    current_state = state[5];

    console.log("deliver package");
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
    
