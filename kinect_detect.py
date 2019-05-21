#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np  
import sys,os  

import rospy
import cv2
from cv_bridge import CvBridge, CvBridgeError
from sensor_msgs.msg import Image

caffe_root = '/home/srobot/caffe/'
sys.path.insert(0, caffe_root + 'python')  
import caffe  

xxx=[0,]

###########################
###########################
# net_file= '/home/srobot/caffe/examples/MobileNet-SSD-master/example/MobileNetSSD_deploy.prototxt'  
# caffe_model='/home/srobot/caffe/examples/MobileNet-SSD-master/the_seventh_training.caffemodel'
net_file= '/home/srobot/caffe/examples/MobileNet-SSD-master/deploy.prototxt'  
caffe_model='/home/srobot/caffe/examples/MobileNet-SSD-master/MobileNetSSD_deploy.caffemodel'

if not os.path.exists(caffe_model):
    print("MobileNetSSD_deploy.caffemodel does not exist,")
    print("use merge_bn.py to generate it.")
    exit()
net = caffe.Net(net_file,caffe_model,caffe.TEST)  

CLASSES = ('background',
           'aeroplane', 'bicycle', 'bird', 'boat',
           'bottle', 'bus', 'car', 'cat', 'chair',
           'cow', 'diningtable', 'dog', 'horse',
           'motorbike', 'person', 'pottedplant',
           'sheep', 'sofa', 'train', 'tvmonitor')
#####change warnning
#CLASSES = ('background',
#           'coke', 'potato_chips', 'xue_bi', 'water',
#           'pie', 'shu_fu_jia', 'noodles','ao_li_ao',
#           'dou_nai','juice',)


def preprocess(src):
    img = cv2.resize(src, (300,300))
    img = img - 127.5
    img = img * 0.007843
    return img

def postprocess(img, out):   
    h = img.shape[0]
    w = img.shape[1]
    box = out['detection_out'][0,0,:,3:7] * np.array([w, h, w, h])

    cls = out['detection_out'][0,0,:,1]
    conf = out['detection_out'][0,0,:,2]
    return (box.astype(np.int32), conf, cls)

def detect(img1,xxx):
    origimg=img1
    img = preprocess(origimg)
    
    img = img.astype(np.float32)
    img = img.transpose((2, 0, 1))

    net.blobs['data'].data[...] = img
    out = net.forward()  
    box, conf, cls = postprocess(origimg, out)

    for i in range(len(box)):
       p1 = (box[i][0], box[i][1])
       p2 = (box[i][2], box[i][3])
       cv2.rectangle(origimg, p1, p2, (0,255,0))
       p3 = (max(p1[0], 15), max(p1[1], 15))
       title = "%s:%.2f" % (CLASSES[int(cls[i])], conf[i])
       cv2.putText(origimg, title, p3, cv2.FONT_ITALIC, 0.6, (0, 255, 0), 1)

    # cv2.imshow("SSD", origimg) 
    # cv2.waitKey(3)
    
    name="/home/srobot/caffe/examples/MobileNet-SSD-master/output.jpg"
    cv2.imwrite(name,origimg)
    xxx[0]=1

class image_converter:
    def __init__(self):    
        # 创建cv_bridge，声明图像的发布者和订阅者
        self.image_pub = rospy.Publisher("cv_bridge_image", Image, queue_size=1)
        self.bridge = CvBridge()
        self.image_sub = rospy.Subscriber("/kinect2/qhd/image_color", Image, self.callback)

    def callback(self,data):
        # 使用cv_bridge将ROS的图像数据转换成OpenCV的图像格式
        try:
            cv_image = self.bridge.imgmsg_to_cv2(data, "bgr8")
        except CvBridgeError as e:
            print e

        # 显示Opencv格式的图像
        if xxx[0]==0:
            detect(cv_image,xxx)
        else:
            exit()

        
if __name__ == '__main__':
    try:
        # 初始化ros节点
        rospy.init_node("cv_bridge_test")
        rospy.loginfo("Starting cv_bridge_test node")
        image_converter()
        rospy.spin()
    except KeyboardInterrupt:
        print "Shutting down cv_bridge_test node."
        cv2.destroyAllWindows()
