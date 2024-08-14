import os
import cv2

read_path = os.path.join(os.getcwd(), "vision/trained-models")
read_filename = "LBPHrecognizer"

face_recognizer = cv2.face.LBPHFaceRecognizer.create()
face_recognizer.read(read_path + "/" +read_filename)

face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

if __name__ == "__main__":

    capture = cv2.VideoCapture(0)

    if not capture.isOpened():
        print("Camera not opened")
    else:
        valid_frame, image = capture.read()
        label = None

        while(valid_frame):
            gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
            faces = face_cascade.detectMultiScale(gray, 1.3, 5)

            for i, (x,y,w,h) in enumerate(faces):
                processed_image = cv2.resize(gray[y:y+h, x:x+w], (200, 200))
                label, confidence = face_recognizer.predict(processed_image)
                
                if confidence > 0.75:
                    cv2.rectangle(image,(y,x), (y+h, x+w),color=(0,255,0),thickness=2)
                    cv2.putText(image,label,(y+10,x+30),cv2.FONT_HERSHEY_COMPLEX,1,(0,255,0),2)
                    cv2.putText(image,str(round(confidence*100,2)),(y+200,x+30), cv2.FONT_HERSHEY_COMPLEX,1,(0,255,0),2)

            cv2.imshow("Output", image)
            key = cv2.waitKey(1) & 0xFF

	        # quit when 'q' key is pressed
            if key == ord("q"):
                break

            valid_frame, image = capture.read()


        capture.release()
        cv2.destroyAllWindows()

