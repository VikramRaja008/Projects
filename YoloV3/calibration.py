import numpy as np
import cv2

# termination criteria
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
objp = np.zeros((6*7,3), np.float32)
objp[:,:2] = np.mgrid[0:7,0:6].T.reshape(-1,2)

# Arrays to store object points and image points from all the images.
objpoints = [] # 3d points in real world space
imgpoints = [] # 2d points in image plane.

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to capture frame")
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Find the chess board corners
    ret, corners = cv2.findChessboardCorners(gray, (7,6), None)

    # If found, add object points, image points (after refining them)
    if ret == True:
        objpoints.append(objp)

        corners2 = cv2.cornerSubPix(gray, corners, (11,11), (-1,-1), criteria)
        imgpoints.append(corners2)

        # Indicate that the image is being captured
        print("Image captured for calibration")

        # Draw and display the corners
        cv2.drawChessboardCorners(frame, (7,6), corners2, ret)
        cv2.imshow('img', frame)

        # Wait for 'q' key to stop the program
        while True:
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
    else:
        cv2.imshow('img', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

cap.release()
cv2.destroyAllWindows()

if len(objpoints) > 0 and len(imgpoints) > 0: # make sure we have some calibration data
    ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)

    print(f'Camera matrix : \n{mtx}\n')

    print(f'Focal length : {mtx[0, 0], mtx[1, 1]}')

    print(f'Principal point : {mtx[0, 2], mtx[1, 2]}')
else:
    print("Not enough calibration data gathered.")
