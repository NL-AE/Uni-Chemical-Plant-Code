
  // Libraries
    #include <OneWire.h> 
    #include <DallasTemperature.h>
    #include <Wire.h>

  // Setup sensor
    #define ONE_WIRE_BUS 11  
    OneWire oneWire(ONE_WIRE_BUS);   
    DallasTemperature sensors(&oneWire);

    char responseChar[5];   // Response String for I2C to display master
    int SetTemp;
    
  // Define Pins    
    int pinArray[9] = {3,4,5,6,7,8,9,2,10}; // Pins for:  HtR, RtC, HtC, CtH, RtP, SFL, SFR, MS, EH
    int ActivePins[8] = {0,0,0,0,0,0,0,0};  // Status of: HtR, RtC, HtC, CtH, RtP, SFL, SFR, MS

  // Setup --------------------------------------------------------------
    void setup() {
      Serial.begin(115200);   // Serial begin
      sensors.begin();        // Setup temp sensor
  
      for(int i=0; i<9; i++){          // For loop (9 times)
        pinMode(pinArray[i],OUTPUT);      // Initialise pin as output
        digitalWrite(pinArray[i],LOW);    // Switch pin off
      }

      // I2C
        Wire.begin(9);                  // Begin as slave 9
        Wire.onRequest(requestEvent);   // Function when data requested
        Wire.onReceive(receiveEvent);   // Function when data recieved
    }

  // Loop  --------------------------------------------------------------
    void loop() {
      sensors.requestTemperatures();                // Request temps
      int RawTemp = 10*sensors.getTempCByIndex(0);  // Set raw temp as int
      int HeaterStatus;

      if(RawTemp<=10){
        RawTemp = 10;
      }

      if(RawTemp<SetTemp){
        digitalWrite(pinArray[8],1);
        HeaterStatus = 1;
      }else{
        digitalWrite(pinArray[8],0);
        HeaterStatus = 0;
      }

      for(int i=0; i<8; i++){
        digitalWrite(pinArray[i],ActivePins[i]);
      }
      
      String response = String(RawTemp) + String(HeaterStatus);   // Create response string      
      response.toCharArray(responseChar, 5);                      // Make into char array

      // Printing to Serial
        for(int i=0; i<8; i++){
          Serial.print(ActivePins[i]);
        }
        Serial.print(HeaterStatus);
        Serial.print(RawTemp);
        Serial.println(SetTemp);
        
    }
    
  // Recieve Event
    void receiveEvent(int howMany) {
      int i=0;
      while (1 < Wire.available()) { // loop through all but the last        
        ActivePins[i] = Wire.read(); // recieve byte and put in array
        i++;
      }
      SetTemp = Wire.read()*10.;    // receive byte as an integer
    }

  // Request Event
    void requestEvent() {
      Wire.write(responseChar);
    }


  //#############################################################################################################
