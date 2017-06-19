
#ifndef FVlib_Arduino_SIM900_Ubidots_Client_h
  #define FVlib_Arduino_SIM900_Ubidots_Client_h

  #include <Arduino.h>
  #include <SoftwareSerial.h>
  #include <avr/pgmspace.h>

  // En aquest define li hem de dir quina placa Arduino utilitzem
    #define placaArduino 1 // 1:UNO 2:DUE/MEGA

  #if placaArduino == 1
    #pragma message ( "Deffined board: Arduino UNO: SoftwareSerial(7, 8)" )
  #elif placaArduino == 2
    #pragma message ("Deffined board: Arduino DUE")
  #else
    #error Has de definir la placa arduino amb la variable de preprocessador placaArduino
  #endif


  #define USER_AGENT  "Ubidots/v1 ArduinoGPRS/1.0"

  class sim900 {
    public:

      /** Constructor
       *  @param token: the ubidots token of your acount
       */
      sim900(char* token);

      /** Powers up the modem and initalize the SIM900 UART
       *  @return true if done, false some error ocurred
       */
      boolean begin();

      /** Connects the modem to internet. Needed if we want to send data to Ubidots
       *  @return true if done, false some error ocurred
       */
      byte setModemOnline();

      /** Sends data to Ubidots IoT Platform
       *  @param:
       *    value: the value to save on the platform
       *    id: the ID of the variable
       *    printVerbose: print the server response
       */
      byte ubidotsSendData(double value, char* id, boolean printVerbose); // Save data

      /** Sets the SIM900 ready to operate with SMS
       *  @return true if done, false some error ocurred
       */
      boolean sim900::startSmsModule();

      /** Checks how many SMS are waiting to be read
       *  @return 255: error, else: available SMSs to read
       */
      byte sim900::availableSMSs();

      /** Read a SMS
       *
       */
      String sim900::readSMS();

      /** Get the signal strength from the SIM900
       *  @return: the strength
       *  @param format:
       *    1: Returns the strenght in the same format as read from the AT commmand
       *    2: Returns the strenght in 0 to 5 scale. 0: No network, 5: Very strong signal
       *    3: Retruns the strenght in dB
       */
      int getSignalStrengthRSSI(byte format);

      /** Get the Bit Error Ratio from the SIM900
       *  @return:
       *    0: Excellent
       *    7: High BER (this is bad!)
       */
      byte getSignalStrengthBER();


    private:
      #if placaArduino == 1
        SoftwareSerial _portSim900 = SoftwareSerial(7, 8);
      #elif placaArduino == 2
        #define _portSim900 Serial1
      #endif

      char* _token;
      char sim900Buffer[500]; // Where we save the SIM900 read buffer

      /** Powers up the module using pin 9. You need to have jumper soldered for software power.
       *  @return: true if Ok, false if error
       */
      boolean powerUpModem();

      /** Initializes de SIM900 UART
       *  @return: true if Ok, false if error
       */
      boolean initializeSIM900UART();

      /** Calls the SAPBR AT Command, needed to setup an internet connection
       *  @return: 0 if Ok, else: error
       */
      byte ubidotsSAPBR();

      /** Lookup for a string inside the
       *  @return: true if found, false if not
       *  @param:
       *    printFound: if true it prints via Serial "Found" or "Not Found"
       */
      boolean findString(boolean printFound, String findWhat);

      /** Terminates a HTTP connection
       *  @return: 0 if Ok, else: error
       */
      boolean ubidotsHttpTerm();

      /** Initializes a HTTP connection
       *  @return: 0 if Ok, else: error
       */
      boolean ubidotsHttpInit(boolean printVerbose);

      /** Reads the SIM900 UART buffer and fills the sim900Buffer array
       * @param: printReading, if true prints the buffer, else do not print buffer
       * @return: pointer to sim900Buffer array
       */
      char *readSim900Buffer(boolean printReading);

  };
#endif
