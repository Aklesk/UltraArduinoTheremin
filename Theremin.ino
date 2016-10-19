/* HC-SR04 Sensor
   https://www.dealextreme.com/p/hc-sr04-ultrasonic-sensor-distance-measuring-module-133696
  
   This sketch reads a HC-SR04 ultrasonic rangefinder and returns the
   distance to the closest object in range. To do this, it sends a pulse
   to the sensor to initiate a reading, then listens for a pulse 
   to return.  The length of the returning pulse is proportional to 
   the distance of the object from the sensor.
     
   The circuit:
  * VCC connection of the sensor attached to +5V
  * GND connection of the sensor attached to ground
  * TRIG connection of the sensor attached to digital pin 2
  * ECHO connection of the sensor attached to digital pin 4
   Original code for Ping))) example was created by David A. Mellis
   Adapted for HC-SR04 by Tautvidas Sipavicius
   This example code is in the public domain.
 */


const int trigPin = 2;
const int echoPin = 4;
const int toneOut = 6;
const int numReadings = 10;     // number of readings over which to average our output
const int timeout = 5;          // Number of times we read zero before stopping output

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the total of all current readings
int average = 0;                // the average

int zeros = 0;                  // tally of consecutive readings of 0

void setup() {
  // initialize serial communication:
  // Serial.begin(9600);
  
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop()
{

  long duration; // duration of pingback

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH, 2500);

  // If we've read a zero, add it to the zero tally and loop back
  // Otherwise input our reading into the averaging code
  if (duration == 0) {

    // If we haven't reached timeout, just add one and loop back
    if (zeros < timeout) {
      zeros++;
    }

    // If we have reached timeout, set all average values to zero and reset
    else {
      for (int thisReading = 0; thisReading < numReadings; thisReading++) {
        readings[thisReading] = 0;
      }
      average = 0;
      zeros = 0;
    }
  }
  else {
    zeros = 0;                             // reset timeout tally
    total = total - readings[readIndex];   // subtract last reading
    readings[readIndex] = duration;        // input current reading
    readIndex = readIndex + 1;             // advance to the next position in the array
  
    // if we're at the end of the array...
    if (readIndex >= numReadings) {
      // ...wrap around to the beginning:
      readIndex = 0;
    }
  
    // Add up all of the entries and divide for the average
    total = 0;
    for (int i = 0; i < numReadings; i++) {
      total += readings[i];
    }
    
    // calculate the average:
    average = total / numReadings;
  }
    
  // Serial.print(average);
  // Serial.println();

  if (average != 0) {
    tone(toneOut, average * 1.5);
  }
  else {
    noTone(toneOut);
  }
  
  delay(10);
}
