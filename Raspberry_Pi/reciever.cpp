#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <RF24/RF24.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;
//
// Hardware configuration
//

<<<<<<< HEAD
=======
/****************** Raspberry Pi ***********************/

>>>>>>> 47003bb3f46847facbc539dabe0048867f9784da
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
unsigned long t0,gnuplotTimer,startTime, stopTime, counter, rxTimer=0;

<<<<<<< HEAD
=======
typedef struct{
	int16_t x;
	int16_t y;
	int16_t z;
} pyStruct;

>>>>>>> 47003bb3f46847facbc539dabe0048867f9784da
int main(int argc, char** argv){
	bool role_ping_out = 1, role_pong_back = 0;
	bool role = 0;
	cout << "Jefferson Seizure Buddy Project\n";
	//setup radio and print info
	radio.begin();                           // Setup and configure rf radio
	radio.setChannel(1);
	radio.setPALevel(RF24_PA_MAX);
	radio.setDataRate(RF24_1MBPS);
	radio.setAutoAck(1);                     // Ensure autoACK is enabled
	radio.setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
	radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
<<<<<<< HEAD
	// radio.printDetails();
	
=======
	radio.printDetails();
>>>>>>> 47003bb3f46847facbc539dabe0048867f9784da
	/********* Role chooser ***********/
	printf("\n ************ Role Setup ***********\n");
	string input = "";
	char myChar = {0};
	
	//cout << "Choose a role: Enter 0 for receiver, 1 for transmitter (CTRL+C to exit)\n>";
<<<<<<< HEAD
=======
	//getline(cin,input);
>>>>>>> 47003bb3f46847facbc539dabe0048867f9784da
	input = "0";
	if(input.length() == 1) {
		myChar = input[0];
		if(myChar == '0'){ //recieve
			cout << "Role: Pong Back, awaiting transmission " << endl << endl;
			role = role_pong_back;
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

<<<<<<< HEAD
	for(int i=0; i < 32; i++){
=======
	for(int i=0; i<32; i++){
>>>>>>> 47003bb3f46847facbc539dabe0048867f9784da
		data[i] = rand() % 255;    //Load the buffer with random data
	}
	FILE *f = fopen("/home/pi/Desktop/data.csv","w");
	//FILE *gnuplotFile = fopen("/home/pi/Desktop/data.dat","w");
<<<<<<< HEAD
	if(f == NULL){
=======
	if(f==NULL){
>>>>>>> 47003bb3f46847facbc539dabe0048867f9784da
		printf("Error opening file!\n");
		exit(1);
	}
	fprintf(f,"ArTime,Pi_Time,AcX,AcY,AcZ,GyX,GyY,GyZ\n"); //heading
	// forever loop
<<<<<<< HEAD
	t0 = millis();
	gnuplotTimer = millis();
=======
	t0=millis();
	gnuplotTimer=millis();
>>>>>>> 47003bb3f46847facbc539dabe0048867f9784da
	while (1){
		if (role == role_ping_out){
			sleep(2);
			printf("Initiating Basic Data Transfer\n\r");
			//long int cycles = 10000; 					//Change this to a higher or lower number
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
			//printf("test");
			while(radio.available()){
				//printf("test2\n");
		      		radio.read(&recievedata,32);
		      		counter++;
			   	if(millis() - rxTimer > 1){
				     	rxTimer = millis();
					// time on arduino
					printf("%d,", (recievedata[13] << 32 | recievedata[14] << 16 | recievedata[15] << 8 |  recievedata[16]));	     	
					fprintf(f,"%d,", (recievedata[13] << 32 | recievedata[14] << 16 | recievedata[15] << 8 |  recievedata[16]));
					fprintf(f,"%d,",millis()-t0); //Time on  RPI 					
					//acc data
					printf("%d,", (int16_t)(recievedata[1] << 8 | recievedata[2]));	     	
					fprintf(f,"%d,", (int16_t)(recievedata[1] << 8 | recievedata[2])); //AcX
					printf("%d,", (int16_t)(recievedata[3] << 8 | recievedata[4]));	     	
					fprintf(f,"%d,", (int16_t)(recievedata[3] << 8 | recievedata[4])); //AcY
					printf("%d\n", (int16_t)(recievedata[5] << 8 | recievedata[6]));	     	
					fprintf(f,"%d,", (int16_t)(recievedata[5] << 8 | recievedata[6])); //AcZ
					//gyro data
				     	//printf("%d,", (int16_t)(recievedata[7] << 8 | recievedata[8]));	     	
					fprintf(f,"%d,", (int16_t)(recievedata[7] << 8 | recievedata[8])); //GyX
					//printf("%d,", (int16_t)(recievedata[9] << 8 | recievedata[10]));	 	
					fprintf(f,"%d,", (int16_t)(recievedata[9] << 8 | recievedata[10])); //GyY
					//printf("%d\n", (int16_t)(recievedata[11] << 8 | recievedata[12]));	
					fprintf(f,"%d\n", (int16_t)(recievedata[11] << 8 | recievedata[12])); //GyZ
					counter = 0;
					fclose(f);
					FILE *f = fopen("/home/pi/Desktop/data.csv","a");
<<<<<<< HEAD
=======
					
					//pyStruct py = {(int16_t)(recievedata[1] << 8 | recievedata[2]),
					//			 (int16_t)(recievedata[3] << 8 | recievedata[4]),
					//			 (int16_t)(recievedata[5] << 8 | recievedata[6])};
					//fwrite(&py, sizeof py, 1, stdout);

					//data so gnuplot can graph it
					//fprintf(gnuplotFile,"%d ",millis()-t0); //Time on  RPI 					
					//acc data
					//fprintf(gnuplotFile,"%d ", (int16_t)(recievedata[1] << 8 | recievedata[2])); //AcX
					//fprintf(gnuplotFile,"%d ", (int16_t)(recievedata[3] << 8 | recievedata[4])); //AcY
					//fprintf(gnuplotFile,"%d\n", (int16_t)(recievedata[5] << 8 | recievedata[6])); //AcZ

					//if(millis() - gnuplotTimer > 1000){
					//	gnuplotTimer = millis();
					//	fclose(gnuplotFile);
					//	FILE *gnuplotFile = fopen("/home/pi/Desktop/pydata.csv","w");					
					//}	
>>>>>>> 47003bb3f46847facbc539dabe0048867f9784da
				}
			} 
		  }
	} // loop
	 fclose(f);
} // main
