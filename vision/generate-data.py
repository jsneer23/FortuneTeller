# This file will loop through videos stored in the training-videos
# folder and will detect any faces in the videos and use them to
# generate training data for a facial recognition algorithm and 
# will store it in the training-data folder
import os
import cv2

read_path = os.path.join(os.getcwd(), "vision/training-videos")
write_path = os.path.join(os.getcwd(), "vision/training-data")
video_dir = os.fsencode(read_path)

face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')
  
for file in os.listdir(video_dir):

    filename = os.fsdecode(file)

    if filename.endswith(".MOV"):

        filepath = os.path.join(read_path, filename)
        print("Processing image:", filepath)

        capture = cv2.VideoCapture(filepath)
        valid_frame, image = capture.read()
        frame_no = 0

        while(valid_frame):

            if frame_no%30 == 0:

                gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
                faces = face_cascade.detectMultiScale(gray, 1.3, 5)

                for i, (x,y,w,h) in enumerate(faces):
                    roi_gray = gray[y:y+h, x:x+w]

                    img_identifier = "_" + str(frame_no) + "_" + str(i)
                    write_filename = "z_" + filename.split(".")[0] + img_identifier + ".png"
                    cv2.imwrite(os.path.join(write_path, write_filename), roi_gray)

            valid_frame, image = capture.read()
            frame_no += 1

        capture.release()