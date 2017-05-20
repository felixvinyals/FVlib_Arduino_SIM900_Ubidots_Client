
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
//unsigned long sim900Watchdog;

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

byte sim900::setModemOnline() { // Turn on the modem
// ------------------------- (3/3) APN -------------------------
  #if SIM900_DEBUG == 1
    Serial.print("Setting APN... ");
  #endif
  if (!apnConfig()) {
    #if SIM900_DEBUG == 1
      Serial.println("ERROR");
    #endif
    return 3;
  }
  else {
    #if SIM900_DEBUG == 1
      Serial.println("Ok!");
    #endif
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
    llegirSim900(); // Try to catch some "NORMAL POWER DOWN" answer
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
    if (buscaOK() == true) {
      return true;
    }
  }

}

boolean sim900::apnConfig() {
    while (getSignalStrengthRSSI(false) == LOW) {
  }

  // Per comprovar el: ATCGATT
  while (comprovaAtachedPackedDomindService_ATCGATT() == LOW) {
    getSignalStrengthRSSI(false);
  }

  while (comprovaSAPBR() == LOW) {

  }
}

int sim900::getSignalStrengthRSSI(byte format) {
  byte cobertura = 0;
  int contador = 0;

  _portSim900.println("AT+CSQ");
  llegirSim900();

  for(contador = 0; contador < 500; contador++) {
    if(bufferResposta[contador] == ',') {
      if((bufferResposta[contador - 2] >= '0') && (bufferResposta[contador - 2] <= '9')) {
        Serial.println("2caracters trobats");
        cobertura = bufferResposta[contador - 2] - '0';
        cobertura = cobertura * 10;
      }
      cobertura = cobertura + (bufferResposta[contador - 1] - '0');
      if (cobertura == 99) return 0;
      break;
    }
    if (contador == 499) return 255;
  }
  if (format == 1) {
    return cobertura;
  }
  else if (format == 2) {
    if ((cobertura >= 2) && (cobertura <= 9)) return 1;
    else if ((cobertura >= 10) && (cobertura <= 14)) return 2;
    else if ((cobertura >= 15) && (cobertura <= 19)) return 3;
    else if ((cobertura >= 20) && (cobertura <= 30)) return 4;
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
  }
  else return cobertura;
}

byte sim900::getSignalStrengthBER() {
  byte cobertura = 0;
  int contador = 0;

  _portSim900.println("AT+CSQ");
  llegirSim900();

  for(contador = 0; contador < 500; contador++) {
    if(bufferResposta[contador] == ',') {
      cobertura = (bufferResposta[contador + 1] - '0');
      if (cobertura == 99) return 0;
      else return cobertura;
    }
    if (contador == 499) return 255;
  }
}

boolean sim900::comprovaAtachedPackedDomindService_ATCGATT() {
  _portSim900.println(F("AT+CGATT?"));
  llegirSim900();
  if (bufferResposta[19] == '0') {
    Serial.print(bufferResposta);
    Serial.println(F("Check the status of Packet service attach. '0' implies device is not attached and '1' implies device is attached."));
    return LOW;
  }
  else return HIGH;
}

boolean sim900::comprovaSAPBR() {
  byte intents = 3;

  Serial.println("AT+SAPBR...");
  _portSim900.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  llegirSim900();
  Serial.println(bufferResposta);

  _portSim900.print(F("AT+SAPBR=3,1,\"APN\",\""));
  _portSim900.print(APN);
  _portSim900.println(F("\""));
  llegirSim900();
  Serial.println(bufferResposta);

  _portSim900.print(F("AT+SAPBR=3,1,\"USER\",\""));
  _portSim900.print(USER);
  _portSim900.println(F("\""));
  llegirSim900();
  Serial.println(bufferResposta);

  _portSim900.print(F("AT+SAPBR=3,1,\"PWD\",\""));
  _portSim900.print(PASS);
  _portSim900.println("\"");
  llegirSim900();
  Serial.println(bufferResposta);

  _portSim900.println(F("AT+SAPBR=1,1"));
  llegirSim900();
  Serial.println(bufferResposta);
  while(buscaOK() == LOW) {
    delay(1500);
    _portSim900.println(F("AT+SAPBR=1,1"));
    llegirSim900();
    Serial.println(bufferResposta);
    if (intents-- == 0) return LOW;
  }

  intents = 3;
  _portSim900.println(F("AT+SAPBR=2,1"));
  llegirSim900();
  Serial.println(bufferResposta);
  while(buscaOK() == LOW) {
    delay(1500);
    _portSim900.println(F("AT+SAPBR=2,1"));
    llegirSim900();
    Serial.println(bufferResposta);
    if (intents-- == 0) return LOW;
  }

  return HIGH;
}
//=============== FI rutina configuraAPN ==========

// Aquesta rutina es perque ens doni el numero de cobertura i l'altre
void sim900::nivellCobertura() {
  _portSim900.println("AT+CSQ");
  llegirSim900();
  Serial.println(bufferResposta);
  cobertura1 = 0;
	cobertura2 = 0;

  char caracter = bufferResposta[14];
  if ((caracter >= '0') && (caracter <= '9')) {
    byte cobertura = caracter - '0';
    cobertura1 = cobertura;
    caracter = bufferResposta[15];
    if ((caracter >= '0') && (caracter <= '9')) {
    	cobertura = (cobertura * 10) + (caracter - '0');
		cobertura1 = cobertura;
	}
	else if (caracter == ',') {
		caracter = bufferResposta[16];
		cobertura = caracter - '0';
		cobertura2 = cobertura;
	}
  }

}

char* sim900::llegirSim900() {
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

boolean sim900::buscaOK() {
  llegirSim900();
  char *p;
  p = strstr (bufferResposta, "OK");
  if (p) {
    //Serial.print("OK Trovat");
    return(true);
  } else {
    //Serial.print("No OK");
    return(false);
  }
}

// =======================================================================================================================================
// =======================================================================================================================================

boolean sim900::guardaDada(double value, char* id) {
  char data[25];
  char val[10];

  // Passem el valor double de value a una cadena representativa de chars a val
  dtostrf(value,7, 3, val);
  httpTerm();
  sprintf(data,"{\"value\":%s}", val);
  Serial.println(data);
  httpInit();
  Serial.print("Token: ");
  Serial.println(_token);
  _portSim900.print(F("AT+HTTPPARA=\"URL\",\"things.ubidots.com/api/v1.6/variables/"));
  _portSim900.print(id);
  _portSim900.print(F("/values?token="));
  _portSim900.print(_token);
  _portSim900.println("\"");
  llegirSim900();
  Serial.println(bufferResposta);

  _portSim900.println(F("AT+HTTPPARA=\"CONTENT\",\"application/json\""));
  llegirSim900();
  Serial.println(bufferResposta);

  _portSim900.print(F("AT+HTTPDATA="));
  _portSim900.print(strlen(data));
  _portSim900.print(F(","));
  _portSim900.println(120000);
  llegirSim900();
  Serial.println(bufferResposta);


  _portSim900.write(data, strlen(data));
  llegirSim900();
  Serial.println(bufferResposta);

  _portSim900.println(F("AT+HTTPACTION=1"));  // HTTPACTION=1 is a POST method
  delay(5000);
  llegirSim900();
  Serial.println(bufferResposta);


  _portSim900.println(F("AT+HTTPREAD"));
  llegirSim900();
  Serial.println(bufferResposta);


  httpTerm();


}




bool sim900::httpInit() {
  _portSim900.println(F("AT+HTTPINIT"));
  llegirSim900();
  Serial.println(bufferResposta);

  _portSim900.println(F("AT+HTTPPARA=\"CID\",1"));
  llegirSim900();
  Serial.println(bufferResposta);

  _portSim900.print(F("AT+HTTPPARA=\"UA\","));
  _portSim900.print(USER_AGENT);
  _portSim900.println(F("\""));
  llegirSim900();
  Serial.println(bufferResposta);
  return false;
}

bool sim900::httpTerm(){
    _portSim900.println(F("AT+HTTPTERM"));
    if(strstr(llegirSim900(),"OK")==NULL){
        Serial.println(F("No HTTP to close"));
        return false;
    }
    return true;
}
