import cv2
import requests
import base64
import numpy as np

# Configuration
URL = 'http://127.0.0.1:8000/recog'

def main():
    """
    Main loop for webcam capture and face recognition.
    """
    from collections import deque, Counter
    
    # Initialization
    cap = cv2.VideoCapture(0)
    prediction_history = deque(maxlen=10) # Store last 10 predictions
    current_display_name = "Unknown"
    
    if not cap.isOpened():
        print("Error: Could not open webcam.")
        return

    print("Webcam Recognition System Started (with smoothing)...")
    print("Press 'q' to quit.")

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        h, w = frame.shape[:2]
        _, buffer = cv2.imencode('.jpg', frame)
        encoded_string = base64.b64encode(buffer).decode('utf-8')

        data = {'image': encoded_string, 'w': w, 'h': h}

        import time
        start_time = time.time()
        
        try:
            response = requests.post(URL, data=data, timeout=3)
            raw_name = response.text
            latency = (time.time() - start_time) * 1000 # Convert to milliseconds
        except Exception:
            raw_name = "Error"
            latency = 0

        # Smoothing Logic: Only update display if a name is consistent
        prediction_history.append(raw_name)
        
        # Get the most frequent prediction in the last 10 frames
        most_common = Counter(prediction_history).most_common(1)[0]
        if most_common[1] >= 4: # Threshold: name must appear in 4/10 recent frames
            current_display_name = most_common[0]

        # Display result
        color = (0, 255, 0) if current_display_name != "Unknown" else (0, 0, 255)
        cv2.putText(frame, f"Identity: {current_display_name}", (10, 30), 
                    cv2.FONT_HERSHEY_SIMPLEX, 1, color, 2)
        cv2.putText(frame, f"Latency: {latency:.2f}ms", (10, 60), 
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)

        # Show the frame
        cv2.imshow('Face Recognition Client', frame)

        # Break loop on 'q' key
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Release resources
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
