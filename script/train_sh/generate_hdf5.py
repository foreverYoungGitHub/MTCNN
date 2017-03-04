import numpy as np
import cv2
import h5py
import sys
#caffe_root = '/Users/xxx/caffe/'
#sys.path.insert(0, caffe_root + 'python')
import caffe

min_img_size = 24

label_path = '../dataset/label.txt'
landmark_path = '../dataset/landmark.txt'
regression_box_path = '../dataset/regression_box.txt'
crop_image_path = '../dataset/crop_image.txt'
train_file_path = '../dataset/train_24.hd5'

label = np.loadtxt(label_path, int)
landmark = np.loadtxt(landmark_path, float)
regression_box = np.loadtxt(regression_box_path, float)

label = np.transpose([label])
#landmark = np.transpose(landmark)
labels = np.concatenate((label, regression_box, landmark), axis = 1)


img_array = []
for line in open(crop_image_path):
    img = cv2.imread(line.strip())
    img = cv2.resize(img, (min_img_size,min_img_size))
    #img = cv2.convertTo(img, cv2.CV_32FC3, 0.0078125,-127.5*0.0078125)
    img = cv2.transpose(img)
    img_forward = np.array(img, dtype=np.float32)
    img_forward = np.transpose(img_forward, (2, 0, 1))
    img_forward = (img_forward - 127.5) * 0.0078125
    img_array.append(img_forward)

a = np.array(img_array, dtype=np.float32)


with h5py.File(train_file_path, 'w') as f:
    f['data'] = a
    f['labels'] = labels
    f['regression'] = regression_box
    f['landmark'] = landmark
