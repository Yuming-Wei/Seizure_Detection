#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <RF24/RF24.h>
#include <unistd.h>

using namespace std;
//
// Hardware configuration
//

/****************** Raspberry Pi ***********************/

// Radio CE Pin, CSN Pin, SPI Speed
// See http://www.airspayce.com/mikem/bcm2835/group__constants.html#ga63c029bd6500167152db4e57736d0939 and the related enumerations for pin information.

// Setup for GPIO 15 CE and CE0 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t addresses[2] = { 0xABCDABCD71LL, 0x544d52687CLL };

//char array that holds the transmitted data
uint8_t data[32];
uint8_t recievedata[32];
//timers for tracking speed of transfer
unsigned long startTime, stopTime, counter, rxTimer=0;

int main(int argc, char** argv){
	bool role_ping_out = 1, role_pong_back = 0;
	bool role = 0;
	cout << "RF24/examples/Transfer/\n";
	//setup radio and print info
	radio.begin();                           // Setup and configure rf radio
	radio.setChannel(1);
	radio.setPALevel(RF24_PA_MAX);
	radio.setDataRate(RF24_1MBPS);
	radio.setAutoAck(1);                     // Ensure autoACK is enabled
	radio.setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
	radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
	radio.printDetails();
	/********* Role chooser ***********/
	printf("\n ************ Role Setup ***********\n");
	string input = "";
	char myChar = {0};
	

	//cout << "Choose a role: Enter 0 for receiver, 1 for transmitter (CTRL+C to exit)\n>";
	//getline(cin,input);
	input = "0";
	if(input.length() == 1) {
		myChar = input[0];
		if(myChar == '0'){ //recieve
			cout << "Role: Pong Back, awaiting transmission " << endl << endl;
		}else{  //transmit
			cout << "Role: Ping Out, starting transmission " << endl << endl;
			role = role_ping_out;
		}
	}
	/***********************************/

	if ( role == role_ping_out )    {
		radio.openWritingPipe(addresses[1]);
		radio.openReadingPipe(1,addresses[0]);
		radio.stopListening();
	} else {
		radio.openWritingPipe(addresses[0]);
		radio.openReadingPipe(1,addresses[1]);
		radio.startListening();
	}

	for(int i=0; i<32; i++){
		data[i] = rand() % 255;    //Load the buffer with random data
	}
	FILE *f = fopen("data.csv","w");
	if(f==NULL){
		printf("Error opening file!\n");
		exit(1);
	}
	fprintf(f,"AcX,AcY,AcZ,GyX,GyY,GyZ\n"); //AcX
	// forever loop
	while (1){
		if (role == role_ping_out){
			sleep(2);
			printf("Initiating Basic Data Transfer\n\r");
			long int cycles = 10000; 					//Change this to a higher or lower number
			// unsigned long pauseTime = millis();		//Uncomment if autoAck == 1 ( NOACK )
			startTime = millis();
			//for(int i=0; i<cycles; i++){        		//Loop through a number of cycles
	      		data[0] = 123;                        //Change the first byte of the payload for identification
			role = role_pong_back;
	      		if(!radio.writeFast(&data,32)){     //Write to the FIFO buffers
				counter++;                      //Keep count of failed payloads
	      		}
		}
		if(role == role_pong_back){
			while(radio.available()){
		      		radio.read(&recievedata,32);
		      		counter++;
			   	if(millis() - rxTimer > 100){
				     	rxTimer = millis();
					// time on arduino
					printf("%d,", (recievedata[13] << 32 | recievedata[14] << 16 | recievedata[15] << 8 |  recievedata[16]));	     	fprintf(f,"%d,", (recievedata[13] << 32 | recievedata[14] << 16 | recievedata[15] << 8 |  recievedata[16]));
					//acc data
					printf("%d,", (int16_t)(recievedata[1] << 8 | recievedata[2]));	     	fprintf(f,"%d,", (int16_t)(recievedata[1] << 8 | recievedata[2])); //AcX
					printf("%d,", (int16_t)(recievedata[3] << 8 | recievedata[4]));	     	fprintf(f,"%d,", (int16_t)(recievedata[3] << 8 | recievedata[4])); //AcY
					printf("%d,", (int16_t)(recievedata[5] << 8 | recievedata[6]));	     	fprintf(f,"%d,", (int16_t)(recievedata[5] << 8 | recievedata[6])); //AcZ
					//gyro data
				     	printf("%d,", (int16_t)(recievedata[7] << 8 | recievedata[8]));	     	fprintf(f,"%d,", (int16_t)(recievedata[7] << 8 | recievedata[8])); //AcX
					printf("%d,", (int16_t)(recievedata[9] << 8 | recievedata[10]));	 	fprintf(f,"%d,", (int16_t)(recievedata[9] << 8 | recievedata[10])); //AcY
					printf("%d\n", (int16_t)(recievedata[11] << 8 | recievedata[12]));	fprintf(f,"%d\n", (int16_t)(recievedata[11] << 8 | recievedata[12])); //AcZ
					counter = 0;
					fclose(f);
					FILE *f = fopen("data.csv","a");	
				}
			} 
		  }
	} // loop
	 fclose(f);
} // main






