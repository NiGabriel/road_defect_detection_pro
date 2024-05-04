# road_defect_detection_pro

Complete project is in the folder called final_pro. A file called data_analysis.ipynb was used to analyse and visualize data stored in data folder. Additionally, save_data.py was used to store data in csv file from the COM port to which the bluetooth is connected to. For instance, if bluetooth(HC-05) is connected with your computer on COM6, you must modify save_data and use COM6. 

**The components to consider:**
1. Accelerometer sensor (ITG/MPU module)
2. Ultrasonic sensor
3. Arduino UNO
4. GPS (GT-U7)
5. Bluetooth(HC-05)
6. Batteries(2) varying from 5v to 12v
7. GSM

**Software**
1. Arduino IDE
2. VS code (for Python) //make sure you can manipulate jupyter files.

**PIN connection**
1. GSM TX: PIN 7, RX: PIN 8
2. GPS TX: 4, RX: 3
3. Ultrasonic tiiger: PIN 12
4. Ultrasonic echo: PIN 13
5. Bluetooth TX: arduino Tx, RX: Arduino Rx

GSM helps to send data on the cloud(thingspeak). However, it may not be reliable because it doesn't capture each data sent by the sonsors.

GSM needs its own voltage supplier to be sufficient and that's why 2 batteries were used otherwise one battery may be good enough.
