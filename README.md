# FVlib_Arduino_SIM900_Ubidots_Client
Library to communicate an Arduino GSM/GPRS SIM900 shield with Ubidots IoT platform. Based on the Ubidots library.

## Important Considerations ##

SIM must be unlocked

boolean begin()
- Powers up the sim900 and initialitzates the sim900 uart. 
- returns 0: Ok
- returns 1: Powerup fail
- return 2: Bring uart up fail

byte setModemOnline()
- return 0: Ok
- return 1: signal strength error
- return 2: error at Attach to the domine packet service 

byte getSignalStrengthBER()
Returns: BRE (0: no BER (good), 7: 7%BER (bad)


int getSignalStrengthRSSI(byte format)
format: 
- 1: Returns RAW AT netrowk strength
- 2: Returns 0 to 5 netrowk strength
- 3: Returns network strength in dB

byte sendData(double value, char* id, boolean printVerbose);
vale: what you want to post
id: ubidots variable id
printverbose: see what server says
return: 0 if variable has been posted, else error