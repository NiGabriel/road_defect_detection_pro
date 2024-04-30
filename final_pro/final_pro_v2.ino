#include "Wire.h"
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial gprsSerial(7, 8);
SoftwareSerial gpsSerial(4, 3);

const int trigger_pin = 12; // Ultrasonic sensor trigger pin
const int echo_pin = 13;    // Ultrasonic sensor echo pin
const int MPU_ADDR = 0x68;
const unsigned long GPS_TIMEOUT = 2000;

TinyGPSPlus gps;

int16_t accelerometer_x, accelerometer_y, accelerometer_z;
int16_t gyro_x, gyro_y, gyro_z;
int16_t temperature;
int16_t prev_accelerometer_y = 0;
bool upward_movement_detected = false;
bool downward_movement_detected = false;

char tmp_str[7];

char *convert_int16_to_str(int16_t i)
{
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void setup()
{
  gprsSerial.begin(9600);
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
}

void loop()
{
  unsigned long start = millis();
  while (millis() - start < GPS_TIMEOUT)
  {
    while (gpsSerial.available())
    {
      if (gps.encode(gpsSerial.read()))
      {
        if (gps.location.isValid())
        {
          // Serial.print("Latitude: ");
          // Serial.println(gps.location.lat(), 6);
          // Serial.print("Longitude: ");
          // Serial.println(gps.location.lng(), 6);
        }
      }
    }
  }

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 7 * 2, true);

  accelerometer_x = Wire.read() << 8 | Wire.read();
  accelerometer_y = Wire.read() << 8 | Wire.read();
  accelerometer_z = Wire.read() << 8 | Wire.read();
  temperature = Wire.read() << 8 | Wire.read();

  gyro_x = Wire.read() << 8 | Wire.read();
  gyro_y = Wire.read() << 8 | Wire.read();
  gyro_z = Wire.read() << 8 | Wire.read();

  // Serial.print("aX = ");
  // Serial.print(convert_int16_to_str(accelerometer_x));
  // Serial.print(" | aY = ");
  // Serial.print(convert_int16_to_str(accelerometer_y));
  // Serial.print(" | aZ = ");
  // Serial.print(convert_int16_to_str(accelerometer_z));

  // Serial.print(" | tmp = ");
  // Serial.print(temperature / 340.00 + 36.53);
  // Serial.print(" | gX = ");
  // Serial.print(convert_int16_to_str(gyro_x));
  // Serial.print(" | gY = ");
  // Serial.print(convert_int16_to_str(gyro_y));
  // Serial.print(" | gZ = ");
  // Serial.print(convert_int16_to_str(gyro_z));

  // Serial.println();

  if (accelerometer_y > prev_accelerometer_y + 1000)
  {
    upward_movement_detected = true;
    prev_accelerometer_y = accelerometer_y;
  }
  else if (accelerometer_y < prev_accelerometer_y - 1000)
  {
    downward_movement_detected = true;
    prev_accelerometer_y = accelerometer_y;
  }

  if (upward_movement_detected || downward_movement_detected)
  {
    if (gps.location.isValid())
    {
      gpsSerial.print("GPS Latitude: ");
      gpsSerial.println(gps.location.lat(), 6);
      gpsSerial.print("GPS Longitude: ");
      gpsSerial.println(gps.location.lng(), 6);
    }
    upward_movement_detected = false;
    downward_movement_detected = false;
  }

  long duration, inches, cm;

  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);

  duration = pulseIn(echo_pin, HIGH);

  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);

  // Serial.print(inches);
  // Serial.print("in, ");
  // Serial.print(cm);
  // Serial.print("cm");

  delay(1000);

  if (gprsSerial.available())
    Serial.write(gprsSerial.read());

  gprsSerial.println("AT");
  delay(1000);

  gprsSerial.println("AT+CPIN?");
  delay(1000);

  gprsSerial.println("AT+CREG?");
  delay(1000);

  gprsSerial.println("AT+CGATT?");
  delay(1000);

  gprsSerial.println("AT+CIPSHUT");
  delay(1000);

  gprsSerial.println("AT+CIPSTATUS");
  delay(2000);

  gprsSerial.println("AT+CIPMUX=0");
  delay(2000);

  gprsSerial.println("AT+CSTT=\"internet\""); //setting APN
  delay(1000);

  gprsSerial.println("AT+CIICR"); //wireless connection
  delay(3000);

  gprsSerial.println("AT+CIFSR"); //get local IP address
  delay(2000);

  gprsSerial.println("AT+CIPSTART=\"TCP\", \"api.thingspeak.com\",\"80\""); //start the connection
  delay(6000);

  gprsSerial.println("AT+CIPSEND"); //begin send data to remote server
  delay(4000);

  // delay(2000); // Allow time for the GSM module to initialize

  // if (sendSMS("+250782055492", composeSMS().c_str())) // Replace with your desired phone number and message
  // {
  //   Serial.println("\nSMS sent OK.");
  // }
  // else
  // {
  //   Serial.println("\nError sending SMS.");
  // }

  // Formulate the data string
  String str = "GET https://api.thingspeak.com/update?api_key=P7NV9M1VIM6AW52I&field1=" + String(accelerometer_x) +
               "&field2=" + String(accelerometer_y) + "&field3=" + String(accelerometer_z) +
               "&field4=" + String(gps.location.lng(), 6) + "&field5=" + String(gps.location.lat(), 6) +
               "&field6=" + String(inches) + "&field7=" + String(cm);
  Serial.println(str);
  gprsSerial.println(str);

  delay(4000);

  gprsSerial.println((char)26); //sending
  delay(5000);
  gprsSerial.println();

  gprsSerial.println("AT+CIPSHUT"); //close the connection
  delay(100);
}

// String composeSMS()
// {
//   // Compose SMS with sensor data
//   String smsMessage = "Sensor Data:\n";
//   smsMessage += "Accelerometer X: " + String(accelerometer_x) + "\n";
//   smsMessage += "Accelerometer Y: " + String(accelerometer_y) + "\n";
//   smsMessage += "Accelerometer Z: " + String(accelerometer_z) + "\n";
//   smsMessage += "Temperature: " + String(temperature / 340.00 + 36.53) + "\n";
//   smsMessage += "Gyro X: " + String(gyro_x) + "\n";
//   smsMessage += "Gyro Y: " + String(gyro_y) + "\n";
//   smsMessage += "Gyro Z: " + String(gyro_z) + "\n";
//   smsMessage += "GPS Latitude: " + String(gps.location.lat(), 6) + "\n";
//   smsMessage += "GPS Longitude: " + String(gps.location.lng(), 6) + "\n";
//   return smsMessage;
// }

// bool sendSMS(const char *phoneNumber, const char *message)
// {
//   gprsSerial.println("AT+CMGF=1"); // Set the GSM module to text mode
//   delay(1000);

//   gprsSerial.print("AT+CMGS=\"");
//   gprsSerial.print(phoneNumber);
//   gprsSerial.println("\"");

//   delay(1000);
//   gprsSerial.print(message);
//   delay(100);
//   gprsSerial.println((char)26); // End SMS transmission with Ctrl+Z
//   delay(1000);

//   if (gprsSerial.find("OK"))
//   {
//     return true;
//   }
//   else
//   {
//     return false;
//   }
// }

long microsecondsToInches(long microseconds)
{
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}