import serial
import csv
from datetime import datetime

# Open serial port
ser = serial.Serial('COM4', 9600)
ser.flushInput()

# Open or create CSV file
with open('sensor_data.csv', mode='w', newline='') as file:
    fieldnames = ["Time", "Accelerometer X", "Accelerometer Y", "Accelerometer Z", "Longitude", "Latitude", "Ultrasonic (inches)", "Ultrasonic (cm)"]
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
                         "Accelerometer Z": field_values['field3'], "Longitude": field_values['field4'], 
                         "Latitude": field_values['field5'], "Ultrasonic (inches)": field_values['field6'], 
                         "Ultrasonic (cm)": field_values['field7']})
        
        # Flush the file to ensure data is written immediately
        file.flush()
        # Print the data for debugging
        print(field_values)