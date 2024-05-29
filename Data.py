from flask import Flask, render_template
import firebase_admin
from firebase_admin import credentials, db

# Initialize Flask app
app = Flask(__name__)

# Initialize Firebase
cred = credentials.Certificate(r"C:\Users\bsc_com_ne_14_19\Desktop\GuardianApp\GuardianApp\serviceAccountKey.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://grow-996d8-default-rtdb.firebaseio.com/'
})

# Function to initialize the database with data
def initialize_database():
    ref = db.reference('/')
    data = {
        "sensors": {
            "sensor_id_1": {
                "type": "temperature",
                "location": "greenhouse_1",
                "value": 22.5,
                "unit": "Celsius",
                "timestamp": "2023-05-20T12:00:00Z"
            },
            "sensor_id_2": {
                "type": "humidity",
                "location": "greenhouse_1",
                "value": 60,
                "unit": "%",
                "timestamp": "2023-05-20T12:00:00Z"
            },
            "sensor_id_3": {
                "type": "moisture",
                "location": "greenhouse_1",
                "value": 45,
                "unit": "%",
                "timestamp": "2023-05-20T12:00:00Z"
            },
            "sensor_id_4": {
                "type": "light",
                "location": "greenhouse_1",
                "value": 300,
                "unit": "lux",
                "timestamp": "2023-05-20T12:00:00Z"
            }
        },
        "actuators": {
            "fan": {
                "status": "on",
                "location": "greenhouse_1",
                "last_changed": "2023-05-20T12:05:00Z"
            },
            "spray": {
                "status": "off",
                "location": "greenhouse_1",
                "last_changed": "2023-05-20T12:05:00Z"
            },
            "motor": {
                "status": "on",
                "location": "greenhouse_1",
                "last_changed": "2023-05-20T12:05:00Z"
            },
            "LED": {
                "status": "off",
                "location": "greenhouse_1",
                "last_changed": "2023-05-20T12:05:00Z"
            }
        },
        "alerts": {
            "alert_id_1": {
                "type": "temperature",
                "message": "Temperature is too high!",
                "sensor_id": "sensor_id_1",
                "timestamp": "2023-05-20T12:10:00Z"
            }
        },
        "configurations": {
            "config_id_1": {
                "setting": "temperature_threshold",
                "value": 25.0,
                "unit": "Celsius"
            },
            "config_id_2": {
                "setting": "humidity_threshold",
                "value": 70,
                "unit": "%"
            },
            "config_id_3": {
                "setting": "moisture_threshold",
                "value": 50,
                "unit": "%"
            },
            "config_id_4": {
                "setting": "light_threshold",
                "value": 400,
                "unit": "lux"
            }
        }
    }
    ref.set(data)
    return data

# Function to fetch and print the data
def fetch_data():
    ref = db.reference('/')
    data = ref.get()
    if data:
        print(data)
    else:
        print("No data available")

# Function to update the value of a sensor
def update_sensor_value(sensor_id, new_value):
    ref = db.reference(f'sensors/{sensor_id}')
    ref.update({
        'value': new_value,
        'timestamp': "2023-05-20T12:30:00Z"  # Update the timestamp accordingly
    })
    print("Sensor value updated successfully!")

# Function to update the status of an actuator
def update_actuator_status(actuator_name, new_status):
    ref = db.reference(f'actuators/{actuator_name}')
    ref.update({
        'status': new_status,
        'last_changed': "2023-05-20T12:30:00Z"  # Update the timestamp accordingly
    })
    print(f"{actuator_name.capitalize()} status updated successfully!")

# Function to delete an actuator
def delete_actuator(actuator_name):
    ref = db.reference(f'actuators/{actuator_name}')
    ref.delete()
    print(f"{actuator_name.capitalize()} deleted successfully!")

# Initialize the database with data
initialize_database()

# Fetch and print the data
fetch_data()

# Update the value of a sensor 
update_sensor_value('sensor_id_1', 25.0)

# Update the status of an actuator 
update_actuator_status('fan', 'on')


# Route for the homepage
@app.route('/')
def index():
    # Reference to the root of the database
    ref = db.reference('/sensors')

    # Read data from the database
    sensor_data = ref.get()

    # Pass sensor data to the HTML template
    return render_template('index.html', sensor_data=sensor_data)

if __name__ == '__main__':
    app.run(debug=True)
