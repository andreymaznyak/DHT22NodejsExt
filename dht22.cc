#include <node.h>

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//#define MAX_TIMINGS    85
//#define DHT_PIN        4    /* GPIO-22 */

int data[5] = { 0, 0, 0, 0, 0 };

using namespace v8;

int read_dht_data(float& h, float& c, int DHT_PIN = 4, int MAX_TIMINGS = 85)
{
    uint8_t laststate    = HIGH;
    uint8_t counter        = 0;
    uint8_t j            = 0, i;
  
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;
  
    /* pull pin down for 18 milliseconds */
    pinMode( DHT_PIN, OUTPUT );
    digitalWrite( DHT_PIN, LOW );
    delay( 18 );
  
    /* prepare to read the pin */
    pinMode( DHT_PIN, INPUT );
  
    /* detect change and read data */
    for ( i = 0; i < MAX_TIMINGS; i++ )
    {
        counter = 0;
        while ( digitalRead( DHT_PIN ) == laststate )
        {
            counter++;
            delayMicroseconds( 1 );
            if ( counter == 255 )
            {
                break;
            }
        }
        laststate = digitalRead( DHT_PIN );
  
        if ( counter == 255 )
            break;
  
        /* ignore first 3 transitions */
        if ( (i >= 4) && (i % 2 == 0) )
        {
            /* shove each bit into the storage bytes */
            data[j / 8] <<= 1;
            if ( counter > 16 )
                data[j / 8] |= 1;
            j++;
        }
    }
  
    /*
     * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
     * print it out if data is good
     */
    if ( (j >= 40) &&
         (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) )
    {
        h = (float)((data[0] << 8) + data[1]) / 10;
        if ( h > 100 )
        {
            h = data[0];    // for DHT11
        }
        c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
        if ( c > 125 )
        {
            c = data[2];    // for DHT11
        }
        if ( data[2] & 0x80 )
        {
            c = -c;
        }
        //float f = c * 1.8f + 32;
        return 1;
        //fprintf( stdout, "Humidity = %.1f %% Temperature = %.1f *C (%.1f *F)\n", h, c, f );
    }else  {
        return -1;
        //fprintf( stdout, "Data not good, skip\n" );
    }
}

void Method(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, " hello world "));
}

void MethodGetDHT22Data(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  
  if ( wiringPiSetup() == -1 ){
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, "wiringPi setup err"));
  }else{
    float h = 0, c = 0;
  
    if(read_dht_data(h,c) == 1){
      Local<Object> obj = Object::New(isolate);
      obj->Set(String::NewFromUtf8(isolate, "Humidity"), Number::New(isolate, h));
      obj->Set(String::NewFromUtf8(isolate, "Temperature"), Number::New(isolate, c));

      args.GetReturnValue().Set(obj);
    }else{
      args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Data not good, skip"));
    }
  }
  
}

void init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "hello", Method);
  NODE_SET_METHOD(exports, "getDHT22Data", MethodGetDHT22Data);
}

NODE_MODULE(addon, init)