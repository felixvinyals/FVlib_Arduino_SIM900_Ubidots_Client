
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
      boolean begin(); // First to run
      byte setModemOnline();
      int getSignalStrengthRSSI(byte format);
      byte getSignalStrengthBER();
      byte sendData(double value, char* id, boolean printVerbose); // Save data

    private:
      #if placaArduino == 1
        SoftwareSerial _portSim900 = SoftwareSerial(7, 8);
      #elif placaArduino == 2
        #define _portSim900 Serial1
      #endif

      char* _token;
      char bufferResposta[500];

      boolean powerUpModem();
      boolean initializeSIM900UART();
      byte SAPBR();
      boolean buscaOk(boolean printFound, String findWhat);
      boolean httpTerm();
      boolean httpInit(boolean printVerbose);
      char *llegirSim900(boolean printReading);

  };
#endif
