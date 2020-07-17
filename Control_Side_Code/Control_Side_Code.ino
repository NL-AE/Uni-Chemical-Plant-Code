
  // Libraries
    #include <Wire.h>                 // Wire for I2C
    #include <OneWire.h>              // For temp sensor
    #include <DallasTemperature.h>    // For temp sensor
    #define SOFTPWM_OUTPUT_DELAY      // Soft output for software PWM
    #include <PalatisSoftPWM.h>       // For Software PWM

  // Setup sensor
    #define ONE_WIRE_BUS 11               // Attach sensor to pin 11
    OneWire oneWire(ONE_WIRE_BUS);
    DallasTemperature sensors(&oneWire);

  // I2C Variables
    char responseChar[5];   // Response String for I2C to display master
    int SetTemp = 150;            // Store what the set temperature is
    
  // Define Pins    
    int pinArray[9] = {3,4,5,6,7,8,9,2,10};   // Pins for:  HtR, RtC, HtC, CtH, RtP, SFL, SFR, MS, EH
    int ActivePins[8] = {0,0,0,0,0,0,0,0};    // Status of: HtR, RtC, HtC, CtH, RtP, SFL, SFR, MS

    SOFTPWM_DEFINE_PIN2_CHANNEL(0);                 // Make pin 2 PWM channel 0
    SOFTPWM_DEFINE_OBJECT_WITH_PWM_LEVELS(1, 10);   // For 1 PWM channels, make them have 10 levels
    

  // Setup --------------------------------------------------------------
    void setup() {
      Serial.begin(115200);   // Serial begin
      sensors.begin();        // Setup temperature sensor

      PalatisSoftPWM.begin(60);  // begin with 60 Hz PWM frequency
  
      for(int i=0; i<9; i++){          // For loop (9 times)
        pinMode(pinArray[i],OUTPUT);      // Initialise pin as output
        digitalWrite(pinArray[i],LOW);    // Switch pin off
      }

      // I2C
        Wire.begin(9);                  // Begin as I2C slave with address 9
        Wire.onRequest(requestEvent);   // Function when data requested
        Wire.onReceive(receiveEvent);   // Function when data recieved
    }

  // Loop  --------------------------------------------------------------
    void loop() {
      sensors.requestTemperatures();                // Request temps
      int RawTemp = 10*sensors.getTempCByIndex(0);  // Read raw temperature and times it by 10 to make it int
      int HeaterStatus;

      if(RawTemp<SetTemp){
        digitalWrite(pinArray[8],1);                // If raw temp. < set temp. turn heater on
        HeaterStatus = 1;
      }else{
        digitalWrite(pinArray[8],0);                // If raw temp. > set temp. turn heater off
        HeaterStatus = 0;
      }

      for(int i=0; i<7; i++){
        digitalWrite(pinArray[i],ActivePins[i]);    // Write to all pins correct states
      }

      if(ActivePins[7]==1){
        PalatisSoftPWM.set(0, 9);
      }else{
        PalatisSoftPWM.set(0, 0);
      }
      
      String response = String(RawTemp) + String(HeaterStatus);   // Create response string (sending in format of: 4501 which means read temperature is 45.0C and heater is on)
      response.toCharArray(responseChar, 5);                      // Make into char array

      // Printing to Serial
        for(int i=0; i<8; i++){         // For loop 8 times
          Serial.print(ActivePins[i]);    // Print status of all pins
        }
        Serial.print(HeaterStatus);       // Print heater status
        Serial.print(RawTemp);            // Print raw temperature
        Serial.println(SetTemp);          // Print set temperature        
    }
    
  // Recieve Event ------------------------------------------------------
    void receiveEvent(int howMany) {
      int i=0;                          // make i variable
      
      while (1 < Wire.available()) {  // loop through until we get to the last bit
        ActivePins[i] = Wire.read();  // recieve byte and put in array
        i++;                          // increment i variable (so we can assign the bit to the chronological place in the array)
      }
      
      SetTemp = Wire.read()*10.;    // receive final byte as an integer
    }

  // Request Event ------------------------------------------------------
    void requestEvent() {
      Wire.write(responseChar);     // Send previously made char array
    }





  // --------------------------------------------------------------------
