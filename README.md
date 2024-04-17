# HW-Eaton-Device-Monitor
Eaton Interview Homework.

## Home work prompt : 
The problem to solve is following:

                You monitor devices, which are sending data to you.
                Each device has a unique name.
                Each device produces measurements. 

Challange is:

                Compute number of messages you got or read from the devices.

The solution can be in any language, preferably modern C++.
The scope is open, you must decide how the “devices” will work in your system.
The solution should be posted on GitHub or a similar page for a review.
Please add Makefile/CMakeLists and documentation explaining us how to run your code.

# Our Proposed Solution
Project consists of 2 executables, one for the Device Monitor, and a second one for Devices.
The Device Monitor is a TCP server that listen from incoming connections from Devices (clients).
The Devices connect to the server and send their name and measurement (not used further) serialized using JSON.
The Device Monitor deserializes the received data, and counts messages according to their senders.
# About Security
A TLS layer is added on top of the TCP sockets, we implemented a symetric encryption scheme. Devices have a copy of the 
server's certificate and check their local copies against the one sent by the server after a connection, this way, 
the server cannot be impersonated.

# Repository Tree
```
.
├── apps
│   ├── device
│   │   └── main.cpp
│   └── device-monitor
│       └── main.cpp
├── certificates
│   ├── cert-bad.pem
│   ├── cert.pem
│   └── key.pem
├── include
│   ├── nlohmann
│   │   ├── json_fwd.hpp
│   │   └── json.hpp
│   ├── Socket
│   │   └── Socket.h
│   └── SSLsocket
│       └── SSLsocket.h
├── lib
├── MakeAndRunUnitTests.mk
├── MakeDevice.mk
├── MakeMonitor.mk
├── mocks
├── README.md
├── src
│   ├── Socket
│   │   └── Socket.cpp
│   └── SSLsocket
│       └── SSLsocket.cpp
└── tests
    ├── AllTests.cpp
    ├── Socket
    │   └── SocketTests.cpp
    └── SSLsocket
        └── SSLsocketTests.cpp
```

# Building and Running the code
The code was built with `OpenSSL 1.1.1f`. Under Ubuntu 20.04.6 LTS. 

Use the following command to build the Device Monitor's executable
```
make -f MakeMonitor.mk
```
The excutable will be saved under `build_monitor/device-monitor.exe`. It can then be run using the command 
```
./build_monitor/device-monitor.exe 8081 certificates/cert.pem certificates/key.pem
```
You can replace the port number and certificate files at your convienience.

From the root folder of the repository, use the following command to build the Device executable 
```
make -f MakeDevice.mk
```
The executable will be saved under `build_device/device.exe` , it can then be run (from another terminal/computer) using the following command

```
./build_device/device.exe localhost 8081 Device1 certificates/cert.pem 
```
localhost is the address of the computer on which the server is running.

The remote port should be the same as the the one being listen to by the server.

The certificate file should be the same as the one used by the server.

# Building And Running Unit Tests
Building and running unit tests requires CppUTest. Download and build from source as described in http://cpputest.github.io/

Before building export the CppUTest directory using 
```
export CPPUTEST_HOME=<CppUtest directory>
```
You can the use the following command (from the repository's root folder) to build and run unittests 
```
make -f MakeAndRunUnitTests.mk [gcov]
```
You can add (optional) gcov to generate code coverage statistics. As for now they are as follows : 

 src/SSLsocket/SSLsocket.cpp : 71.56%  
 src/Socket/Socket.cpp : 84.21% 

 # Possible Improvements
 As for now, the reception of incoming connections is performed in a single thread, this can be handle in multiple threads (one thread for each connection).
 

