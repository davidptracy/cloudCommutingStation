 // Dependency of ID20Reader. Must include in main file due to Arduino software limitations.
#include <SoftwareSerial.h>                           
#include <ID20Reader.h>

// Dependencies for the Arduino Yun's processor to communicate with its linux 
#include <Bridge.h>
#include <Process.h>

// An array holding all card IDs known to the system
String playerIds[] = {"75004812CA", "7500485250", "7500481018", "7500483954", "7500485523", "7500483478", 
"750047FE7E", "750048192B", "75004831DB", "750047FECB", "6A0049DE4D", "6A0049DCC5", "6A0049F440", "6A0049DD55", "6A0049D8AA" };

// An array holding all of the player Names (numbers for now)
int playerNames[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};                    

// The current station's ID, this must be changed for every station
int stationId = 6;

//Data input pin for the RFID reader
int rx_pin = 8; 

//Unused, but must be defined. (Nothing is sent from the Arduino to the reader.)
int tx_pin = 1;

//myStationId = 1;

//An instance of ID20Reader.
ID20Reader rfid(rx_pin, tx_pin);

void setup() {
  
  //delay is required so that arduino processor does not interrupt linux boot on initial startup and reset
  delay(5000);
  Serial.begin(9600);
  Bridge.begin(); 
 
}

void loop() {
  
  //Receive a tag from the reader if available
  rfid.read(); 
  
  //Get the tag
  if(rfid.available()){    
      String code = rfid.get(); 
      Serial.println(code);
      // pass the code to the checkPlayer function      
      checkPlayer(code);           
   }  
}


// Function checks to see if the current RFID card matches one that the arduino knows about
void checkPlayer(String code){
  Serial.println("Checking ... " + code);
    
  // Loop through all cards and check if the ID is in the master array (above)
  // if the card is in the array, send it to the server
  for (int i=0; i<sizeof(playerIds); i++) {
     if (code == playerIds[i]) {
       sendToServer(playerNames[i]);
       break; 
     } else {
       continue;
     }
  }
}  

// Function to send station ID and player ID to the server
void sendToServer(int playerNumber){ 
  int playerId = playerNumber;
  Serial.println("Attempting to send player " + String(playerId) + " at station " + String(stationId));
  
  // instantiate a new Process to communicate the the Arduino's linux environment
  Process p;
  
  p.begin("curl");
  // replace the IP address below with another server's IP
  p.addParameter("http://104.131.39.242:3000/station/" + String(stationId) + "/player/" + String(playerId));
  p.run();
 
  // read anything that comes back from linux (ie. server response)
  while (p.available()>0) {
    char c = p.read();
    Serial.print(c);
  }
  
}
