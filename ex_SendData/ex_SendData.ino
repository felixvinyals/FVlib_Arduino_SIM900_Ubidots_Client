#include "FVlib_Arduino_SIM900_Ubidots_Client.h"

#define ubidotsToken
#define ubidotsVariableID

sim900 modem(ubidotsToken);

int batec = 0;

void setup() {
  Serial.begin(9600);
 
  modem.setModemOnline();
 
}

void loop() {
  Serial.print("Post Result: ");
  Serial.println(modem.sendData(analogRead(0), ubidotsVariableID, false));

}