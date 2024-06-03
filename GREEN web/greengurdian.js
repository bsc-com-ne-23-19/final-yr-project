// data to simulate sensor readings and actuator status
let sensorData = {
    humidity: 55,
    temperature: 24,
    lightIntensity: 300,
    moisture: 40
};

let actuators = {
    fan: { status: "OFF", mode: "Manual" },
    heater: { status: "OFF", mode: "Manual" },
    waterPump: { status: "OFF", mode: "Manual" },
    bulbs: { status: "OFF", mode: "Manual" }
};

// Function to update the UI with the sensor data
function updateSensorData() {
    document.getElementById('humidity').textContent = `${sensorData.humidity}%`;
    document.getElementById('temperature').textContent = `${sensorData.temperature}°C`;
    document.getElementById('lightIntensity').textContent = `${sensorData.lightIntensity} lux`;
    document.getElementById('moisture').textContent = `${sensorData.moisture}%`;
}

// Function to update the UI with the actuator status
function updateActuatorStatus() {
    document.getElementById('fanStatus').textContent = actuators.fan.status;
    document.getElementById('fanMode').textContent = actuators.fan.mode;
    document.getElementById('heaterStatus').textContent = actuators.heater.status;
    document.getElementById('heaterMode').textContent = actuators.heater.mode;
    document.getElementById('waterPumpStatus').textContent = actuators.waterPump.status;
    document.getElementById('waterPumpMode').textContent = actuators.waterPump.mode;
    document.getElementById('bulbsStatus').textContent = actuators.bulbs.status;
    document.getElementById('bulbsMode').textContent = actuators.bulbs.mode;
}

// Function to toggle the status of an actuator
function toggleStatus(actuator) {
    actuators[actuator].status = actuators[actuator].status === "OFF" ? "ON" : "OFF";
    updateActuatorStatus();
}

// Function to toggle the mode of an actuator
function toggleMode(actuator) {
    actuators[actuator].mode = actuators[actuator].mode === "Manual" ? "Automatic" : "Manual";
    updateActuatorStatus();
}

function showHome() {
    document.getElementById("home").style.display = "block";
    document.getElementById("about").style.display = "none";
}

function showAbout() {
    document.getElementById("home").style.display = "none";
    document.getElementById("about").style.display = "block";
}

// Initialize the UI with the initial data
updateSensorData();
updateActuatorStatus();
