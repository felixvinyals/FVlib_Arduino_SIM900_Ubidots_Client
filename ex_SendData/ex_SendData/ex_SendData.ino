#include "FVlib_Arduino_SIM900_Ubidots_Client.h"

sim900 modem("ySh5mOQvxuoMYtXhS2ELVJ1BniTuPW");

int batec = 0;

void setup() {
  Serial.begin(9600);
  //modem.begin();

  //Serial.println("Setting modem online...");
  modem.setModemOnline();
  Serial.println("------------ END Setting Modem Online");
}

void loop() {
  Serial.print("Post Result: ");
  Serial.println(modem.sendData(batec++, "58fb4a197625420d812ab61b", false));

}
