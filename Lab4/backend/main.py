import os
import json
import sqlite3
import threading
from datetime import datetime
from typing import Optional
from fastapi import FastAPI, Request
from fastapi.staticfiles import StaticFiles
from fastapi.responses import HTMLResponse
from pydantic import BaseModel
import paho.mqtt.client as mqtt

app = FastAPI(title="IoT Telemetry API", description="Custom IoT backend for air quality monitoring", version="1.0.0")

class TelemetryPayload(BaseModel):
    temperature: Optional[float] = None
    humidity: Optional[float] = None
    air_quality: Optional[float] = None

class ControlCommand(BaseModel):
    command: str
    value: Optional[str] = None

DB_FILE = "sensor_data.db"

def init_db():
    conn = sqlite3.connect(DB_FILE)
    c = conn.cursor()
    c.execute('''
        CREATE TABLE IF NOT EXISTS sensors (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            device_id TEXT,
            temperature REAL,
            humidity REAL,
            air_quality REAL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    ''')
    conn.commit()
    conn.close()

init_db()

def insert_data(device_id, temp=None, hum=None, aq=None):
    conn = sqlite3.connect(DB_FILE)
    c = conn.cursor()
    c.execute('''
        INSERT INTO sensors (device_id, temperature, humidity, air_quality)
        VALUES (?, ?, ?, ?)
    ''', (device_id, temp, hum, aq))
    conn.commit()
    conn.close()

# MQTT Background Task for Raspberry Pi
def on_connect(client, userdata, flags, rc, properties=None):
    print("Connected to MQTT broker")
    client.subscribe("v1/devices/me/telemetry")

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        insert_data("raspberry", temp=payload.get("temperature"), hum=payload.get("humidity"))
        print(f"Stored MQTT Data: {payload}")
    except Exception as e:
        print(f"Error parsing MQTT message: {e}")

def mqtt_thread():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message
    try:
        client.connect("localhost", 1883, 60)
        client.loop_forever()
    except Exception as e:
        print("MQTT connection failed:", e)

threading.Thread(target=mqtt_thread, daemon=True).start()

# HTTP Endpoints for Wemos D1
@app.post("/api/v1/{token}/telemetry", summary="Receive sensor telemetry from device")
async def receive_telemetry(token: str, payload: TelemetryPayload):
    """Receive telemetry data from Wemos D1 (HTTP) or any device."""
    try:
        insert_data("wemos", aq=payload.air_quality)
        print(f"Stored HTTP Data: {payload}")
        return {"status": "success"}
    except Exception as e:
        return {"status": "error", "message": str(e)}

# Device Control API
@app.post("/api/control/{device_id}", summary="Send control command to a device")
async def control_device(device_id: str, cmd: ControlCommand):
    """Send a control command to a specific device (e.g., toggle LED, fan)."""
    print(f"Command received for {device_id}: {cmd}")
    return {"status": "success", "message": "Command queued", "device": device_id, "command": cmd.command}

# Data API for Dashboard
@app.get("/api/data")
def get_data(minutes: int = 5, last_id: int = 0):
    conn = sqlite3.connect(DB_FILE)
    conn.row_factory = sqlite3.Row
    c = conn.cursor()
    if last_id > 0:
        c.execute('SELECT * FROM sensors WHERE id > ? ORDER BY id ASC LIMIT 100', (last_id,))
    else:
        c.execute("SELECT * FROM sensors WHERE timestamp >= datetime('now', ?) ORDER BY id ASC", (f'-{minutes} minutes',))
    rows = c.fetchall()
    conn.close()
    return [dict(row) for row in rows]

# Mount Dashboard Frontend
from fastapi.responses import FileResponse

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
FRONTEND_DIR = os.path.abspath(os.path.join(BASE_DIR, "..", "frontend"))
os.makedirs(FRONTEND_DIR, exist_ok=True)

@app.get("/")
def read_root():
    return FileResponse(os.path.join(FRONTEND_DIR, "index.html"))

app.mount("/", StaticFiles(directory=FRONTEND_DIR, html=True), name="frontend")

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
