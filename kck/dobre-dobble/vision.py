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

from cvtools import *
from train import *

def dobble_features(cutout):
    cutout = resizeAndPad(cutout, (40, 40))
    return np.hstack((cutout.reshape(-1)))

def clean_circle_mask(x, fill=True, erode=8, dilate=5):
    if fill: x = ndi.binary_fill_holes(x)
    for _ in range(erode): x = erosion(x)
    for _ in range(dilate): x = dilation(x)
    return x

def clean_dobble_mask(x, fill=True, remove=True, dilate=1, erode=1):
    for _ in range(dilate): x = dilation(x)
    for _ in range(erode): x = erosion(x)
    if fill: x = ndi.binary_fill_holes(x)
    if remove: x = remove_small_objects(x)
    x, _ = ndi.label(x)
    #x = convex_hull_object(x)
    return ski2cv(x)

def filter_circles(mask, contours):
    out = np.zeros_like(mask, dtype=np.uint8)
    obj = []
    for i, c in enumerate(contours):
        # not an ellipse
        if len(c) < 5: continue
        p = contour_props(c)
        if p is None: continue
        if p['area'] < 10000: continue
        # not circly enough or too small
        if p['roundness'] < 0.6 or p['radius'] < 30: continue
        ellipse = cv.fitEllipse(c)
        ax1, ax2 = ellipse[1]
        # too squishy
        #if abs(ax1 / ax2) < 0.6: continue
        obj.append(dict(contour=c, props=p, ellipse=ellipse))
        cv.ellipse(out, ellipse, WHITE, -1)
        #cv.drawContours(out, [c], 0, WHITE, -1)
    return out, obj

def filter_dobbles(img, contours, circles):
    out = np.zeros_like(img, dtype=np.uint8)
    obj = []
    for i, x in enumerate(contours):
        p = contour_props(x)
        if p is None: continue
        xo = p['origin']
        # no circles on image
        if not circles: continue
        # closest circle
        ci = sorted(range(len(circles)), key=lambda i: dist(xo, circles[i]['props']['origin']))[0]
        co = circles[ci]['props']['origin']
        cr = circles[ci]['props']['radius']
        cutout = trimmed_cutout(contour_mask(img, x), x)
        # TODO: check if inside ellipse, not circle
        hsv = bgr2hsv(cutout)
        all = count_in_range(hsv, (0, 0, 1), (360/2, 255, 255))
        grays = count_in_range(hsv, (0, 0, 0.3*255), (360/2, 0.2*255, 255)) / all
        purple = count_in_range(hsv, (260/2, 0.1*255, 0.4*255), (360/2, 255, 255)) / all
        black = count_in_range(hsv, (0, 0, 1), (360/2, 255, 0.3*255)) / all
        saturated = count_in_range(hsv, (0, 0.2*255, 0.4*255), (360/2, 255, 0.7*255)) / all
        #print(f'{i:2d} wht {grays:.2f}')
        if grays > .9 and purple < .1 and black < .2 and saturated < .1: continue
        #if p['area'] < 200: continue
        if dist(xo, co) > cr: continue
        obj.append(dict(contour=x, cutout=cutout, props=p, circle=ci))
        cv.drawContours(out, [x], 0, WHITE, -1)
        putText2(out, str(i), p['origin'], 1.3, 3)
    return out, obj

def basic_circle_mask(input):
    hsv = bgr2hsv(input)
    msk = cv.inRange(hsv, (260/2, 0.1*255, 0.4*255), (360/2, 255, 255)) # purples
    msk += cv.inRange(hsv, (30/2, 0.3*255, 0), (80/2, 255, 0.9*255)) # yellows
    msk += cv.inRange(hsv, (0, 0, 0), (360/2, 255, 0.2*255)) # blacks
    # saturated colors
    window = 0.5
    for s in np.arange(0.4, 1-window, 0.1):
        msk += cv.inRange(hsv, (0, s*255, 0), (360/2, 255, (s+window)*255))
    msk = ski2cv(msk > 0)
    return invert(msk)

def extract_metadata(X):
    img = src = shrink(cvimread(X), side=1000, interpolation=cv.INTER_NEAREST) if isinstance(X, str) else X
    #img = cv.medianBlur(img, 5)
    #img = adjust_gamma(img, 2)
    cs1 = img
    
    k = 10
    cs1 = cv.bilateralFilter(cs1, k, k*2, k/2)
    lab = bgr2lab(cs1)
    #lab[:,:,0] = cv.equalizeHist(lab[:,:,0])
    lab[:,:,0] = equalize_adapthist(lab[:,:,0], clip_limit=1, tile_size=10)
    cs1 = lab2bgr(lab)
    
    cs2 = bgr2gray(cs1)
    cs2 = erosion_dilation(cs2, 1, 1)
        
    cs3 = basic_circle_mask(cs1)
    
    cs4 = circle_mask = cv.adaptiveThreshold(cs2, 255, cv.ADAPTIVE_THRESH_GAUSSIAN_C, cv.THRESH_BINARY, 51, 0)
        
    cs5 = cs4 * cs3
    cs5 = erosion_dilation(cs5, 1, 1)
    
    cs6 = clean_circle_mask(cs5, fill=True, erode=12, dilate=8)
    
    circle_cont, _ = cv.findContours(ski2cv(cs6), cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)
    circle_mask, circles = filter_circles(circle_mask, circle_cont)
    for _ in range(2): circle_mask = erosion(circle_mask)

    ds1 = equalize_adapthist_lab(img, clip_limit=3, tile_size=4)
    ds1 = cv.bilateralFilter(img, 5, 80, 30)
    #ds1 = cs1
    ds2 = bgr2gray(ds1)
    #ds2 = cs2
    ds2 = erosion_dilation(ds2, 3, 2)
    
    ds3 = cv.adaptiveThreshold(ds2, 255, cv.ADAPTIVE_THRESH_GAUSSIAN_C, cv.THRESH_BINARY_INV, 81, 0)
    ds4 = ds3 * circle_mask
    ds5 = clean_dobble_mask(ds4, dilate=1, erode=1)
    dobble_cont, _ = cv.findContours(ds5, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)
    dobble_mask, dobbles = filter_dobbles(img, dobble_cont, circles)
    dobble_mask = clean_dobble_mask(dobble_mask, dilate=1, erode=1)
    #dobble_mask = erosion_dilation(dobble_mask, 1, 1)
    
    steps = [src, cs1, cs2, cs3, cs4, cs5, cs6, 
             circle_mask, ds1, ds2, ds3, ds4, ds5, dobble_mask]
    return circles, dobbles, steps

def show_process(path):
    circles, dobbles, steps = extract_metadata(path)
    print(f'found {len(circles)} circles, {len(dobbles)} dobbles')
    imgs = [cv2ski(x) for x in steps[:2]] + steps[2:]
    titles = ['original', 'filtered', 'gray', 'color thresh', 'adapt thresh', 'final thresh', 'cleaned', 'selected-ellipses',
              'filtered', 'mono+morph', 'adapt-thresh', 'masked', 'cleaned', 'selected']
    imshow_grid(imgs, 6, 3, titles=titles, figsize=(12, 22), pad=.05, cmap='gray')
    return plt.gcf()

def build_hierarchy(circles, dobbles):
    h = {j: [] for j in range(len(circles))}
    for i, x in enumerate(dobbles):
        j = x['circle']
        h[j].append(i)
    return h

def solve_dobble(path, model, outlines=True, connections=True, text=True):
    circles, dobbles, steps = extract_metadata(path)
    X, Y = None, None
    if dobbles:
        #X = np.array([dobble_features(x['cutout']) for x in dobbles]).reshape((len(dobbles), -1))
        X = np.array([dobble_features(x['cutout']) for x in dobbles]).reshape((-1, 40, 40, 3))
        Y = model.predict(X)
        Y = enc.inverse_transform(Y).reshape(-1)

    out = steps[0].copy()
    for i, x in enumerate(circles):
        if outlines: cv.ellipse(out, x['ellipse'], RCOLORS[i], 2)

    for i, x in enumerate(dobbles):
        ox, oy = x['props']['origin']
        color = RCOLORS[x['circle']]
        label = Y[i]
        dobbles[i]['label'] = label

        if label == '_spark': continue
        if outlines: cv.drawContours(out, [x['contour']], 0, color, 2)
        #cv.rectangle(out, (ox-1, oy-1), (ox+1, oy+1), color, 8)
        cv.circle(out, (ox, oy), 6, color, 3)

    hier = build_hierarchy(circles, dobbles)
    for c1, c2 in combinations(hier, 2):
        for i in hier[c1]:
            u = dobbles[i]
            if u['label'] == '_spark': continue
            for j in hier[c2]:
                v = dobbles[j]
                if u['label'] == v['label'] and u['label'] is not None:
                    ou, ov = u['props']['origin'], v['props']['origin']
                    if connections: cv.line(out, ou, ov, CYAN, 2)

    for i, x in enumerate(dobbles):
        if text: putText2(out, x['label'], x['props']['origin'], 0.7, 2)
    
    return out

def export_cutouts_supervised(cutouts):
    X = np.array([list(x) for x in cutouts.cutout]).reshape((-1, 40, 40, 3))
    cutouts['Y'] = predict_cnn(model, X)
    for _, cutout, _, _, source, _, label in tqdm(cutouts.itertuples()):
        cv.imwrite(f'res/classes-auto/{label}/{source}', cutout)
        
def export_cutouts(input_path, export_path):
    from os.path import join
    paths = glob(join(input_path, '*.jpg'))
    for path in tqdm(paths):
        basename = path.split('/')[-1].split('.')[0]
        _, dobbles, _ = extract_metadata(path)
        for i, dobble in enumerate(dobbles):
            cutout = resizeAndPad(dobble['cutout'], (40, 40))
            out = join(export_path, f'{basename}-{i}.png')
            cv.imwrite(out, cutout)

def load_cutouts(base_path):
    df = []
    paths = glob(os.path.join(base_path, '*/*.png'))
    for path in tqdm(paths):
        label = path.split('/')[-2]
        name = path.split('/')[-1]
        cutout = cv.imread(path)
        features = dobble_features(cutout)[:46+7]
        df.append(dict(source=name, label=label, features=features, cutout=cutout))
    return pd.DataFrame(df)

def load_cutouts_unlabeled(path_patt):
    df = []
    for path in tqdm(glob(path_patt)):
        name = path.split('/')[-1]
        cutout = cv.imread(path)
        features = dobble_features(cutout)
        df.append(dict(source=name, features=features, cutout=cutout))
    return pd.DataFrame(df)
        
def make_cutouts(base_path):
    df = []
    paths = glob(os.path.join(base_path, '*.jpg'))
    for path in tqdm(paths):
        _, dobbles, _ = extract_metadata(path)
        for i, dobble in enumerate(dobbles):
            cutout = resizeAndPad(dobble['cutout'], (40, 40))
            features = dobble_features(dobble['cutout'])[:46+7]
            df.append(dict(source=path, features=features, cutout=cutout))
    return pd.DataFrame(df)

def plot_clusters(df):
    clusters = [[x, cutout_grid(df[df.label == x].cutout)] for x in df.label.unique()]
    clusters.sort(key=lambda x: x[0])
    w, h = rect_size(len(clusters))
    plt.figure(figsize=(13, 13), dpi=100)
    for (i, _, _), (label, img) in enumerate_grid(clusters, (w, h)):
        plt.subplot(w, h, i+1)
        plt.imshow(img)
        plt.title(label)
        plt.axis('off')
    plt.show()
    
def export_solution_all():
    for f in tqdm(glob('res/all/*.jpg')):
        solution = solve_dobble(f, model, outlines=False, text=False)
        imshow_fig(cv2ski(solution))
        plt.savefig('out/clean/'+f.split('/')[-1])
        plt.close()
        
def export_process_all():
    for f in tqdm(glob('res/all/*.jpg')):
        fig = show_process(f)
        fig.savefig('tmp/' + f.split('/')[-1])
        fig.close()