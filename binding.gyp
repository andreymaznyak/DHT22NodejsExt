{
  "targets": [
    {
      "target_name": "dht22",
      "sources": [ "dht22.cc" ],
      "conditions": [
                ['OS=="linux"', {
                  "cflags!": [
                    "-lwiringPi",
                  ],
                }],
              ],
      "libraries": ["-lwiringPi"],        
      "cflags" : [ "-lwiringPi" ]
    }
  ]
}