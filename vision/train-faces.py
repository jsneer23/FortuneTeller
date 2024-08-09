import os
os.environ['OPENBLAS_NUM_THREADS'] = '1'
os.environ['OPENBLAS_MAIN_FREE'] = '1'
import cv2
import numpy as np

def prepare_training_data(data_folder_path):
 
    dirs = os.listdir(data_folder_path)

    #lists to hold all subject faces and labels
    faces = []
    labels = []

    for label, dir_name in enumerate(dirs):

        if dir_name.startswith('.'):
            continue

        # get image names from training data
        subject_dir_path = data_folder_path + "/" + dir_name
        subject_images_names = os.listdir(subject_dir_path)
    
        for image_name in subject_images_names:
    
            #ensure file is png
            if not image_name.endswith(".png"):
                continue

    
            #build image path
            #sample image path = training-data/s1/1.pgm
            image_path = subject_dir_path + "/" + image_name

            #read image
            image = cv2.imread(image_path, 0)
            image = cv2.resize(image, (200, 200)) 

            faces.append(image)
            labels.append(label)
    
    return faces, labels

read_path = os.path.join(os.getcwd(), "vision/training-data")
faces, labels = prepare_training_data(read_path)
face_recognizer = cv2.face.LBPHFaceRecognizer.create()
face_recognizer.train(faces, np.array(labels))


