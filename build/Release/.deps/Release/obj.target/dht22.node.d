cmd_Release/obj.target/dht22.node := g++ -shared -pthread -rdynamic  -Wl,-soname=dht22.node -o Release/obj.target/dht22.node -Wl,--start-group Release/obj.target/dht22/dht22.o -Wl,--end-group -lwiringPi
