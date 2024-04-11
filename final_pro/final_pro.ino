#include "Wire.h"
#include <SoftwareSerial.h>

SoftwareSerial gprsSerial(2,3);

#include <String.h>
// #include <DHT.h>

// #define DHTPIN A0

// DHT dht(DHTPIN, DHT11);

const int MPU_ADDR=0x68;

int16_t accelerometer_x, accelerometer_y, accelerometer_z;
int16_t gyro_x, gyro_y, gyro_z;
int16_t temperature;

char tmp_str[7];

char* convert_int16_to_str(int16_t i){
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}


void setup() {
  // put your setup code here, to run once:
  gprsSerial.begin(9600);
  Serial.begin(9600);
  // dht.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}


void loop() {
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 7*2, true);

  accelerometer_x = Wire.read()<<8 | Wire.read();
  accelerometer_y = Wire.read()<<8 | Wire.read();
  accelerometer_z = Wire.read()<<8 | Wire.read();
  temperature = Wire.read()<<8 | Wire.read();

  gyro_x = Wire.read()<<8 | Wire.read();
  gyro_y = Wire.read()<<8 | Wire.read();
  gyro_z = Wire.read()<<8 | Wire.read();

  Serial.print("aX = "); Serial.print(convert_int16_to_str(accelerometer_x));
  Serial.print(" | aY = "); Serial.print(convert_int16_to_str(accelerometer_y));
  Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(accelerometer_z));

  Serial.print(" | tmp = "); Serial.print(temperature/340.00+36.53);
  Serial.print(" | gX = "); Serial.print(convert_int16_to_str(gyro_x));
  Serial.print(" | gY = "); Serial.print(convert_int16_to_str(gyro_y));
  Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(gyro_z));

  Serial.println();

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

  // ShowSerialData();

  gprsSerial.println("AT+CSTT=\"airtelgprs.com\"");//setting APN
  delay(1000);

  // ShowSerialData();

  gprsSerial.println("AT+CIICR");//wireless connection
  delay(3000);

  // ShowSerialData();

  gprsSerial.println("AT+CIFSR");//get local IP address
  delay(2000);

  // ShowSerialData();

  gprsSerial.println("AT+CIPSTART=\"TCP\", \"api.thingspeak.com\",\"80\""); //start the connection
  delay(6000);

  // ShowSerialData();

  gprsSerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);

  // ShowSerialData();

  String str = "GET https://api.thingspeak.com/update?api_key=P7NV9M1VIM6AW52I&field1="+String(accelerometer_x)+"&field2="+String(accelerometer_y)+"&field3="+String(accelerometer_z);
  Serial.println(str);
  gprsSerial.println(str);

  delay(4000);
  // ShowSerialData();

  gprsSerial.println((char)26);//sending
  delay(5000);
  gprsSerial.println();

  // ShowSerialData();

  gprsSerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  // ShowSerialData();

}
