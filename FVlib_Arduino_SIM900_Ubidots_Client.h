
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
    public:
      sim900(char* token); // Constructor
      boolean begin();
      byte setModemOnline(); // First to run
      int getSignalStrengthRSSI(byte format);
      byte getSignalStrengthBER();
      boolean guardaDada(double value, char* id); // Save data

    private:
      char* _token;
      #if placaArduino == 1
        SoftwareSerial _portSim900 = SoftwareSerial(7, 8);
      #elif placaArduino == 2
        #define _portSim900 Serial1
      #endif

      boolean powerUpModem();
      boolean initializeSIM900UART();

      boolean apnConfig();
      boolean comprovaAtachedPackedDomindService_ATCGATT();
      boolean comprovaSAPBR();
      boolean buscaOK();

      bool httpTerm();
      bool httpInit();



      char *llegirSim900();
      char bufferResposta[500];

      void nivellCobertura();
      byte cobertura1;
  	byte cobertura2;








  };

#endif
