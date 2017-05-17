#include <SoftwareSerial.h> 
#include "felix-sim900_ubidots.h"

// Defines referents al proveidor de serveis GSM/GPRS:
  #define APN "movistar.es"
  #define USER "MOVISTAR" 
  #define PASS "MOVISTAR" 

// Defines referents a Ubidots:
  #define TOKEN "ySh5mOQvxuoMYtXhS2ELVJ1BniTuPW"  // Remplace it with your token
  #define ID "58f72a0676254254ab8dfc1a" // Remplace it with your variable ID

sim900 modem1(TOKEN);

int conta = 0;

void setup() {
  
  Serial.begin(9600);

  Serial.println("Engegant Modem i Port Serie del Modem...");
  modem1.engegaSim900();
  modem1.inicialitzaPortSerieSim900();
  Serial.println("Modem i Port Serie engegat");

  modem1.configuraAPN();
  
}

//=================================================
//============ INICI LOOP =========================
//=================================================

void loop() {
  
  Serial.println("\n\n\n\nInici Loop...");
  modem1.guardaDada(conta++,ID);
  
  delay(1000);
}

//=================================================
//============ FI LOOP ============================
//=================================================


