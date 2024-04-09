import cv2
import numpy as np
from keras.models import load_model
from keras.preprocessing.image import img_to_array

# Load YOLO
net = cv2.dnn.readNet("yolov3.weights", "yolov3.cfg")

layer_names = net.getLayerNames()
output_layers = [layer_names[i - 1] for i in net.getUnconnectedOutLayers()]

# Load COCO classes
with open("coco.names", "r") as f:
    classes = [line.strip() for line in f.readlines()]

# Load emotion model
emotion_model = load_model('emotion_model.hdf5')

# Define emotion labels
emotion_labels = {0: 'angry', 1: 'disgust', 2: 'fear', 3: 'happy', 4: 'sad', 5: 'surprise', 6: 'neutral'}

# Open camera
cap = cv2.VideoCapture(0)

# Known parameters
Z = 10  # replace with the known distance of the object from the camera in cm
f_x = 948.9546463110819  # focal length in the x-axis (in pixels)
f_y = 952.2636708744681  # focal length in the y-axis (in pixels)
c_x = 653.781919785527  # principal point x-coordinate (in pixels)
c_y = 366.05457251029446  # principal point y-coordinate (in pixels)

while True:
    _, frame = cap.read()

    height, width, channels = frame.shape

    # Detect objects
    blob = cv2.dnn.blobFromImage(frame, 0.00392, (320, 320), (0, 0, 0), True, crop=False)

    net.setInput(blob)
    outs = net.forward(output_layers)

    # Show information on screen
    class_ids = []
    confidences = []
    boxes = []

    for out in outs:
        for detection in out:
            scores = detection[5:]
            class_id = np.argmax(scores)
            confidence = scores[class_id]
            if confidence > 0.5: 
                center_x = int(detection[0] * width)
                center_y = int(detection[1] * height)
                w = int(detection[2] * width)
                h = int(detection[3] * height)

                # Rectangle coordinates
                x = int(center_x - w / 2)
                y = int(center_y - h / 2)

                boxes.append([x, y, w, h])
                confidences.append(float(confidence))
                class_ids.append(class_id)

                # If the detected object is a person, try to detect emotion
                if class_id == 0:
                    face_img = frame[y:y+h, x:x+w]
                    if face_img.size == 0:
                        print("Face image is empty.")
                        continue
                    face_img_gray = cv2.cvtColor(face_img, cv2.COLOR_BGR2GRAY)
                    face_img_gray = cv2.resize(face_img_gray, (64, 64))  # Resize to match model input
                    face_img_gray = face_img_gray.astype("float") / 255.0
                    face_img_gray = img_to_array(face_img_gray)
                    face_img_gray = np.expand_dims(face_img_gray, axis=0)

                    emotion_prediction = emotion_model.predict(face_img_gray)[0]
                    emotion = emotion_labels[emotion_prediction.argmax()]

                    print(f"Detected emotion: {emotion}")
              

    indexes = cv2.dnn.NMSBoxes(boxes, confidences, 0.5, 0.4)

    for i in range(len(boxes)):
        if i in indexes:
            x, y, w, h = boxes[i]
            label = str(classes[class_ids[i]])

            
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.putText(frame, label, (x, y + 30), cv2.FONT_HERSHEY_PLAIN, 3, (0, 255, 0), 3)

    cv2.imshow("Image", frame)
    key = cv2.waitKey(1)
    if key == 27:
        break

cap.release()
cv2.destroyAllWindows()
