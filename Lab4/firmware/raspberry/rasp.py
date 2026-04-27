import paho.mqtt.client as mqtt
import time
import json
import random

# Thư viện 
try:
    import adafruit_dht
    import board
    # Khởi tạo thiết bị (Pin GPIO 4)
    dhtDevice = adafruit_dht.DHT11(board.D4)
    HAS_DHT = True
except ImportError:
    HAS_DHT = False
    print("Adafruit_DHT libraries not found. Running in simulation mode...")
except Exception as e:
    HAS_DHT = False
    print(f"Error initializing DHT sensor: {e}. Running in simulation mode...")

# Custom MQTT Broker Configuration (Local Docker)
THINGSBOARD_HOST = 'localhost'
ACCESS_TOKEN = 'raspberry_pi'

# Initialize MQTT Client
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
# client.username_pw_set(ACCESS_TOKEN) # Not needed for custom anonymous broker

# Send telemetry data
def send_telemetry():
    try:
        if HAS_DHT:
            try:
                # Đọc từ cảm biến thật
                temperature = dhtDevice.temperature
                humidity = dhtDevice.humidity
            except RuntimeError as error:
                # Lỗi đọc DHT thường xuyên xảy ra, chỉ cần thử lại
                print(f"Sensor error: {error.args[0]}. Retrying...")
                return
        else:
            # Dữ liệu giả lập
            humidity = random.uniform(40, 60)
            temperature = random.uniform(20, 30)

        if humidity is not None and temperature is not None:
            payload = {
                "temperature": round(temperature, 2),
                "humidity": round(humidity, 2)
            }
            client.publish('v1/devices/me/telemetry', json.dumps(payload), qos=1)
            print(f"Published Real Data: {payload}" if HAS_DHT else f"Published Sim Data: {payload}")
        
    except Exception as e:
        print(f"Error: {e}")

# Main execution loop
def main():
    try:
        client.connect(THINGSBOARD_HOST, 1883, 60)
        client.loop_start()
        print(f"Connected to {THINGSBOARD_HOST}")

        while True:
            send_telemetry()
            time.sleep(5) # Delay 5 seconds
            
    except KeyboardInterrupt:
        print("Stopping client...")
        client.loop_stop()
        client.disconnect()
    except Exception as e:
        print(f"Connection failed: {e}")

if __name__ == "__main__":
    main()
