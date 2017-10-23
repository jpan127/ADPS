
# coding: utf-8

# In[1]:


import glob
import sys
from scipy import ndimage
from PIL import Image
import os, os.path
import numpy as np
import cv2
import math

import matplotlib.pyplot as plt


# In[2]:


# Import images
image_list = []
image_data = []
for filename in glob.glob('Sidewalk/*.JPG'):
    im=Image.open(filename)
    imcv=cv2.imread(filename)
    image_list.append(imcv)
    image_data.append(list(im.getdata()));


# In[3]:


# color regions
HSV_data = []
for image in image_list:
    HSV = cv2.cvtColor(image, cv2.COLOR_RGB2HSV)
    HSV_data.append(HSV)
#     HLS = cv2.cvtColor(image, cv2.COLOR_RGB2HLS)
#     GRAY = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
#     cv2.imshow("Images", np.hstack([HSV, HLS]))
#     cv2.imshow("Original", image)
#     cv2.waitKey(0)


# HSV would be helpful for distinguishing between the road, sidewalk, and other areas, so we'll continue with that when it comes to making a mask.
# 
# Let's continue by blurring the image to see if that helps define the lines between the road and sidewalk. Almost seems counterintuitive.

# In[4]:


median_data = []

for image in image_list:
    BLUR = cv2.GaussianBlur(image, (3, 3), 0)
    MEDIAN = cv2.medianBlur(image,5)
    MEDIAN = cv2.medianBlur(MEDIAN,5)
    median_data.append(MEDIAN)
#     cv2.imshow("Blurred", BLUR)
#     cv2.imshow("Median", MEDIAN)
#     cv2.waitKey(0)


# Blurring the image seems to do the trick. It keeps the colors the same while reducing noise. This lets us eliminate the moss on the sidewalk and keeps the lines remain well defined.
# 
# One possible problem would be a clean sidewalk with the sun directly overhead. This would result in the sidewalk lines dissapearing. The road-sidewalk distinction would remain the same.
# 
# At this point we've got our data. Let's try gray scale on this to see what happens.

# In[5]:


# for image in median_data:
#     GRAY = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
#     cv2.imshow("GRAY", GRAY)
#     cv2.waitKey(0)


# Not much unfortunately. At this point, let's try eliminating all non gray colors.
# 
# First begin by defining the range of colors.

# In[6]:


def select_sidewalk(image):
    # sidewalk color mask using HSV scale
    lower = np.uint8([  0, 0,   140])
    upper = np.uint8([179, 60, 200])
    mask = cv2.inRange(image, lower, upper)
    return cv2.bitwise_and(image, image, mask = mask)

def select_sidewalk_dynamically(image):
    # Get ROI:
    height, width, channels = image.shape
    ROI = [height-100, height, round((width/2)-80), round((width/2)+80)]
    im1 = image[ROI[0]:ROI[1], ROI[2]:ROI[3]]
    # Determine color range
    lower = np.array([im1[:,:,0].min(), im1[:,:,1].min(), im1[:,:,2].min()])
    upper = np.array([im1[:,:,0].max(), im1[:,:,1].max(), im1[:,:,2].max()])
    # sidewalk color mask using HSV scale
    mask = cv2.inRange(image, lower, upper)
    mask2 = cv2.inRange(image, lower, upper)
    return cv2.bitwise_and(image, image, mask = mask)


# ... and test it out on a single image.

# In[7]:


# mask_test = select_sidewalk(HSV_data[0])
# cv2.imshow("MASK", mask_test)
# cv2.imshow("HSV", HSV_data[0])
# cv2.imshow("Original", image_list[0])
# cv2.waitKey(0)


# It appears to work! But there are some gaps where the sidewalk is dirty, or it's too far and increases in saturation (warmth at vanishing points something something).
# 
# So let's go ahead and use the blurred image, change the color space, and apply the filter.

# In[8]:


masked_images = []

# for image in median_data:
#     mask_test = select_sidewalk(cv2.cvtColor(image, cv2.COLOR_RGB2HSV))
#     masked_images.append(mask_test)
#     cv2.imshow("MASK", mask_test)
#     cv2.imshow("Blurred", image)
#     cv2.waitKey(0)


# This looks great. The sidewalk and road are selected, with more of the sidewalk being selected than the road. Let's do this dynamically by selecting a ROI (region of interest) in the bottom-middle of the image. We'll extract the color ranges and then interpolate an HSV color range for the filter.
# 
# First up, capturing the ROI:

# In[9]:


# height, width, channels = median_data[0].shape
# ROI = [height-100, height, round((width/2)-80), round((width/2)+80)]
# im1 = median_data[0][ROI[0]:ROI[1], ROI[2]:ROI[3]]
# cv2.imshow("CROP", im1)
# cv2.imshow("Blurred", median_data[0])
# cv2.waitKey(0)
# cv2.destroyAllWindows()
# print(height, width, channels)
# print(ROI[0], ROI[1], ROI[2], ROI[3])


# Now let's determine the HSV color ranges using a rudimentary min/max function on the color ranges:

# In[10]:


# lower = np.array([im1[:,:,0].min(), im1[:,:,1].min(), im1[:,:,2].min()])
# upper = np.array([im1[:,:,0].max(), im1[:,:,1].max(), im1[:,:,2].max()])
# print(lower, upper)


# And now test it to filter the blurred sidewalk image:

# In[11]:


for image in median_data:
    mask_test = select_sidewalk_dynamically(cv2.cvtColor(image, cv2.COLOR_RGB2HSV))
    masked_images.append(mask_test)
#     cv2.imshow("MASK", mask_test)
#     cv2.imshow("Blurred", image)
#     cv2.waitKey(0)


# Definitely an improvement, but there are some issues...
# 
# 1. If there's some debris in the ROI, it's anyones guess what's going to be selected.
#     1. Debris can be grass, leaves, anything.
# 2. If the ROI is not directly on the sidewalk something else is going to be selected.
#     1. An example of this would be a curve, or a tree planted on the sidewalk.
#     2. In such a scenario a larger color range is allowed.
# 
# Solutions are needed for each issue.
# 
# 1. Use statistics to eliminate the debris. A leaf or debris will show up as an outlier in the image dataset, and can be eliminated. Currently, only min/max is being used. That's a rudimentary algorithm at best.
# 2. Use legacy data, or train the robot to only search for specific colors.
#     1. We don't want to travel on dirt, so don't look for brown.
#     2. Same for grass.
#     3. Possibly make the algorithm more robust by allowing specific colors for initial detection, then extend once a possible path has been found.
#     
# For now, let's try to apply edge detection.

# In[12]:


def color_fill(im_in):
    # Threshold.
    # Set values equal to or above 220 to 0.
    # Set values below 220 to 255.
    th, im_th = cv2.threshold(im_in, 0, 255, cv2.THRESH_BINARY);

    # Copy the thresholded image.
    im_floodfill = im_th.copy()

    # Mask used to flood filling.
    # Notice the size needs to be 2 pixels than the image.
    h, w = im_th.shape[:2]
    mask = np.zeros((h+2, w+2), np.uint8)

    # Floodfill from point (0, 0)
    cv2.floodFill(im_floodfill, mask, (0,0), 255);

    # Invert floodfilled image
    im_floodfill_inv = cv2.bitwise_not(im_floodfill)

    # Combine the two images to get the foreground.
    im_out = im_th | im_floodfill_inv
    return im_out


# In[13]:


kernel_size=5
for image in masked_images:
    image = cv2.medianBlur(image,5)
    binarized = color_fill(image)
    edges = cv2.Canny(image,50,150,apertureSize = 3)
    lines = cv2.HoughLinesP(edges,1,np.pi/180,75,150)
    
    # displays line segments
    a,b,c = lines.shape
    for i in range(a):
        cv2.line(binarized, (lines[i][0][0], lines[i][0][1]), (lines[i][0][2], lines[i][0][3]), (0, 0, 255), 3, cv2.LINE_AA)
    
    lines = cv2.HoughLines(edges,1,np.pi/180,75,150)
    a,b,c = lines.shape
    for i in range(a):
        rho = lines[i][0][0]
        theta = lines[i][0][1]
        a = math.cos(theta)
        b = math.sin(theta)
        x0, y0 = a*rho, b*rho
        pt1 = ( int(x0+1000*(-b)), int(y0+1000*(a)) )
        pt2 = ( int(x0-1000*(-b)), int(y0-1000*(a)) )
        cv2.line(image, pt1, pt2, (0, 0, 255), 2, cv2.LINE_AA)
        
    cv2.imshow("Original", image)
    cv2.imshow("Binarized", binarized)
    cv2.imshow("im1", edges)
#     cv2.imshow("im2", im2)
#     cv2.imshow("im3", im3)
    cv2.waitKey(0)

