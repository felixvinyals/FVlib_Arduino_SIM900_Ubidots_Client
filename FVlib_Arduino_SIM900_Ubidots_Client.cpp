
#include <SoftwareSerial.h>

#include "FVlib_Arduino_SIM900_Ubidots_Client.h"


#define APN "movistar.es"
#define USER "MOVISTAR"
#define PASS "MOVISTAR"
char bufferResposta[500];
byte cobertura1;
byte cobertura2;

sim900::sim900(char* token) {
  _token = token;
  _portSim900.begin(19200);
}

void sim900::engegaSim900() {
  pinMode(9, OUTPUT);
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(2000);
  digitalWrite(9,LOW);
  delay(3000);
}

void sim900::inicialitzaPortSerieSim900() {
   while(!buscaOK()) {
    Serial.println("Esperant resposta AT...");
    _portSim900.println("AT");
    llegirSim900();
  }
}

//=============== Inici rutina configuraAPN ==========
void sim900::configuraAPN() {
  Serial.println(F("Connectant modem..."));

  while (comprovaCobertura_ATCSQ() == LOW) {
  }

  // Per comprovar el: ATCGATT
  while (comprovaAtachedPackedDomindService_ATCGATT() == LOW) {
    comprovaCobertura_ATCSQ();
  }

  while (comprovaSAPBR() == LOW) {

  }

  Serial.println(F("Modem online"));

}

boolean sim900::comprovaCobertura_ATCSQ() {
  _portSim900.println("AT+CSQ");
  llegirSim900();
  Serial.print(bufferResposta);

  char caracter = bufferResposta[14];
  if ((caracter >= '0') && (caracter <= '9')) {
    byte cobertura = caracter - '0';
    caracter = bufferResposta[15];
    if (caracter != ',') cobertura = (cobertura * 10) + (caracter - '0');

    if (cobertura == 0) {Serial.println(F("No hi ha cobertura")); return LOW;}
    else if ((cobertura > 0) && (cobertura <= 9)) {Serial.println(F("Cobertura 1/4")); return HIGH;}
    else if ((cobertura > 9) && (cobertura <= 14)) {Serial.println(F("Cobertura 2/4")); return HIGH;}
    else if ((cobertura > 14) && (cobertura <= 19)) {Serial.println(F("Cobertura 3/4")); return HIGH;}
    else if ((cobertura > 19) && (cobertura <= 31)) {Serial.println(F("Cobertura 4/4")); return HIGH;}
    else {Serial.println(F("Error de cobertura")); return LOW;}
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

  //Serial.print("\nResposta del Modem: ");
  //Serial.println(bufferResposta);

  while(_portSim900.available()){
    _portSim900.read();
  }
  if(strstr(bufferResposta,"NORMAL POWER DOWN")!=NULL){
    engegaSim900();
  }
  return bufferResposta;
}

boolean sim900::buscaOK() {
  char *p;
  p = strstr (bufferResposta, "OK");
  if (p) {
    //Serial.print("OK Trovat");
    return(HIGH);
  } else {
    //Serial.print("No OK");
    return(LOW);
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
