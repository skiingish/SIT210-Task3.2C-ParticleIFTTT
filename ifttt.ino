/*
 * Project ifttt
 * Description: The following program takes the level from a light sensor and publishes 
 *              an event if it's sunny or not and then if the state changes. The following is picked up by a IFTTT trigger 
 * Author: Sean Corcoran
 * Date: 24/04/20
 */

//pin declarations
int led = D7;         // onboard LED
int lightSensor = A1; // pin to read pot value (pot middle terminal)

// variable to store lightSensor avg value
int analogvalueAvg;

bool sunlight = false; // <-- used for the sunlight string

int sunnyLevel = 200; // <-- the sensor level between sunny and not sunny

void setup()
{
  // pin IO declarations
  pinMode(led, OUTPUT);
  pinMode(lightSensor, INPUT);

  // Exposes the value of a variable on the Photon to an http GET method

  Particle.variable("lightValue", &analogvalueAvg, INT);
  Particle.variable("isSunny", &sunlight, BOOLEAN); // <-- is it sunny or not
}

void loop()
{
  // setup before main event loop
  // gets the state of the light and publish the event
  analogvalueAvg = getSensorValue(); //<-- get light level

  if (analogvalueAvg > sunnyLevel) // <-- if sunny pub it's sunny and set sunlight to true
  {
    sunlight = true;
    Particle.publish("isItSunny", "itsSunny", 60, PRIVATE);
  }
  else // <-- if it's not sunny then pub and set to sunlight to false
  {
    sunlight = false;
    Particle.publish("isItSunny", "itsNotSunny", 60, PRIVATE);
  }

  // after building the start up values of if it's sunny
  // keep looping and update the state of sunlight if a change in light level happens
  while (true)
  {
    analogvalueAvg = getSensorValue(); // <-- get light level

    if (analogvalueAvg < sunnyLevel && sunlight != false) // <-- if the avg is under the requirement for it to be sunny and the bool sunlight is not false then update the change
    {
      sunlight = false;
      Particle.publish("isItSunny", "itsNotSunny", 60, PRIVATE);
    }
    else if (analogvalueAvg >= sunnyLevel && sunlight != true) // <-- if the avg is over the requirement for it to be sunny and the bool sunlight is not true then update the change
    {
      sunlight = true;
      Particle.publish("isItSunny", "itsSunny", 60, PRIVATE);
    }
    else
    {
      // do nothing and publish no updates to the isSunny if no changes have been found to effect the state of sunlight
    }

    //for monitoring
    String analogvalueString = String(analogvalueAvg, DEC); // create a string from the avg of the sensor reads, (converts a int to a sting with a base)
    Particle.variable("lightValue", &analogvalueAvg, INT);
    //Particle.publish("lightValue", analogvalueString, PRIVATE); // push and publish the data
  }
}

int getSensorValue()
{
  // reset all values before resampling
  int n = 4; // 5 seconds of sampling before uploading a avg

  float num[4], sum = 0.0, avg = 0;

  // check to see what the value of the lightSensor is
  // and store it in the int variable analogvalueAvg
  /* The below builds an array of readings over a period of 5 seconds at one reading a second */
  for (int i = 0; i < n; ++i)
  {
    num[i] = analogRead(lightSensor);
    sum += num[i];
    delay(1000);
  }
  // Create an avg from the array
  avg = sum / n;
  analogvalueAvg = avg;
  return analogvalueAvg;
}