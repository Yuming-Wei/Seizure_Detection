#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#define RF_CE    7
#define RF_CSN   8
#define LED_PIN  6

RF24 radio(RF_CE,RF_CSN);

byte myID = 0x01;

byte pipes[][7] = {"master","slave","idle"};

struct payload_request_t
{
  uint8_t number;
  uint8_t destination;
  char message[14];
};

struct payload_general_t
{
  uint8_t number;
  char message[15];
};

payload_request_t incoming;
payload_general_t outgoing;

void sendReply()
{
  Serial.print("answering with \"");
  Serial.print(outgoing.message);
  Serial.println("\"");
  
  radio.stopListening();
  radio.openWritingPipe(pipes[1]);
  delay(10);
  radio.write(&outgoing,sizeof(payload_general_t)+1);
  delay(10);
  radio.startListening();
}

/*
uint64_t ticktack;

void sendEvent()
{
  Serial.print("sending asyncronous event/message ");
  Serial.print(outgoing.message);
  Serial.println("\"");
  radio.stopListening();
  radio.openWritingPipe(pipes[2]);
  delay(10);
  outgoing.node = myID;
  radio.write(&outgoing,sizeof(payload_general_t)+1);
  delay(10);
  radio.startListening();
}
*/

byte last = 0;

void setup() {

  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  
   printf_begin();
   radio.begin();   
   radio.setAutoAck(1); // Ensure autoACK is enabled
   radio.setRetries(1,3);
   radio.enableDynamicPayloads();
   radio.openWritingPipe(pipes[1]);
   radio.openReadingPipe(1, pipes[0]);
   radio.startListening();
   radio.printDetails();   
}

void loop(void)
{
/*
    if (millis() - ticktack > 2500)
    {
      sprintf(outgoing.message, "%ld", ticktack);
      sendEvent();
      ticktack = millis();
    }
*/    
    if(radio.available())
    {
        radio.read(&incoming, sizeof(payload_request_t));
        Serial.println(incoming.message);        
        if (incoming.destination==myID)
        {
          
          if (strncmp(incoming.message, "get", 3) == 0)
          {
            if (last > 0)
              strcpy(outgoing.message, "on");
            else
              strcpy(outgoing.message, "off");
          }
          else if (strncmp(incoming.message, "set", 3) == 0)
          {
            if (strncmp(&incoming.message[4], "on", 2)==0)
            {
              digitalWrite(LED_PIN, HIGH);
              last = 1;
              strcpy(outgoing.message, "ok");
            } else if (strncmp(&incoming.message[4], "off", 3)==0)
            {
              digitalWrite(LED_PIN, LOW);
              last = 0;
              strcpy(outgoing.message, "ok");
            }
            else
              strcpy(outgoing.message, "?");
          }
          else
          {
            strcpy(outgoing.message, "?");
          }
          outgoing.number = incoming.number;
          sendReply();
        }
    }
    delay(100);
    //radio.stopListening();
    //radio.openWritingPipe(pipes[1]);
    //delay(10);
    //strcpy(outgoing.message, "data");
    //radio.write(&outgoing,sizeof(payload_general_t)+1);
    //delay(10);
    //radio.startListening();
}

