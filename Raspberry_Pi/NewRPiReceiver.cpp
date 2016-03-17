#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <RF24/RF24.h>

#include <json/json.h>
using namespace std;

RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_16MHZ);

const uint64_t pipes = 0xE8E8F0F0E1LL;

// Define a struct for storing acc and gyro value
struct ACC_GYRO {
  float AccX = 0, AccY = 0, AccZ = 0;
  float GyroX = 0, GyroY = 0, GyroZ = 0;
};

// Define a duration of timeout 
uint16_t timeout_ttl = 1000;

// Get a variable to store IMU data
ACC_GYRO IMU_Data;

// Print the status message
void finish(bool sucess, int error, char * message)
{
	char status[6];
	if (sucess)
		strcpy(status, "true");
	else
		strcpy(status, "false");
	printf("{\"status\":\"%s\",\"code\":\"%d\",\"message\":\"%s\"}", status, error, message);
	
	if (sucess)
		exit(0);
	else
		exit(1);
}

// Main function
int main(int argc, char** argv)
{
   // Accert when there is no exact two arguments
   if (argc!=2)
	finish(false, 1, (char *) "Missing JSON");
   // Get the arguments
   json_object * jobj = json_tokener_parse( (char *) argv[1]);
   if (jobj==0)
   // When no argument got, print error
	finish(false, 2, (char *) "JSON not valid");

   srand(time(NULL));
   
   /*
    *   Configuration of RF radio
    */
   radio.begin();
   radio.setAutoAck(1); // Ensure autoACK is enabled
   radio.setRetries(15,15);
   radio.enableDynamicPayloads();
   radio.openWritingPipe(pipes);
   radio.openReadingPipe(1,pipes);
   radio.startListening();
	
   // Start counting time in millisecond
   unsigned long started_waiting_at = millis();
   bool timeout = false;
   
   
   while ( !radio.available() && !timeout )
    // Determine whether it's timeout
	if (millis() - started_waiting_at > timeout_ttl )
	    timeout = true;
                     
   if ( timeout )
    // Print timeout error messagge
	finish(false, 5, (char *) "Timed-out");
    
   // Read the data from the transmisster 
   radio.read(&IMU_Data, sizeof(ACC_GYRO));

   radio.stopListening();
   finish(true, 0, "Finish");
}
