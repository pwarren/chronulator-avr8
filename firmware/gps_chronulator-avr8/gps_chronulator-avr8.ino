/* GPS Timing for "The Volt" clock
 *
 * Requires:
 * TimeZone: https://github.com/JChristensen/Timezone.git
 * TinyGPS: http://arduiniana.org/libraries/TinyGPS/
 * The Arduino Time Libraries
 *
 * Assumes:
 * Meters previously calibrated?
 *
 * Side Effects:
 * May chew batteries quickly!
 *
 * Copyright 2016 Paul Warren <pwarren@pwarren.id.au>
 *
 * This file is part of chronulator-avr8.
 *
 * chronulator-avr8 is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * chronulator-avr8 is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General
 * Public License along with chronulator-avr8. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 **********************************************************
 */

#include <SoftwareSerial.h>
#include <Time.h>
#include <TimeZone.h>
#include <TineyGPS.h>
#include <avr/sleep.h>
#include <util/delay.h>


/* See https://github.com/JChristensen/Timezone.git for full deatils on
 * TimeChangeRule formats
 */
// Australia/Sydney
TimeChangeRule myDST = {"AEDST", First, Sun, Nov,  2, +660};
TimeChangeRule mySTD = {"AEST", First, Sun, Apr, 2, +600};
Timezone myTZ(myDST, mySTD);

TimeChangeRule *tcr;
time_t utc, local;
TinyGPS gps;

// For GPS input
SoftwareSerial GPSserial(2, 3); // RX, TX

// how much serial data we expect before a newline
const unsigned int MAX_INPUT = 100;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  
  // set the data rate for the SoftwareSerial port
  GPSserial.begin(4800);

  setSyncProvider(gpsTimeSync); 
}

void loop() {
  while (GPSSerial.available()) {
    gps.encode(seral_gps.read());
  }

  if(timeStatus()!=timeNotSet) {
    update_time(myTZ.toLocal(now(), &tcr));
  }

  // Sleep as long as possible here?
  // Interrupts etc?

}


ime_t gpsTimeSync(){
  //  returns time if avail from gps, else returns 0
  unsigned long fix_age = 0 ;
  gps.get_datetime(NULL,NULL, &fix_age);
  unsigned long time_since_last_fix;
  if(fix_age < 1000)
    return gpsTimeToArduinoTime(); // return time only if updated recently by gps  
  return 0;
}

time_t gpsTimeToArduinoTime(){
  // returns time_t from gps date and time with the given offset hours
  tmElements_t tm;
  int year;
  gps.crack_datetime(&year, &tm.Month, &tm.Day, &tm.Hour, &tm.Minute, &tm.Second, NULL, NULL);
  tm.Year = year - 1970; 
  time_t time = makeTime(tm);
  return time + (offset * SECS_PER_HOUR);
}
