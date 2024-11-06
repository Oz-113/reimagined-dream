
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <wiringPi.h>
#include <wiringSerial.h>
std::atomic<bool> running(true);
void writeSerial(int serial_port) {
    while (running) {
        std::string message;
        std::getline(std::cin, message);  // Kullanıcıdan veri al
        for (char c : message) {
            serialPutchar(serial_port, c); // Seri porttan gönder
        }
        serialPutchar(serial_port, '\n'); // Mesaj sonunda satır atla

    }
}
void reqData(int serial_port){
while(running){
	
	serialPutchar(serial_port, '.');
	delay(1000);
}

}
void readSerial(int serial_port) {
	using namespace std;
	 
  int sayac=0;
  int intler[3];
  float dafloat;
  string iplik;
  char dat;
  uint8_t espid;
  string mesag = "";
  string bir;
  string iki;
  string uc;
  string dort;
  string bes;
    while (running) {

      if(serialDataAvail (serial_port) )
	{ 
		dat = serialGetchar(serial_port);
		
		switch (dat){
			case '?':
			espid = stoi(mesag);
			
			mesag = "";
			switch(espid){
			case 1:
			bir ="mesaj: ";
			iki = "mesafe: ";
			uc = "potansiyometre: ";
			dort ="boolean: ";
			bes ="sıcaklık: ";

			break;
			case 2:
			bir ="mesaj: ";
			iki = "int 100-200: ";
			uc = "int 1-100: ";
			dort ="boolean : ";
			bes ="float : ";

			break;

			}
			break;
			case '#':
			cout<<"-----------------"<<endl;
		
			break;	
			case '&':

			// serialPrintf(serial_port,"alındı");
		
			iplik=mesag;
			cout<<bir<<iplik<<endl;
			mesag="";
			dat = '0';
			break;

			case '$':

			// serialPrintf(serial_port,"alındı");
			
			intler[sayac]=stoi(mesag);
			mesag="";
			dat = '0';
			switch (sayac){
				case 0:
				cout<<iki;
				break;
				case 1:
				cout<<uc;
				break;
				case 2:
				cout<<dort;
				break;
				
			}
			cout<<intler[sayac]<<endl;
			sayac++;if(sayac>2){sayac=0;}
			break;

			case '!':
			// serialPrintf(serial_port,"alındı");
			
			dafloat=stof(mesag);
			cout<<bes<<dafloat<<endl;
			mesag="";
			dat = '0';


			break;
		default:
			mesag+=dat;
			}	/* receive character serially*/	
		
		
		//serialPutchar(serial_port,dat);
	
			/* transmit character serially on port */
		
	}
        }
    }


int main ()
{
	using namespace std;
 int serial_port ;
  if ((serial_port = serialOpen ("/dev/ttyS0", 115200)) < 0)	/* open serial port */
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  if (wiringPiSetup () == -1)					/* initializes wiringPi setup */
  {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }
 	thread readerThread(readSerial, serial_port);
    thread writerThread(writeSerial, serial_port);
	thread reqdat(reqData,serial_port);

	
    readerThread.join();
    writerThread.join();
    reqdat.join();
serialClose(serial_port);

}
