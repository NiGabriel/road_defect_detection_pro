import serial
import csv
from datetime import datetime
import requests
import time


# Open serial port
ser = serial.Serial('COM3', 9600)
ser.flushInput()

# Open or create CSV file
with open('data/sensor_data.csv', mode='w', newline='') as file:
    fieldnames = ["Time", "Accelerometer X", "Accelerometer Y", "Accelerometer Z", "Gyro roll rate", "Gyro pitch rate", "Gyro yaw rate", "Longitude", "Latitude", "Ultrasonic (inches)", "Ultrasonic (cm)"]
    writer = csv.DictWriter(file, fieldnames=fieldnames)
    writer.writeheader()

    while True:
        # Read data from serial port
        line = ser.readline().decode('utf-8').rstrip()

        # Split the data into individual values
        data = line.split('&')

        # Extract field values
        field_values = {}
        for item in data[1:]:
            field_name, value = item.split('=')
            field_values[field_name] = value

        # Get current time
        current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        # Write data to CSV file
        writer.writerow({"Time": current_time, "Accelerometer X": field_values['field1'], "Accelerometer Y": field_values['field2'], 
                         "Accelerometer Z": field_values['field3'], "Gyro roll rate": field_values['field4'], "Gyro pitch rate": field_values['field5'], 
                         "Gyro yaw rate": field_values['field6'], "Longitude": field_values['field7'], 
                         "Latitude": field_values['field8'], "Ultrasonic (inches)": field_values['field9'], 
                         "Ultrasonic (cm)": field_values['field10']})
        
        # Flush the file to ensure data is written immediately
        file.flush()
        # Print the data for debugging
        print(field_values)


# WRITE_API_KEY = 'P7NV9M1VIM6AW52I'

# CSV_FILE = 'data/sensor_data.csv'

# def send_to_thingspeak(data):
#     url = f'https://api.thingspeak.com/update?api_key={WRITE_API_KEY}&{data}'
#     response = requests.get(url)
#     if response.status_code == 200:
#         print("Data sent successfully to ThingSpeak")
#     else:
#         print("Failed to send data to ThingSpeak")

# def read_csv_and_send():
#     with open(CSV_FILE, 'r') as file:
#         reader = csv.reader(file)
#         for row in reader:
#             data = "&".join([f"field{i+1}={value}" for i, value in enumerate(row)])
#             send_to_thingspeak(data)
#             time.sleep(15)  # ThingSpeak allows updates every 15 seconds

# if __name__ == "__main__":
#     read_csv_and_send()