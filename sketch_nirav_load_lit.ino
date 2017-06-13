
      
      
      //RMH change to report to RPi when each 1 Liter has flown (when 5600 pulses were accumulated).
      //RMH Do not allow pulses to be missed, or total to overflow!

      #define PULSES_PER_LITRE      187  //RMH #pulses produced by the flowmeter for each litre of water.
                                                        //RMH this number comes from Carriots RPi code.

      int flowPin = 2;    //flowmeter pin number
      int pulsesCounter = 0;
      int pulses = 0;
      int totalLitres=0;      //RMH this will keep the acumulated flow, but overflows at 32767!

      //Interrupt Service Routine for interrupt 0 (ISR0)
      // ISR0 services an interrupt condition on Pin 2 - whenever voltage on that pin rises.
      void rpm ()    
      {
      pulsesCounter++;            //RMH every RISING pulse causes pulsesCounter to increase by one.
      }

      void setup()
      {
      pinMode(flowPin, INPUT); // Initialize the digital pin as an input
      Serial.begin(9600); // Start serial port
      attachInterrupt(0, rpm, RISING); //Interrupt is attached

      sei();    //RMH enable interrupt 0 on Pin 2 for a RISING signal.
                    // This causes ISR "rpm()" to be called and pulsesCounter to increment when the flowmeter pulses.
                    // note: 5600 pulses/litre, so if water flows at 1 gpm, this happens >350 times per second!
      }

      void loop ()
      {
        cli();                                    //RMH BRIEFLY disable interrupts
        pulses = pulsesCounter;     //RMH copy the ISR variable. Don't corrupt the counting process
        sei();
      //RMH see if it is time to report flow. In this code, reported accumulated litres
      //RMH whenever 1 new litre has passed (5600 pulses from the flowmeter)
        if(pulses >= PULSES_PER_LITRE)
        {
              totalLitres++;                      //RMH local totalizer if needed. Watch for overflow of the UINT!
              Serial.print("Litres:");      //RMH this goes to the RPi as a serial stream.
                                                        //RMH Use the Arduino to reduce raw pulses to Litres
              Serial.println (totalLitres, DEC); //RMH Sends the accumulated litres to RPi


              cli();                                    //RMH Don't corrupt the ISR counter!
              pulsesCounter -= PULSES_PER_LITRE;  //RMH back out the reported 1 litre accumulation from the counter
              sei();

              //RMH If desired, reset totalLitres at a controlled position rather than integer overflow.
              totalLitres %= 10000;         //RMH modulo operation. 0-9999 unchanged; 10000 changes to 0.
        }          
      }
    
