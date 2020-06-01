 
  // Libraries
    #include <Adafruit_GFX.h>     // Adafruit GFX library
    #include <TouchScreen.h>      // Touchscreen
    #include <Adafruit_TFTLCD.h>  // Adafruit TFT
    #include <Wire.h> 
    
  // Display Declecation
    Adafruit_TFTLCD tft(A3, A2, A1, A0, A4);            // Pins for TFT 
    TouchScreen ts = TouchScreen(8, A3, A2, 9, 300);    // Pins for Touchscreen

    #define TimeToDeselect 3000     // Miliseconds for when it deselects
    long TimeWhenTouched;
    int SetTemp = 15.0;

  //Colours
    #define W  0xF79E  // White
    #define B  0x001E  // Blue
    #define R  0xF000  // Red
    #define G  0x26A0  // Green
    #define GR 0x94B2  // Grey
    #define N  0x0000  // Black

  // Misc Variables
    long ColourState[8]  = {0,0,0,0,0,0,0,0};   // Stores the colour state of all clickable things

  // Setup --------------------------------------------------------------
    void setup() {
      Serial.begin(115200);   // Start serial       
      Wire.begin();           // Begin as master
      SetupDisplay();         // What do you think it does dumbass (Setup display and also draws everything for first time)
    }

  // Loop ---------------------------------------------------------------
    void loop() {

      // Get touchpoint
        TSPoint p = ts.getPoint();    
        pinMode(A2, OUTPUT);
        pinMode(A3, OUTPUT);

      // If user touches screen
        if(p.z > 10){
          int XPos = map(p.y, 80,890,320,  0);  // Map x
          int YPos = map(p.x,110,910,  0,240);  // Map y
          
          tft.setTextColor(W,N);  tft.setTextSize(1);  tft.setCursor(20,187);  
  
          // Find where the user selected - im only going to comment one of these since the only thing that changes between them is the co-ordinates and the array number
            if( XPos> 13 && XPos< 55 && YPos> 57 && YPos< 99){  // H2O -> Reactor pump
              
              tft.print(F("H2O -> Reactor Pump ")); // Print what thing you selected
              TimeWhenTouched = millis();
              
                ColourState[0]++;           // Increment touch counter                
                if(ColourState[0]%2 == 1){  // Check if divisible by two
                  DrawHtR(G);                 // No, change it to green
                }else{
                  DrawHtR(R);                 // Yes, change it back to red
                }              
            }
            if( XPos>141 && XPos<183 && YPos> 34 && YPos< 76){  // Reactor -> CO2 pump
              tft.print(F("Reactor -> CO2 Pump "));
              TimeWhenTouched = millis();
                ColourState[1]++;
                if(ColourState[1]%2 == 1){
                  DrawRtC(G);
                }else{
                  DrawRtC(R);
                }              
            }
            if( XPos>233 && XPos<275 && YPos> 14 && YPos< 56){  // H2O -> CO2 pump
              tft.print(F("H2O -> CO2 Pump     "));
              TimeWhenTouched = millis();
                ColourState[2]++;
                if(ColourState[2]%2 == 1){
                  DrawHtC(G);
                }else{
                  DrawHtC(R);
                }
            }
            if( XPos>233 && XPos<275 && YPos> 54 && YPos< 96){  // CO2 -> H2O pump
              tft.print(F("CO2 -> H2O Pump     "));
              TimeWhenTouched = millis();
                ColourState[3]++;
                if(ColourState[3]%2 == 1){
                  DrawCtH(G);
                }else{
                  DrawCtH(R);
                }
            }
            if( XPos>216 && XPos<258 && YPos>125 && YPos<167){  // Reactor -> Products
              tft.print(F("React. -> Prod. Pump"));
              TimeWhenTouched = millis();
                ColourState[4]++;
                if(ColourState[4]%2 == 1){
                  DrawRtP(G);
                }else{
                  DrawRtP(R);
                }
            }
            if( XPos> 30 && XPos< 98 && YPos>  7 && YPos< 36){  // L Screw Feed
              tft.print(F("L Screw Feed        "));
              TimeWhenTouched = millis();
                ColourState[5]++;
                if(ColourState[5]%2 == 1){
                  DrawSFL(G);
                }else{
                  DrawSFL(R);
                }
            }
            if( XPos> 99 && XPos<166 && YPos>  7 && YPos< 36){  // R Screw Feed
              tft.print(F("R Screw Feed        "));
              TimeWhenTouched = millis();
                ColourState[6]++;
                if(ColourState[6]%2 == 1){
                  DrawSFR(G);
                }else{
                  DrawSFR(R);
                }
            }
            if( XPos> 69 && XPos<139 && YPos>163 && YPos<192){  // Mag Stirrer
              tft.print(F("Magnetic Stirrer    "));
              TimeWhenTouched = millis();
                ColourState[7]++;
                if(ColourState[7]%2 == 1){
                  DrawMS(G);
                }else{
                  DrawMS(R);
                }
            }

            // This one draws a draggable slider
            if( XPos> 17 && XPos<222 && YPos>205 && YPos<230){  // Set temperature
              tft.print(F("Changing Set Temp   "));
              TimeWhenTouched = millis();

              // Draw Slider
                tft.fillRoundRect( 18,206,192, 23, 3, N);                 // Black Background
                int sliderX = constrain(map(p.y, 80,890,320,0),19,195);   // Constrain and map touch X co-ord
                tft.fillRoundRect(sliderX,206, 15, 23, 3, GR);            // Move Slider

              // Set Temperature
                SetTemp = map(sliderX,19,195,15,60);          // Map slider position to set temp between 15C and 60C
                tft.setTextColor(W,N);  tft.setTextSize(2);
                tft.setCursor(69,90); tft.print(SetTemp);     // Print the set temperature
            }

            // Now Send I2C Data
              Wire.beginTransmission(9);        // Send to slave with address 9
              for(int i=0; i<8; i++){
                Wire.write(ColourState[i]%2);   // Send the colour of all the selectables by accessing the precious array
              }
              Wire.write(SetTemp);              // Send the set temperature
              Wire.endTransmission();           // End transmission
        }

      // Deselect
        if((millis()-TimeWhenTouched) > TimeToDeselect){    // Deselect after number of seconds
          tft.setTextColor(W,N);  tft.setTextSize(1);
          tft.setCursor(20,187);  tft.print(F("Nothing Selected    "));
          }

      // I2C Request
        Wire.requestFrom(9,4);    // Request 3 bytes from slave with address 9
  
        String InResponse = "";       // String for entire response
        String RawTempStr = "";       // String for raw temp
        String HeaterStatChar = "";   // String for heater status
        
        while (Wire.available()) {    // When data avaliable, read data
            char b = Wire.read();
            InResponse += b;
        } 
        
        RawTempStr = InResponse.substring(0,3);     // Split string to isolate the raw temperature reading
        HeaterStatChar = InResponse.substring(3);   // Split string to isolate the heater status
        
        float RawTemp = RawTempStr.toInt()/10.;     // Convert to float (since the data we got it in was something like 456 which is 45.6C)
        int HeaterStat = HeaterStatChar.toInt();    // Convert to int
      
      // Draw Temperature Displays
        tft.setTextColor(W,N);  tft.setTextSize(2);
        tft.setCursor(69,120);  tft.print(RawTemp,1);     // Print raw temperature
        if(HeaterStat == 1){
          DrawEH(G);          // If heater is on, make it green
        }else{
          DrawEH(R);          // If heater is off, make it red (I know this is counter-intuitive but got to remain consistent)
        }
    }

  // Draw Functions - dont even bother looking down here ----------------
    void SetupDisplay(){   // Setup
      // Commands
        tft.reset();          // Initialise Display
        tft.begin(0x9341);    // Call display driver code
        tft.setRotation(1);   // Correct Rotation
        tft.fillScreen(N);    // Clear entire screen   

      // Text
        tft.setCursor(220,182); tft.setTextColor(W);  // Key
        tft.setTextSize(2);     tft.print(F("Key:"));
        tft.setCursor(220,200); tft.setTextColor(G);   // Active
        tft.setTextSize(2);     tft.print(F("Active"));
        tft.setCursor(218,218); tft.setTextColor(R);   // Inactive
        tft.setTextSize(2);     tft.print(F("Inactive"));
      
      // Labels
        tft.setTextColor(W);   tft.setTextSize(1);
        tft.setCursor( 25,128); tft.print(F("H2O"));      // H2O column R
        tft.setCursor( 22,138); tft.print(F("Col."));
        tft.setCursor( 31,148); tft.print(F("R"));
        tft.setCursor(289, 39); tft.print(F("H2O"));      // H2O column L
        tft.setCursor(287, 49); tft.print(F("Col."));
        tft.setCursor(296, 59); tft.print(F("L"));
        tft.setCursor(269,130); tft.print(F("Product"));  // Product column
        tft.setCursor(272,140); tft.print(F("Column"));
        tft.setCursor( 67, 53); tft.print(F("H2O"));      // H2O in
        tft.setCursor(113, 53); tft.print(F("CO2"));      // CO2 out
        tft.setCursor(102,141); tft.print(F("Prod."));    // Prod. out
        tft.setCursor( 78, 65); tft.print(F("Reactor"));  // Reactor
        tft.setCursor( 69, 80); tft.print(F("Set:"));     // Set:
        tft.setCursor( 69,110); tft.print(F("Raw:"));     // Raw:        
        tft.setCursor(120, 90); tft.print(F("C"));        // C
        tft.setCursor(120,120); tft.print(F("C"));        // C
        tft.setCursor(20,187);  tft.print(F("Nothing Selected    "));       

      // Set Temperature Scroll Bar
        tft.drawRoundRect( 17,205,194, 25, 2,  W);    // Outer Edges
        tft.fillRoundRect( 18,206, 15, 23, 3, GR);    // Left
        
        tft.setTextColor(W,N);  tft.setTextSize(2);
        tft.setCursor(69,90); tft.print(SetTemp);     // Set T
        tft.setCursor( 93, 90); tft.print(F(".0"));   // C
        
      // Vessels
        tft.drawRoundRect( 17,106, 32, 72, 5, W);  // H2O store 1
        tft.drawRoundRect( 62, 39, 72,123,10, W);  // Main reactor
        tft.drawRect( 62, 49, 72,103, W);      
        tft.drawRoundRect(181, 18, 32, 32,7, W);  // C2O cylinder 1 - Top
        tft.drawRect(181, 22, 32, 24, W);   // Inside rect
        tft.drawLine(181, 22,212, 45, W);   // Down line
        tft.drawLine(181, 45,212, 22, W);   // Up line        
        tft.drawRoundRect(181, 58, 32, 32,7, W);  // C2O cylinder 2 - Bottom
        tft.drawRect(181, 62, 32, 24, W);   // Inside rect
        tft.drawLine(181, 62,212, 85, W);   // Down line
        tft.drawLine(181, 85,212, 62, W);   // Up line        
        tft.drawRoundRect(282, 18, 32, 72, 5, W);  // H2O store 2
        tft.drawRoundRect(266,106, 48, 72, 5, W);  // Product tank

      // Valve
        tft.drawTriangle(173,130,173,138,179,134, W);     // L Top valve
        tft.drawTriangle(185,130,185,138,179,134, W);     // R Top valve
        tft.drawTriangle(173,150,173,158,179,154, W);     // L Bottom valve
        tft.drawTriangle(185,150,185,158,179,154, W);     // R Bottom valve
        tft.drawLine(173,126,185,126, W);   // Top Valve Horizontal
        tft.drawLine(179,134,179,126, W);   // Top Valve Vertical
        tft.drawLine(173,146,185,146, W);   // Bottom valve Horizontal
        tft.drawLine(179,154,179,146, W);   // Bottom valve Vertical
        tft.drawLine(173,134,163,134, W); // Top valve Vertical     L
        tft.drawLine(173,154,163,154, W); // Bottom valve Vertical  L
        tft.drawLine(163,134,163,154, W); // Top Bottom Connector   L
        tft.drawLine(134,144,163,144, W); // Connector to splitter  L

      // Draw Objects for first time
        DrawSFR(R);
        DrawSFL(R);
        DrawMS (R);
        DrawEH (R);
        DrawHtR(R);
        DrawRtC(R);
        DrawHtC(R);
        DrawCtH(R);
        DrawRtP(R);
    }

    void DrawHtR(uint16_t HtR){   // H2O->Reactor
      tft.drawCircle( 33, 77, 16, HtR);                 // H2O -> Reactor pump
      tft.drawCircle( 33, 69,  5, HtR);     // Top circle
      tft.drawCircle( 26, 81,  5, HtR);     // L circle
      tft.drawCircle( 40, 81,  5, HtR);     // R circle
      tft.drawLine( 28, 86, 38, 86, HtR);   // Bottom line
      tft.drawLine( 20, 81, 28, 68, HtR);   // L line
      tft.drawLine( 46, 81, 37, 66, HtR);   // R line        
      tft.drawLine( 20, 93, 46, 93, HtR);   // Bottom line
      tft.drawLine( 20, 93, 22, 90, HtR);   // L line
      tft.drawLine( 46, 93, 44, 90, HtR);   // R line
      tft.drawLine( 33, 54, 61, 54, HtR);               // Horizontal
      tft.drawLine( 33, 54, 33, 60, HtR);               // Vertical out
      tft.drawLine( 33, 96, 33,106, HtR);               // Vertical to
      tft.fillTriangle( 30, 97, 33, 93, 37, 97, HtR);   // Pointer to pump
      tft.fillTriangle( 57, 57, 57, 51, 61, 54, HtR);   // Pointer out pump
    }
    void DrawRtC(uint16_t RtC){   // Reactor->CO2
      tft.drawCircle(161, 54, 16, RtC);               // Reactor -> CO2 pump
      tft.drawCircle(161, 46,  5, RtC );       // Top circle    
      tft.drawCircle(154, 58,  5, RtC );       // L circle    
      tft.drawCircle(168, 58,  5, RtC );       // R circle    
      tft.drawLine(156, 63,166, 63, RtC );     // Bottom line
      tft.drawLine(148, 58,156, 45, RtC );     // L line
      tft.drawLine(174, 58,165, 43, RtC );     // R line
      tft.drawLine(148, 70,174, 70, RtC );     // Bottom line
      tft.drawLine(148, 70,150, 67, RtC );     // L line
      tft.drawLine(174, 70,172, 67, RtC );     // R line
      tft.drawLine(134, 54,144, 54, RtC);               // Horizontal to
      tft.fillTriangle(140, 57,140, 50,144, 53, RtC);   // Pointer to pump
      tft.drawLine(161, 33,161, 37, RtC);             // Vertical up
      tft.drawLine(161, 33,180, 33, RtC);               // Horizontal to top
      tft.fillTriangle(176, 30,176, 36,180, 33, RtC);   // Pointer out pump top      
      tft.drawLine(160, 70,160, 74, RtC);             // Vertical down
      tft.drawLine(160, 74,180, 74, RtC);               // Horizontal to bottom
      tft.fillTriangle(176, 71,176, 77,180, 74, RtC);   // Pointer out pump bottom
    }
    void DrawHtC(uint16_t HtC){   // H2O->CO2
        tft.drawCircle(253, 34, 16, HtC);               // H2O -> CO2 pump
        tft.drawCircle(253, 26,  5, HtC);       // Top circle    
        tft.drawCircle(246, 38,  5, HtC);       // L circle    
        tft.drawCircle(260, 38,  5, HtC);       // R circle    
        tft.drawLine(248, 43,258, 43, HtC);     // Bottom line
        tft.drawLine(240, 38,248, 25, HtC);     // L line
        tft.drawLine(266, 38,257, 23, HtC);     // R line
        tft.drawLine(240, 50,266, 50, HtC);     // Bottom line
        tft.drawLine(240, 50,242, 47, HtC);     // L line
        tft.drawLine(266, 50,264, 47, HtC);     // R line
        tft.drawLine(269, 34,281, 34, HtC);               // Horizontal to
        tft.fillTriangle(269, 34,273, 31,273, 37, HtC); // Pointer to pump
        tft.drawLine(213, 26,230, 26, HtC);               // Horizontal top
        tft.drawLine(213, 66,221, 66, HtC);               // Horizontal bottom left
        tft.drawLine(227, 66,230, 66, HtC);               // Horizontal bottom right
        tft.drawLine(230, 26,230, 66, HtC);               // Vertical
        tft.drawLine(230, 34,237, 34, HtC);               // Horizontal out pump
        tft.fillTriangle(213, 26,217, 23,217, 29, HtC);   // Pointer out pump top
        tft.fillTriangle(213, 66,217, 63,217, 69, HtC);   // Pointer out pump bottom
    }
    void DrawCtH(uint16_t CtH){   // CO2->H2O
      tft.drawCircle(253, 74, 16, CtH);           // CO2 -> H2O pump
      tft.drawCircle(253, 66,  5, CtH);       // Top circle    
      tft.drawCircle(246, 78,  5, CtH);       // L circle    
      tft.drawCircle(260, 78,  5, CtH);       // R circle    
      tft.drawLine(248, 83,258, 83, CtH);     // Bottom line
      tft.drawLine(240, 78,248, 65, CtH);     // L line
      tft.drawLine(266, 78,257, 63, CtH);     // R line
      tft.drawLine(240, 90,266, 90, CtH);     // Bottom line
      tft.drawLine(240, 90,242, 87, CtH);     // L line
      tft.drawLine(266, 90,264, 87, CtH);     // R line
      tft.drawLine(269, 74,281, 74, CtH);               // Horizontal out
      tft.fillTriangle(281, 74,277, 71,277, 77, CtH); // Pointer out pump
      tft.drawLine(213, 41,224, 41, CtH);               // Horizontal top
      tft.drawLine(213, 81,224, 81, CtH);               // Horizontal bottom
      tft.drawLine(224, 41,224, 81, CtH);               // Vertical
      tft.drawLine(224, 74,236, 74, CtH);               // Horizontal to
      tft.fillTriangle(236, 74,232, 71,232, 77, CtH);   // Pointer to pump
    }
    void DrawRtP(uint16_t RtP){   // Reactor->Product
      tft.drawCircle(236,145, 16, RtP);           // Reactor -> Products
      tft.drawCircle(236,137,  5, RtP);       // Top circle    
      tft.drawCircle(229,149,  5, RtP);       // L circle    
      tft.drawCircle(243,149,  5, RtP);       // R circle    
      tft.drawLine(231,154,241,154, RtP);     // Bottom line
      tft.drawLine(223,149,231,136, RtP);     // L line
      tft.drawLine(249,149,240,134, RtP);     // R line
      tft.drawLine(223,161,249,161, RtP);     // Bottom line
      tft.drawLine(223,161,225,158, RtP);     // L line
      tft.drawLine(249,161,247,158, RtP);     // R line        
      tft.drawLine(252,144,265,144, RtP);               // Horizontal out
      tft.fillTriangle(214,141,214,147,218,144, RtP);   // Pointer to pump
      tft.fillTriangle(260,141,260,147,264,144, RtP);   // Pointer out pump        
      tft.drawLine(186,134,195,134, RtP); // Top valve Horizontal     R
      tft.drawLine(186,154,195,154, RtP); // Bottom valve Horizontal  R
      tft.drawLine(195,134,195,154, RtP); // Top Bottom Connector     R  
      tft.drawLine(218,144,195,144, RtP); // Connector to splitter    R 
    }
    void DrawSFL(uint16_t SFL){   // Left Screw Feed
      tft.drawLine( 45, 28, 90, 28, SFL); // Bottom
      tft.drawLine( 53, 21, 90, 21, SFL); // Top
      tft.drawLine( 45, 28, 38, 15, SFL); // Bottom funnel
      tft.drawLine( 53, 21, 55, 15, SFL); // Top funnel      
      tft.drawLine( 90, 28, 90, 21, SFL); // End
      tft.drawLine( 85, 28, 85, 38, SFL); // Down line      
      tft.drawLine( 45, 28, 50, 21, SFL); // Inside line 1
      tft.drawLine( 55, 28, 50, 21, SFL); // Inside line 2
      tft.drawLine( 55, 28, 60, 21, SFL); // Inside line 3
      tft.drawLine( 65, 28, 60, 21, SFL); // Inside line 4
      tft.drawLine( 65, 28, 70, 21, SFL); // Inside line 5
      tft.drawLine( 75, 28, 70, 21, SFL); // Inside line 6
      tft.drawLine( 75, 28, 80, 21, SFL); // Inside line 7
      tft.drawLine( 85, 28, 80, 21, SFL); // Inside line 8
      tft.drawLine( 85, 28, 90, 21, SFL); // Inside line 9
      tft.fillTriangle( 85, 38, 82, 34, 88, 34, SFL); // Pointer to reactor
    }
    void DrawSFR(uint16_t SFR){   // Right Screw Feeder
      tft.drawLine(106, 28,151, 28, SFR); // Bottom
      tft.drawLine(106, 21,143, 21, SFR); // Top
      tft.drawLine(151, 28,158, 15, SFR); // Bottom funnel
      tft.drawLine(143, 21,141, 15, SFR); // Top funnel                
      tft.drawLine(106, 28,106, 21, SFR); // End
      tft.drawLine(111, 28,111, 38, SFR); // Down line      
      tft.drawLine(106, 21,111, 28, SFR); // Inside line 1
      tft.drawLine(116, 21,111, 28, SFR); // Inside line 2
      tft.drawLine(116, 21,121, 28, SFR); // Inside line 3
      tft.drawLine(126, 21,121, 28, SFR); // Inside line 4
      tft.drawLine(126, 21,131, 28, SFR); // Inside line 5
      tft.drawLine(136, 21,131, 28, SFR); // Inside line 6
      tft.drawLine(136, 21,141, 28, SFR); // Inside line 7
      tft.drawLine(146, 21,141, 28, SFR); // Inside line 8
      tft.drawLine(146, 21,151, 28, SFR); // Inside line 9
      tft.fillTriangle(111, 38,108, 34,114, 34, SFR); // Pointer to reactor
    }
    void DrawMS (uint16_t MS ){   // Magnetic Stirrer
      tft.fillRect( 80,168, 7,6, MS);  // L rect
      tft.fillRect( 80,171,37,3, MS);  // R rect
      tft.fillRect(110,168, 7,6, MS);  // Middle rect
      tft.fillRect( 95,172, 9,5, MS);  // Bottom rect
      tft.setCursor(121,168); tft.setTextColor(MS);  // Text
      tft.setTextSize(1);     tft.print(F("MS"));
    }
    void DrawEH (uint16_t EH ){   // Electric Heater
      tft.drawRect(139, 87,  7, 40, EH); //Electric Heater
      tft.drawLine(142, 82,142, 87, EH);   // Vertical line top
      tft.drawLine(142,127,142,132, EH);   // Vertical line bottom
      tft.drawLine(139, 97,145, 97, EH);   // Horizontal 1
      tft.drawLine(139,107,145,107, EH);   // Horizontal 2
      tft.drawLine(139,117,145,117, EH);   // Horizontal 3
    }





  // --------------------------------------------------------------------
