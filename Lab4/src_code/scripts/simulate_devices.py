import requests
import paho.mqtt.client as mqtt
import time
import json
import random
import threading

# Configuration - Local Custom Backend
THINGSBOARD_HOST = 'localhost'
THINGSBOARD_HTTP_PORT = 8000
WEMOS_HTTP_TOKEN = 'wemos_d1'
RASPBERRY_MQTT_TOKEN = 'raspberry_pi'

# --- WEMOS HTTP SIMULATION ---
def simulate_wemos_http():
    url = f"http://{THINGSBOARD_HOST}:{THINGSBOARD_HTTP_PORT}/api/v1/{WEMOS_HTTP_TOKEN}/telemetry"
    print(f"[Wemos] Starting HTTP simulation to {url}")
    
    while True:
        # Simulate MQ-135 reading (Air Quality)
        air_quality = random.randint(300, 800)
        payload = {"air_quality": air_quality}
        
        try:
            response = requests.post(url, json=payload, timeout=5)
            if response.status_code == 200:
                print(f"[Wemos] Sent HTTP Telemetry: {payload}")
            else:
                print(f"[Wemos] HTTP Error: {response.status_code}")
        except Exception as e:
            print(f"[Wemos] HTTP Exception: {e}")
            
        time.sleep(5)

# --- RASPBERRY MQTT SIMULATION ---
def simulate_rpi_mqtt():
    client = mqtt.Client()
    client.username_pw_set(RASPBERRY_MQTT_TOKEN)
    
    print(f"[RPi] Starting MQTT simulation to {THINGSBOARD_HOST}")
    
    try:
        client.connect(THINGSBOARD_HOST, 1883, 60)
        client.loop_start()
        
        while True:
            # Simulate DHT22 readings
            temp = random.uniform(20.0, 35.0)
            hum = random.uniform(40.0, 80.0)
            payload = {"temperature": round(temp, 2), "humidity": round(hum, 2)}
            
            client.publish('v1/devices/me/telemetry', json.dumps(payload), qos=1)
            print(f"[RPi] Published MQTT Telemetry: {payload}")
            
            time.sleep(5)
            
    except Exception as e:
        print(f"[RPi] MQTT Exception: {e}")

if __name__ == "__main__":
    print("=== IoT Device Simulator ===")
    print("Press Ctrl+C to stop.\n")
    
    # Run both simulations in parallel
    t1 = threading.Thread(target=simulate_wemos_http, daemon=True)
    t2 = threading.Thread(target=simulate_rpi_mqtt, daemon=True)
    
    t1.start()
    t2.start()
    
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nStopping simulations...")
