// EMF Detector for 8-Bit LED Display - v3
//
// original code/project by Aaron ALAI - aaronalai1@gmail.com
// modified for use w/ LED bargraph by Collin Cunningham - collin@makezine.com
// modified again by ComputerGeek for instructable & 7-segment display - http://www.instructables.com/id/Arduino-EMF-Detector/
// modified again by K Seigler for ATtiny85/Shift Register usage - me@kyleseigler.net

#define NUMREADINGS 8 // raise this number to increase data smoothing

int senseLimit = 15; // raise this number to decrease sensitivity (up to 1023 max)
int probePin = PB3;
int val = 0; // reading from probePin

int dataPin = PB0; // set shift register pins
int latchPin = PB1;
int clockPin = PB4;

int seq[15] = {1,2,4,8,16,32,64,128,64,32,16,8,4,2,1}; // test sequence matrix

// variables for smoothing
int readings[NUMREADINGS];                // the readings from the analog input
int index = 0;                            // the index of the current reading
int total = 0;                            // the running total
int average = 0;                          // final average of the probe reading

//CHANGE THIS TO affect the speed of the updates for numbers. Lower the number the faster it updates.
int updateTime = 5;

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  
  pinMode(probePin, INPUT);
  
  for (int i = 0; i < NUMREADINGS; i++)
    readings[i] = 0;                     // initialize all the readings to 0

  for (int n = 0; n < 15; n++)
    {
        digitalWrite(latchPin, LOW);                     // Pull latch LOW to start sending data
        shiftOut(dataPin, clockPin, MSBFIRST, seq[n]);   // Send the data
        digitalWrite(latchPin, HIGH);                    // Pull latch HIGH to stop sending data
        delay(100);
    }
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
}

void loop() {
  
  val = analogRead(probePin);  // take a reading from the probe
  
  if (val >= 0) {              // if the reading isn't zero, proceed

    val = constrain(val, 1, senseLimit);     // turn any reading higher than the senseLimit value into the senseLimit value
    val = map(val, 1, senseLimit, 1, 1023);  // remap the constrained value within a 1 to 1023 range
    val = constrain(val, 1, 1023);
    
    total -= readings[index];            // subtract the last reading
    readings[index] = val;               // read from the sensor
    total += readings[index];            // add the reading to the total
    index = (index + 1);                 // advance to the next index

    if (index >= NUMREADINGS)            // if we're at the end of the array...
      index = 0;                         // ...wrap around to the beginning

    average = total / NUMREADINGS;       // calculate the average

    if (average < 8) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 0);
      digitalWrite(latchPin, HIGH);
    }

    if ((average >= 8) && (average < 16)) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 1);
      digitalWrite(latchPin, HIGH);
    }

    if ((average >= 16) && (average <32)) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 3);
      digitalWrite(latchPin, HIGH);
    }

    if ((average >= 32) && (average < 64)) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 7);
      digitalWrite(latchPin, HIGH);
    }

    if ((average >= 64) && (average <125)) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 15);
      digitalWrite(latchPin, HIGH);
    }


    if ((average >= 125) && (average < 250)) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 31);
      digitalWrite(latchPin, HIGH);
    }

    if ((average >= 250) && (average < 500)) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 63);
      digitalWrite(latchPin, HIGH);
    }

    if ((average >= 500) && (average < 1000)) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 127);
      digitalWrite(latchPin, HIGH);
    }

    if (average > 1000) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 255);
      digitalWrite(latchPin, HIGH);
    }

    delay(updateTime);
  }
}
