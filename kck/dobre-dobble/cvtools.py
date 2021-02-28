import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import skimage as ski
import skimage.io
import cv2 as cv
import math
import os
import cv2
import random
import matplotlib

from skimage.io import imshow
from math import pi, floor, ceil
from numpy import sqrt
from pprint import pprint
from random import randint
from itertools import combinations 
from sklearn.metrics.pairwise import cosine_similarity

import scipy.ndimage as ndi
import skimage.draw as draw
from skimage.util import invert
from skimage.feature import canny, peak_local_max
from skimage.filters import rank, sobel, threshold_otsu, threshold_local, threshold_minimum, gaussian
from skimage.morphology import disk, erosion, dilation, watershed, remove_small_holes, remove_small_objects, convex_hull_object, area_opening
from skimage.transform import hough_circle, hough_circle_peaks
from skimage.exposure import adjust_gamma, rescale_intensity, equalize_hist
from skimage.io import imshow as skimshow


BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (0, 0, 255)
GREEN = (0, 255, 0)
BLUE = (255, 0, 0)
CYAN = (255, 255, 0)
MAGENTA = (255, 0, 255)
YELLOW = (0, 255, 255)

CMAP = matplotlib.cm.get_cmap('hsv')
COLORS = [CMAP(x) for x in np.linspace(0, 1, 10)]
COLORS = [(int(b*255), int(g*255), int(r*255)) for (r, g, b, _) in COLORS]
RCOLORS = COLORS.copy()
random.shuffle(RCOLORS)

def bgr2hsv(x): return cv.cvtColor(x, cv.COLOR_BGR2HSV)
def bgr2lab(x): return cv.cvtColor(x, cv.COLOR_BGR2LAB)
def bgr2rgb(x): return cv.cvtColor(x, cv.COLOR_BGR2RGB)
def bgr2gray(x): return cv.cvtColor(x, cv.COLOR_BGR2GRAY)

def hsv2bgr(x): return cv.cvtColor(x, cv.COLOR_HSV2BGR)
def hsv2rgb(x): return bgr2rgb(hsv2bgr(x))
def hsv2lab(x): return bgr2lab(hsv2bgr(x))
def hsv2gray(x): return bgr2gray(hsv2bgr(x))

def lab2bgr(x): return cv.cvtColor(x, cv.COLOR_LAB2BGR)
def lab2rgb(x): return bgr2rgb(lab2bgr(x))
def lab2hsv(x): return bgr2lab(lab2hsv(x))
def lab2gray(x): return bgr2gray(lab2bgr(x))

def rgb2bgr(x): return cv.cvtColor(x, cv.COLOR_RGB2BGR)
def rgb2hsv(x): return bgr2hsv(rgb2bgr(x))
def rgb2lab(x): return bgr2lab(rgb2bgr(x))
def rgb2gray(x): return bgr2gray(rgb2bgr(x))

class cvwindow:
    def __init__(self, img, title="opencv"):
        cv.namedWindow(title, cv.WINDOW_AUTOSIZE)
        self.out = img.copy()
    def __enter__(self):
        return self.out
    def __exit__(self, type, value, traceback):
        cv.waitKey(0)
        cv.destroyAllWindows()
        
def imshow_bgr(x, **kwargs):
    imshow(cv.cvtColor(x, cv.COLOR_BGR2RGB), **kwargs)
    
def rect_size(area, width=None, height=None):
    n, w, h = area, width, height
    side = sqrt(n)
    if w is None and h is None: w, h = ceil(side), ceil(side)
    elif w is None: w = ceil(n / h)
    elif h is None: h = ceil(n / w)
    if w*h < n: raise Exception(f'rectangle of size {w}x{h} cannot fit {n} elements')
    return w, h
    
def imshow_grid(xs, rows=None, cols=None, titles=None, pad=0, figsize=(10, 10), dpi=100, **kwargs):
    plt.figure(figsize=figsize, dpi=dpi)
    for i, x in enumerate(xs):
        plt.subplot(rows, cols, i+1)
        plt.imshow(x, aspect='auto', **kwargs)
        plt.axis('off')
        plt.axis('tight')
        plt.axis('image')
        if titles: plt.title(titles[i])
    wspace, hspace = pad, pad
    if titles: hspace += 0.2
    plt.subplots_adjust(wspace=wspace, hspace=hspace)
    return plt.gcf()
    
def enumerate_grid(xs, size):
    cols, rows = size
    for i in range(len(xs)):
        y = i % cols
        x = i // cols
        yield (i, x, y), xs[i]
    
def imshow_fig(img, figsize=10, dpi=100.0):
    w, h = img.shape[0], img.shape[1]
    plt.figure(figsize=(figsize, figsize), dpi=dpi)
    plt.imshow(img)
    plt.axis('off')
    return plt.gcf()
    
def cvimread(path):
    if not os.path.exists(path):
        raise Exception(f'file {path} does not exist!')
    return cv.imread(path)

def cvimshow(x, title="opencv"):
    cv.imshow(title, x)

def ski2cv(x):
    return (x * 255).astype(np.uint8)

def cv2ski(x):
    return cv.cvtColor(x, cv.COLOR_BGR2RGB)

def norm(x):
    return (x - x.min()) / (x.max() - x.min())

def dist(a, b):
    (ax, ay), (bx, by) = a, b
    return np.sqrt((ax - bx)**2 + (ay - by)**2)

def is_gray(x):
    return len(x.shape) == 2 or x.shape[2] == 1

def center(a, b):
    (x1, y1), (x2, y2) = a, b
    return abs(x2 - x1) // 2, abs(y2 - y1) // 2

def eccentricity(M):
    u = (M['m20'] - M['m02'])**2
    return (u - 4*M['m11']**2) / u

def centroid(M):
    x = int(M['m10'] / M['m00'])
    y = int(M['m01'] / M['m00'])
    return x, y

def radius(M):
    return sqrt(M['m00'] / pi)

def contour_props(contour):
    M = cv.moments(contour)
    P = cv.arcLength(contour, True)
    A = M['m00']
    if A == 0: return None
    r = sqrt(A / pi)
    R = 4*pi*A / P**2
    hu = cv.HuMoments(M).reshape(-1)
    hu = -np.sign(hu) * np.log10(np.abs(hu))
    p = centroid(M)
    return dict(origin=p, M=M, hu=hu, area=A, radius=r, arc=P, roundness=R)

def contour_mask(out, contour, hull=False):
    mask = np.zeros_like(out)
    if hull:
        cont = cv.convexHull(contours[i])
        #cv.drawContours(mask, [cont], 0, WHITE, -1)
    cv.drawContours(mask, [contour], 0, WHITE, -1)
    return cv.copyTo(out, mask)

def shrink(x, side=400, **kwargs):
    w1, h1, _ = x.shape
    w2, h2 = side, side
    if w1 < h1:
        h2 = w2 * w1 // h1
    else:
        w2 = h2 * h1 // w1
    x = cv.resize(x, (w2, h2), **kwargs)
    return x
    
def converting(x, f, t1, t2):
    abc = cv.cvtColor(x, t1)
    f(abc)
    return cv.cvtColor(abc, t2)

def adjust_gamma(x, gamma=1.0):
    inv = 1.0 / gamma
    table = np.array([(i / 255)**inv * 255 for i in range(0, 256)])
    return cv.LUT(x, table.astype(np.uint8))

def count_in_range(img, a, b):
    return cv.inRange(img, a, b).sum() // 255

def hsv2cv(hsv):
    h, s, v = hsv
    return h // 2, int(s * 255), int(v * 255)

def trimmed_cutout(cutout, contour):
    x, y, w, h = cv.boundingRect(contour)
    return cutout[y:y+h, x:x+w, :]

def putText2(out, text, point, size, weight=1):
    x, y = point
    pad = weight
    cv.putText(out, text, (x-pad, y+pad), cv.FONT_HERSHEY_SIMPLEX, size, WHITE, weight)
    cv.putText(out, text, (x-pad, y-pad), cv.FONT_HERSHEY_SIMPLEX, size, WHITE, weight)
    cv.putText(out, text, (x+pad, y-pad), cv.FONT_HERSHEY_SIMPLEX, size, WHITE, weight)
    cv.putText(out, text, (x+pad, y+pad), cv.FONT_HERSHEY_SIMPLEX, size, WHITE, weight)
    cv.putText(out, text, (x, y), cv.FONT_HERSHEY_SIMPLEX, size, BLACK, weight)

def equalize_adapthist(x, clip_limit=2.0, tile_size=8):
    clahe = cv.createCLAHE(clipLimit=clip_limit, tileGridSize=(tile_size, tile_size))
    return clahe.apply(x)

def equalize_adapthist_cvt(x, channel, t1, t2, **kwargs):
    abc = cv.cvtColor(x, t1)
    abc[:,:,channel] = equalize_adapthist(abc[:,:,channel], **kwargs)
    return cv.cvtColor(abc, t2)

def equalize_adapthist_lab(x, **kwargs):
    return equalize_adapthist_cvt(x, 0, cv.COLOR_BGR2LAB, cv.COLOR_LAB2BGR, **kwargs)

def equalize_adapthist_hsv(x, **kwargs):
    return equalize_adapthist_cvt(x, 0, cv.COLOR_BGR2HSV, cv.COLOR_HSV2BGR, **kwargs)

def cutout_grid(xs, depth=3, size=40):
    if xs.empty: raise Exception("input list must not be empty")
    s = size
    n = len(xs)
    w = int(sqrt(n))
    h = ceil(n / w)
    res = np.zeros((s*w, s*h, depth), dtype=np.uint8)
    for i, data in enumerate(xs):
        assert(size == data.shape[0] == data.shape[1])
        y, x = i % w, i // w
        res[y*s:(y+1)*s, x*s:(x+1)*s, :] = cv2ski(data)
    return res

def resizeAndPad(img, size, padColor=0):
    h, w = img.shape[:2]
    sh, sw = size
    # interpolation method
    if h > sh or w > sw: # shrinking image
        interp = cv2.INTER_AREA
    else: # stretching image
        interp = cv2.INTER_CUBIC
    # aspect ratio of image
    aspect = w/h  # if on Python 2, you might need to cast as a float: float(w)/h
    # compute scaling and pad sizing
    if aspect > 1: # horizontal image
        new_w = sw
        new_h = np.round(new_w/aspect).astype(int)
        pad_vert = (sh-new_h)/2
        pad_top, pad_bot = np.floor(pad_vert).astype(int), np.ceil(pad_vert).astype(int)
        pad_left, pad_right = 0, 0
    elif aspect < 1: # vertical image
        new_h = sh
        new_w = np.round(new_h*aspect).astype(int)
        pad_horz = (sw-new_w)/2
        pad_left, pad_right = np.floor(pad_horz).astype(int), np.ceil(pad_horz).astype(int)
        pad_top, pad_bot = 0, 0
    else: # square image
        new_h, new_w = sh, sw
        pad_left, pad_right, pad_top, pad_bot = 0, 0, 0, 0
    # set pad color
    if len(img.shape) is 3 and not isinstance(padColor, (list, tuple, np.ndarray)): # color image but only one color provided
        padColor = [padColor]*3
    # scale and pad
    scaled_img = cv2.resize(img, (new_w, new_h), interpolation=interp)
    scaled_img = cv2.copyMakeBorder(scaled_img, pad_top, pad_bot, pad_left, pad_right, borderType=cv2.BORDER_CONSTANT, value=padColor)
    return scaled_img

def erosion_dilation(x, n, m):
    for _ in range(n): x = erosion(x)
    for _ in range(m): x = dilation(x)
    return x

def dilation_erosion(x, n, m):
    for _ in range(n): x = dilation(x)
    for _ in range(m): x = erosion(x)
    return x