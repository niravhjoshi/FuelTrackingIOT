#include "TinyGPS++.h"
#include "SoftwareSerial.h"

//RMH change to report to RPi when each 1 Liter has flown (when 5600 pulses were accumulated).
//RMH Do not allow pulses to be missed, or total to overflow!

#define PULSES_PER_LITRE      187  //RMH #pulses produced by the flowmeter for each litre of water.
                                                  //RMH this number comes from Carriots RPi code.

int flowPin = 2;    //flowmeter pin number
int pulsesCounter = 0;
int pulses = 0;
int totalLitres=0;      //RMH this will keep the acumulated flow, but overflows at 32767!

void rpm ()    
      {
      pulsesCounter++;            //RMH every RISING pulse causes pulsesCounter to increase by one.
      }



SoftwareSerial serial_connection(10, 11); //RX=pin 10, TX=pin 11
TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data
void setup()
{ //GPD Module setup instruction begin
  Serial.begin(9600);//This opens up communications to the Serial monitor in the Arduino IDE
  serial_connection.begin(9600);//This opens up communications to the GPS
  Serial.println("GPS Start");//Just show to the monitor that the sketch has started
  //GPS module instruction end

  //Flowcontrol setup begin
  pinMode(flowPin, INPUT); // Initialize the digital pin as an input
  Serial.begin(9600); // Start serial port
  attachInterrupt(0, rpm, RISING); //Interrupt is attached
  sei();    //RMH enable interrupt 0 on Pin 2 for a RISING signal.
                    // This causes ISR "rpm()" to be called and pulsesCounter to increment when the flowmeter pulses.
                    // note: 5600 pulses/litre, so if water flows at 1 gpm, this happens >350 times per second!
  //Flowcontrol setup end

}

void loop()
{

//Flowcontrol print in serial port begin
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


//Flowcontrol print in serial port end

  //GPS print in serial port begin

  while(serial_connection.available())//While there are characters to come from the GPS
  {
    gps.encode(serial_connection.read());//This feeds the serial NMEA data into the library one char at a time
  }
  if(gps.location.isUpdated())//This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
  {
    //Get the latest info from the gps object which it derived from the data sent by the GPS unit
    Serial.println("Satellite Count:");
    Serial.println(gps.satellites.value());
    Serial.println("Latitude:");
    Serial.println(gps.location.lat(), 6);
    Serial.println("Longitude:");
    Serial.println(gps.location.lng(), 6);
    Serial.println("Speed MPH:");
    Serial.println(gps.speed.mph());
    Serial.println("Altitude Feet:");
    Serial.println(gps.altitude.feet());
    Serial.println("");
  }
}
//GPS print in serial port end

