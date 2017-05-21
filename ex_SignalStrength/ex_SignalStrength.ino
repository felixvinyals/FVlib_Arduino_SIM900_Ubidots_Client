#include "FVlib_Arduino_SIM900_Ubidots_Client.h"

sim900 modem("ySh5mOQvxuoMYtXhS2ELVJ1BniTuPW");

void setup() {
  Serial.begin(9600);
  modem.begin();
  
}

void loop() {
  Serial.println("--------------------------------");
  Serial.println("SignalStrength (RSSI):");
  Serial.print("AT Command RAW--> ");
  Serial.println(modem.getSignalStrengthRSSI(1));
  Serial.print("Over 5 (0: No Signal, 5: Excellent)--> ");
  Serial.println(modem.getSignalStrengthRSSI(2));
  Serial.print("In dB--> ");
  Serial.println(modem.getSignalStrengthRSSI(3));

  Serial.println("\nBER (Bit Error Ratio 0:Excellent, 7:Bad): ");
  Serial.println(modem.getSignalStrengthBER());

}