
#include <SoftwareSerial.h>
#include "FVlib_Arduino_SIM900_Ubidots_Client.h"

// Enable this label if you want to se the SIM900 UART verbose
  #define SIM900_VERBOSE 0
// Enable this define to see debug coments
  #define SIM900_DEBUG 1

#define APN "movistar.es"
#define USER "MOVISTAR"
#define PASS "MOVISTAR"

char bufferResposta[500];

sim900::sim900(char* token) {
  _token = token;
  _portSim900.begin(19200);
}

boolean sim900::begin() {
// ------------------------- (1/3) Powerup -------------------------
  #if SIM900_DEBUG == 1
    Serial.print("\n\nStarting SIM900: \n(1/2) Powering up... ");
  #endif
  if (!powerUpModem()) {
    #if SIM900_DEBUG == 1
      Serial.println("ERROR");
    #endif
    return 1;
  }
  else {
    #if SIM900_DEBUG == 1
      Serial.println("Ok!");
    #endif
  }

// ------------------------- (2/3) UART -------------------------
  #if SIM900_DEBUG == 1
    Serial.print("(2/2) Initializing SIM900 UART... ");
  #endif
  if (initializeSIM900UART() == false) {
    #if SIM900_DEBUG == 1
      Serial.println("ERROR");
    #endif
    return 2;
  }
  else {
    #if SIM900_DEBUG == 1
      Serial.println("Ok!");
    #endif
    return 0;
  }
}

byte sim900::setModemOnline() {
  byte whileAttempts = 20;
  byte signalStrength = 0;

  #if SIM900_DEBUG == 1
    Serial.println("Setting APN:");
  #endif

  // ----------------- (1/3) ---------------------
  // First check if there is enought netwotk strength:
  while (whileAttempts--) {
    signalStrength = getSignalStrengthRSSI(2);
    #if SIM900_DEBUG == 1
      Serial.print("(1/3) Checking signal signal Strength... ");
      Serial.print(signalStrength);
      Serial.print("/5 ");
      if (signalStrength == 0) Serial.println("ERROR");
    #endif
    if (signalStrength != 0) break;
    if (whileAttempts == 0) return 1;
  }
  #if SIM900_DEBUG == 1
    Serial.println("Ok!");
  #endif
  whileAttempts = 20;


  // ----------------- (2/3) ---------------------
  // Attach to the domine packet service
  while (whileAttempts--) {
    _portSim900.println(F("AT+CGATT?"));
    llegirSim900(false);
    #if SIM900_DEBUG == 1
      Serial.print("(2/3) Attaching device to packet domine service... ");
    #endif
    if (bufferResposta[19] == '1') {
      #if SIM900_DEBUG == 1
        Serial.println("Ok!");
      #endif
      break;
    }
    else {
      #if SIM900_DEBUG == 1
        Serial.println("ERROR");
      #endif
    }
    if (whileAttempts == 0) return 2;
  }
  whileAttempts = 20;

  #if SIM900_DEBUG == 1
    Serial.println("(3/3) Configuring SAPBR:");
  #endif

  byte SAPBRResult = 255;
  SAPBRResult = SAPBR();
  if(SAPBRResult == 0) {
    #if SIM900_DEBUG == 1
      Serial.println("SIM900 online!");
    #endif
    return SAPBRResult;
  }
  else {
    #if SIM900_DEBUG == 1
      Serial.println("Failed to set the SIM900 online.");
    #endif
    return SAPBRResult;
  }
}

boolean sim900::powerUpModem() {
  for (byte contador = 1; contador <= 3; contador++) {
    pinMode(9, OUTPUT);
    digitalWrite(9,LOW);
    delay(1000);
    digitalWrite(9,HIGH);
    delay(2000);
    digitalWrite(9,LOW);
    delay(3000);
    llegirSim900(false); // Try to catch some "NORMAL POWER DOWN" answer
    if ((bufferResposta[0] == 'N') && (bufferResposta[1] == 'P') &&  (bufferResposta[2] == 'D') && (bufferResposta[3] == '\0')) {
      if (contador == 3) {
        return false;
      }
    }
    else {
      return true;
    }
  }
}

boolean sim900::initializeSIM900UART() { // Whait for SIM900 UART to be ready
  for (byte contador = 0; contador <= 10; contador++) {
    _portSim900.println("AT");
    llegirSim900(false);
    if (buscaOk(false, "OK") == true) {
      return true;
    }
  }
}

int sim900::getSignalStrengthRSSI(byte format) {
  byte cobertura = 0;
  int contador = 0;

  _portSim900.println("AT+CSQ");
  llegirSim900(false);

  for(contador = 0; contador < 500; contador++) {
    if(bufferResposta[contador] == ',') {
      if((bufferResposta[contador - 2] >= '0') && (bufferResposta[contador - 2] <= '9')) {
        cobertura = bufferResposta[contador - 2] - '0';
        cobertura = cobertura * 10;
      }
      cobertura = cobertura + (bufferResposta[contador - 1] - '0');
      if (cobertura == 99) return 0;
      break;
    }
    if (contador == 499) return 0;
  }
  if (format == 1) {
    return cobertura;
  }
  else if (format == 2) {
    if ((cobertura >= 2) && (cobertura <= 9)) return 1;
    else if ((cobertura >= 10) && (cobertura <= 14)) return 2;
    else if ((cobertura >= 15) && (cobertura <= 19)) return 3;
    else if ((cobertura >= 20) && (cobertura <= 30)) return 4;
    else return 0;
  }
  else if (format == 3) {
    if (cobertura == 2) return -109;
    else if (cobertura == 3) return -107;
    else if (cobertura == 4) return -105;
    else if (cobertura == 5) return -103;
    else if (cobertura == 6) return -101;
    else if (cobertura == 7) return -99;
    else if (cobertura == 8) return -97;
    else if (cobertura == 9) return -95;
    else if (cobertura == 10) return -93;
    else if (cobertura == 11) return -91;
    else if (cobertura == 12) return -89;
    else if (cobertura == 13) return -87;
    else if (cobertura == 14) return -85;
    else if (cobertura == 15) return -83;
    else if (cobertura == 16) return -81;
    else if (cobertura == 17) return -79;
    else if (cobertura == 18) return -77;
    else if (cobertura == 19) return -75;
    else if (cobertura == 20) return -73;
    else if (cobertura == 21) return -71;
    else if (cobertura == 22) return -69;
    else if (cobertura == 23) return -67;
    else if (cobertura == 24) return -65;
    else if (cobertura == 25) return -63;
    else if (cobertura == 26) return -61;
    else if (cobertura == 27) return -59;
    else if (cobertura == 28) return -57;
    else if (cobertura == 29) return -55;
    else if (cobertura == 30) return -53;
    else return 0;
  }
  else return cobertura;
}

byte sim900::getSignalStrengthBER() {
  byte cobertura = 0;
  int contador = 0;

  _portSim900.println("AT+CSQ");
  llegirSim900(false);
  for(contador = 0; contador < 500; contador++) {
    if(bufferResposta[contador] == ',') {
      cobertura = (bufferResposta[contador + 1] - '0');
      if (cobertura == 99) return 0;
      else return cobertura;
    }
    if (contador == 499) return 255;
  }
}



byte sim900::SAPBR() {
  byte whileAttempts = 10;

  while(whileAttempts--) {
    _portSim900.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
    llegirSim900(false);
    #if SIM900_DEBUG == 1
      Serial.println(bufferResposta);
    #endif
    if (buscaOk(false, "OK")) break;
    if (whileAttempts == 0) return 3;
  }
  whileAttempts = 10;

  while(whileAttempts--) {
    _portSim900.print(F("AT+SAPBR=3,1,\"APN\",\""));
    _portSim900.print(APN);
    _portSim900.println(F("\""));
    llegirSim900(false);
    #if SIM900_DEBUG == 1
      Serial.println(bufferResposta);
    #endif
    if (buscaOk(false, "OK")) break;
    if (whileAttempts == 0) return 4;
  }
  whileAttempts = 10;

  while(whileAttempts--) {
    _portSim900.print(F("AT+SAPBR=3,1,\"USER\",\""));
    _portSim900.print(USER);
    _portSim900.println(F("\""));
    llegirSim900(false);
    #if SIM900_DEBUG == 1
      Serial.println(bufferResposta);
    #endif
    if (buscaOk(false, "OK")) break;
    if (whileAttempts == 0) return 5;
  }
  whileAttempts = 10;

  while(whileAttempts--) {
    _portSim900.print(F("AT+SAPBR=3,1,\"PWD\",\""));
    _portSim900.print(PASS);
    _portSim900.println("\"");
    llegirSim900(false);
    #if SIM900_DEBUG == 1
      Serial.println(bufferResposta);
    #endif
    if (buscaOk(false, "OK")) break;
    if (whileAttempts == 0) return 6;
  }
  whileAttempts = 10;

  while(whileAttempts--) {
    _portSim900.println(F("AT+SAPBR=1,1"));
    llegirSim900(false);
    #if SIM900_DEBUG == 1
      Serial.println(bufferResposta);
    #endif
    if (buscaOk(false, "OK")) break;
    if (whileAttempts == 0) return 7;
  }
  whileAttempts = 10;

  while(whileAttempts--) {
    _portSim900.println(F("AT+SAPBR=2,1"));
    llegirSim900(false);
    #if SIM900_DEBUG == 1
      Serial.println(bufferResposta);
    #endif
    if (buscaOk(false, "OK")) break;
    if (whileAttempts == 0) return 8;
  }

  return 0; // SIM900 is online! Mission accomplished!
}

char* sim900::llegirSim900(boolean printReading) {
  unsigned int idxBufferResposta;
  byte timeout = 10; // 10 iteracions de 500ms d'espera

  // Netegem tot el buffer de rebuda de dades
  for(idxBufferResposta = 0; idxBufferResposta < 500; idxBufferResposta++) bufferResposta[idxBufferResposta] = '\0';

  idxBufferResposta = 0;
  while ((idxBufferResposta == 0) && (timeout != 0))
  {
    while(_portSim900.available()) {
      char c =  _portSim900.read();
      if (c == 10) c = 32;
      if (c == 13) continue;
      bufferResposta[idxBufferResposta] = c;
      idxBufferResposta++;
    }
    delay(500);
    timeout--;
  }

  bufferResposta[idxBufferResposta] = '\0';  // null term

  #if SIM900_VERBOSE == 1
    Serial.print("\nResposta del Modem: ");
    Serial.println(bufferResposta);
  #else
    if (printReading == true) {
      Serial.print("\nResposta del Modem: ");
      Serial.println(bufferResposta);
    }
  #endif

  while (_portSim900.available()) {
    _portSim900.read();
  }

  if (strstr(bufferResposta, "NORMAL POWER DOWN") != NULL) {
    bufferResposta[0] = 'N';
    bufferResposta[1] = 'P';
    bufferResposta[2] = 'D';
    bufferResposta[3] = '\0';
  }
  return bufferResposta;
}

boolean sim900::buscaOk(boolean printFound, String findWhat) {
  char *p;
  p = strstr (bufferResposta, findWhat.c_str()); //
  if (p) {
    if (printFound == true) Serial.print("OK Found!");
    return(true);
  }
  else {
    if (printFound == true) Serial.print("No OK");
    return(false);
  }
}

// =======================================================================================================================================
// =======================================================================================================================================

byte sim900::sendData(double value, char* id, boolean printVerbose) {
  char data[25];
  char val[10];
  byte httpInitResult = 255;

  #if SIM900_DEBUG == 1
    Serial.print("Posting ");
    Serial.print(value);
    Serial.print(" at ");
    Serial.print(id);
    Serial.print("... ");
  #endif

  dtostrf(value,7, 3, val); // Double to char array at val
  httpTerm();
  sprintf(data,"{\"value\":%s}", val);
  httpInitResult = httpInit(printVerbose);
  if(httpInitResult != 0) return httpInitResult;

  _portSim900.print(F("AT+HTTPPARA=\"URL\",\"things.ubidots.com/api/v1.6/variables/"));
  _portSim900.print(id);
  _portSim900.print(F("/values?token="));
  _portSim900.print(_token);
  _portSim900.println("\"");
  llegirSim900(false);
  if (printVerbose == true) Serial.println(bufferResposta);

  _portSim900.println(F("AT+HTTPPARA=\"CONTENT\",\"application/json\"")); // ha de donar OK
  llegirSim900(false);
  if (printVerbose == true) Serial.println(bufferResposta);
  if (!buscaOk(false, "OK")) return 4;

  _portSim900.print(F("AT+HTTPDATA=")); // Ha de donar DOWNLOAD per enviar la info
  _portSim900.print(strlen(data));
  _portSim900.print(F(","));
  _portSim900.println(120000);
  llegirSim900(false);
  if (printVerbose == true) Serial.println(bufferResposta);
  if (!buscaOk(false, "DOWNLOAD")) return 5;


  _portSim900.write(data, strlen(data));
  llegirSim900(false);
  if (printVerbose == true) Serial.println(bufferResposta);

  _portSim900.println(F("AT+HTTPACTION=1"));  // HTTPACTION=1 is a POST method, ha de donar OK
  delay(5000);
  llegirSim900(false);
  if (printVerbose == true) Serial.println(bufferResposta);
  if (!buscaOk(false, "OK")) return 6;


  _portSim900.println(F("AT+HTTPREAD")); //ha de donar OK
  if (printVerbose == true) Serial.println(bufferResposta);
  if (!buscaOk(false, "OK")) return 7;
  httpTerm();
}

boolean sim900::httpInit(boolean printVerbose) {
  _portSim900.println(F("AT+HTTPINIT")); // Ha de donar OK per validar dada enviada
  llegirSim900(false);
  if (printVerbose == true) Serial.println(bufferResposta);
  if (!buscaOk(false, "OK")) return 1;

  _portSim900.println(F("AT+HTTPPARA=\"CID\",1")); //OK
  llegirSim900(false);
  if (printVerbose == true) Serial.println(bufferResposta);
  if (!buscaOk(false, "OK")) return 2;

  _portSim900.print(F("AT+HTTPPARA=\"UA\",")); //OK
  _portSim900.print(USER_AGENT);
  _portSim900.println(F("\""));
  llegirSim900(false);
  if (printVerbose == true) Serial.println(bufferResposta);
  if (!buscaOk(false, "OK")) return 3;
  else return 0;
}

boolean sim900::httpTerm(){
  _portSim900.println(F("AT+HTTPTERM"));
  if(strstr(llegirSim900(false),"OK")==NULL){
      #if SIM900_DEBUG == 1
        // Serial.println(F("No HTTP to close"));
      #endif
      return false;
  }
  return true;
}
