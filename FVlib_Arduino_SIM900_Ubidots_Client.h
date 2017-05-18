
#ifndef FVlib_Arduino_SIM900_Ubidots_Client_h
  #define FVlib_Arduino_SIM900_Ubidots_Client_h

  #include <Arduino.h>
  #include <SoftwareSerial.h>
  #include <avr/pgmspace.h>

  // En aquest define li hem de dir quina placa Arduino utilitzem
    #define placaArduino 2 // 1:UNO 2:DUE/MEGA

  #if placaArduino == 1
    #pragma message ( "placa definida: Arduino UNO: SoftwareSerial(7, 8)" )
  #elif placaArduino == 2
    #pragma message ("message placa definida: Arduino DUE")
  #else
    #error Has de definir la placa arduino amb la variable de preprocessador placaArduino
  #endif


  #define USER_AGENT  "Ubidots/v1 ArduinoGPRS/1.0"

  class sim900 {
    private:
      char* _token;
      boolean buscaOK();

      #if placaArduino == 1
        SoftwareSerial _portSim900 = SoftwareSerial(7, 8);
      #elif placaArduino == 2
        #define _portSim900 Serial1
      #endif

      boolean comprovaAtachedPackedDomindService_ATCGATT();
      boolean comprovaSAPBR();
      bool httpTerm();
      bool httpInit();

    public:
      sim900(char* token);


      boolean comprovaCobertura_ATCSQ();
      boolean guardaDada(double value, char* id);


      boolean inicialitzaPortSerieSim900();


      void configuraAPN();

      char *llegirSim900();
      char bufferResposta[500];

      void nivellCobertura();
      byte cobertura1;
  	byte cobertura2;

      boolean setModemOnline();


  };

#endif
