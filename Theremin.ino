/* Theremin
   Project to build a simple theremin using an arduino and two HC-SR04 sensors wired in parallel
   for better accuracy.

   Code written by Wolf Hatch, with thanks to various public domain examples.
 */


const int trigPin = 2;
const int echoPin = 4;
const int toneOut = 6;
const int numReadings = 20;     // number of readings over which to average our output
const int timeout = 5;          // Number of times we read zero before stopping output

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
long total = 0;                  // the total of all current readings
int average = 0;                // the average
int freq = 0;                   // the frequency this reading has converted to
int zeros = 0;                  // tally of consecutive readings of 0

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  // Set up the raw output override pin
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);
}

void loop()
{

  int duration; // duration of pingback

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
  duration = pulseIn(echoPin, HIGH, 2800);

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
      total = 0;
      average = 0;
      zeros = 0;
    }
  }
  else {
    freq = 440 * pow(2, round(duration / 38 - 16) / 12.0);   // snap the reading to musical note frequency
    zeros = 0;                                               // reset timeout tally
    total = total - readings[readIndex];                     // subtract last reading
    readings[readIndex] = freq;                              // input current reading
    total = total + readings[readIndex];                     // add new reading to total
    readIndex = readIndex + 1;                               // advance to the next position in the array
  
    // if we're at the end of the array...
    if (readIndex >= numReadings) {
      // ...wrap around to the beginning:
      readIndex = 0;
    }
    
    // calculate the average:
    average = total / numReadings;
  }

  // now normalize the value to standard musical frequencies
    
  Serial.print(average);
  Serial.println();

  // To prevent warble when getting a first reading after silence
  // wait until the readings all have a value before sounding
  // this also filters out random noise from the sensors
  int zeroReadings = 0;
  for (int i = 0; i < numReadings; i++) {
    if (readings[i] == 0) {
      zeroReadings++;
    }
  }

  if (digitalRead(8) == 0){
    if (duration > 100) {
      tone(toneOut, duration * 1.5);
    }
    else {
      noTone(toneOut);
    }
  }
  else if (average != 0 && zeroReadings == 0) {
    tone(toneOut, average);
  }
  else {
    noTone(toneOut);
  }
  
  delay(10);
}
