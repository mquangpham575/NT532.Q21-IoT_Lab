import requests
import base64
import os

# Function to encode image to Base64
def encode_image_to_base64(image_path):
    if not os.path.exists(image_path):
        print(f"❌ Error: File '{image_path}' not found.")
        return None
    with open(image_path, "rb") as image_file:
        encoded_image = base64.b64encode(image_file.read()).decode('utf-8')
    return encoded_image

# Configuration
url = 'http://127.0.0.1:8000/recog'
# Change this to your test image path
image_path = 'Dataset/raw/khanh/WIN_20260514_13_36_48_Pro.jpg' 

encoded_image_data = encode_image_to_base64(image_path)

if encoded_image_data:
    # Parameters to send in the POST request
    data = {
        'image': encoded_image_data,
        'w': 160,  # Match FaceNet input size
        'h': 160
    }

    print(f"Sending request to {url}...")
    try:
        response = requests.post(url, data=data)
        print("Response from server:")
        print(response.text)
    except requests.exceptions.ConnectionError:
        print("Error: Could not connect to server. Is face_rec_flask.py running?")
