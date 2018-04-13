import sys, copy, math, random
import collections
import numpy as np
import pandas as pd
import pygame as pg
import pygame.gfxdraw


Range = collections.namedtuple('Range', 'start end')
Vec = collections.namedtuple('Vec', 'x y')
S = 2
O = 20

def distance(u, v):
    return math.sqrt((u.x - v.x)**2 + (u.y - v.y)**2)

def add(u, v):
    return Vec(u.x + v.x, u.y + v.y)

def had(u, v):
    return Vec(u.x * v.x, u.y * v.y)

def mul(u, a):
    return Vec(u.x * a, u.y * a)

def random_color():
    return random.randint(0, 0xFFFFFF)
    #return 0x0

def random_momentum(P):
    return Vec(random.uniform(-P, P), random.uniform(-P, P))


def next_momentum(p, r, w, h):
    if r.y >= h or r.y <= 0:
        return Vec(p.x, -p.y)
    if r.x >= w or r.x <= 0:
        return Vec(-p.x, p.y)
    return p

def clip_position(p, r, w, h):
    rx, ry = r
    # clip position
    if ry > h:
        ry = h
    if ry < 0:
        ry = 0
    if rx > w:
        rx = w
    if rx < 0:
        rx = 0
    return Vec(rx, ry)

class Atom:
    def __init__(self, r, p, R):
        self.r, self.p, self.R = r, p, R
        self.color = random_color()
        self.radius = 2

    def draw(self, screen):
        rx, ry = int(self.r.x), int(self.r.y)
        s = self.radius
        #pg.draw.rect(screen, self.color, [rx * S + O, ry * S + O, s * S, s * S])
        pg.draw.circle(screen, self.color, [rx * S + O, ry * S + O], s * S)

class World:
    def __init__(self, r):
        # ilość cząsteczek
        self.N = 2**r
        # |max pęd|
        self.P = r + (1 - r % 2)
        # |max x/y|
        self.R = 2*r + 1
        # atomy
        self.a = []
        # czas
        self.t = 0.0
        self.w = 350
        self.h = 350

        for i in range(self.N):
            r_i = Vec(0, i*self.h / self.N)
            p_i = random_momentum(self.P)
            m_i = 1
            self.a.append(Atom(r_i, p_i, m_i))

    def update(self, dt):
        self.t += dt
        for a in self.a:
            a.p = next_momentum(a.p, a.r, self.w, self.h)
            a.r = clip_position(a.p, a.r, self.w, self.h)
            a.r = add(a.r, mul(a.p, dt))
            a.r = clip_position(a.p, a.r, self.w, self.h)

    def draw(self, screen):
        pg.draw.rect(screen, 0xFFFFFF, [O, O, self.w * S, self.h * S])
        for a in self.a:
            a.draw(screen)

def main():
    screen = pg.display.set_mode([1000, 800])
    # hiperparametr r
    r = 11
    w = World(r)
    # szybkość symulacji
    u = 2
    # krok czasowy
    dt = u / w.P
    
    while True:
        screen.fill(0xCCCCCC)
        w.update(dt)
        w.draw(screen)
        pg.display.flip()

if __name__ == '__main__':
    main()
