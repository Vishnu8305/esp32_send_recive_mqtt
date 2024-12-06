const mqtt = require('mqtt');
const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');

// MQTT Config
const brokerUrl = 'mqtt://192.168.0.141'; // Replace with your MQTT broker's IP
const commandTopic = 'esp32/commands';
const sensorTopic = 'esp32/data';

// REST API Config
const app = express();
app.use(bodyParser.json());
app.use(cors());

// Store sensor data
let sensorData = 'No Data';

// Connect to MQTT broker
const mqttClient = mqtt.connect(brokerUrl);

mqttClient.on('connect', () => {
  console.log('Connected to MQTT broker');
  
  // Subscribe to sensor topic
  mqttClient.subscribe(sensorTopic, (err) => {
    if (!err) {
      console.log(`Subscribed to topic: ${sensorTopic}`);
    }
  });
});

// Handle incoming MQTT messages (from ESP32)
mqttClient.on('message', (topic, message) => {
  if (topic === sensorTopic) {
    sensorData = message.toString();
    console.log(`Sensor Data: ${sensorData}`);
  }
});

// REST API Endpoints

// POST: Send command to ESP32
app.post('/api/command', (req, res) => {
  const { command } = req.body;
  if (command === '1' || command === '0') {
    mqttClient.publish(commandTopic, command);
    res.json({ message: 'Command sent', command });
  } else {
    res.status(400).json({ message: 'Invalid command, use "1" or "0"' });
  }
});

// GET: Fetch sensor data
app.get('/api/sensor', (req, res) => {
  res.json({ sensorData });
});

// Start the server
const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});
