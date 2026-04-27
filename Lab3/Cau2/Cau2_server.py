from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/api/sensors', methods=['POST'])
def handle_sensors():
    data = request.json
    if not data:
        return jsonify({"error": True, "message": "No data received"}), 400

    # Lấy giá trị từ JSON payload (Wemos gửi lên)
    light_val = data.get("data", {}).get("light", 0)
    dist_val = data.get("data", {}).get("distance", 0)

    print(f"--- Nhận dữ liệu ---")
    print(f"Ánh sáng: {light_val} | Khoảng cách: {dist_val} cm")

    led_count = 0
    # Logic: Nếu có người tiếp cận (khoảng cách < 50cm)
    if dist_val < 50:
        # Càng tối thì càng bật nhiều đèn
        if light_val < 300:
            led_count = 3
        elif light_val < 700:
            led_count = 2
        else:
            led_count = 1
    else:
        # Không có người thì tắt hết
        led_count = 0

    # Phản hồi theo định dạng yêu cầu
    response = {
        "error": False,
        "message": "Processed successfully",
        "data": {
            "led_count": led_count
        }
    }
    print(f"Server trả về: Bật {led_count} đèn")
    return jsonify(response)

if __name__ == '__main__':
    # Chạy server ở cổng 5000, lắng nghe mọi IP trong mạng nội bộ
    app.run(host='0.0.0.0', port=5000)