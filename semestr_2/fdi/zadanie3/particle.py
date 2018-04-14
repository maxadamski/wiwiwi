from random import uniform, randint
from math import log, factorial
import numpy as np
import pygame as pg
import matplotlib as plt
import matplotlib.backends.backend_agg as agg
import pylab


###############################################################################
# Utilities
###############################################################################

def clip(value, min_value, max_value):
    return min(max(value, min_value), max_value)


def product(l):
    v = 1
    for e in l:
        v *= e
    return v


def draw_vector(surface, origin, vector):
    (ox, oy), (dx, dy) = origin, vector
    pg.draw.line(surface, 0xFF0000, origin, (ox+dx, oy+dy), 1)


###############################################################################
# Structures
###############################################################################

class Atom:
    def __init__(self, r, p, color):
        self.r, self.p, self.color = r, p, color

    @property
    def hash(self):
        (rx, ry), (dx, dy) = self.r.astype(int), self.p.astype(int)
        return (rx, ry), (dx, dy)


class World:
    def __init__(self, N, P, R):
        self.size = width, height = np.array([2*R, 2*R])
        self.atoms = []

        for i in range(N):
            r = np.array([0, i * height / N])
            p = np.array([uniform(-P, P), uniform(-P, P)])
            self.atoms += [Atom(r, p, 0)]

    ################
    # Computations

    def update(self, dt):
        X, Y = self.size

        for i, a in enumerate(self.atoms):
            (rx, ry), (px, py) = a.r, a.p
            if ry >= Y or ry <= 0:
                py *= -1
            if rx >= X or rx <= 0:
                px *= -1
            rx, ry = clip(rx, 0, X), clip(ry, 0, Y)
            rx, ry = rx + px*dt, ry + py*dt
            self.atoms[i].r = np.array([rx, ry])
            self.atoms[i].p = np.array([px, py])

        self.groups = {}
        for a in self.atoms:
            hash = a.hash
            if hash not in self.groups:
                self.groups[hash] = 0
            self.groups[hash] += 1

    def gas_probability(self):
        #return factorial(len(self.atoms)) / prod(map(factorial, self.groups.values()))
        return len(self.atoms) / product(self.groups.values())

    def gas_entropy(self):
        return log(self.gas_probability())

    ###########
    # Drawing

    def draw(self, screen, origin, scale=10, draw_momentum=False):
        size, offset, radius = self.size, origin, 4
        pg.draw.rect(screen, 0xFFFFFF, [*offset, *size*scale])
        for atom in self.atoms:
            atom_origin = atom.r * scale + offset
            atom_radius = radius
            #atom_radius = radius * scale / 2
            pg.draw.circle(screen, atom.color,
                           atom_origin.astype(int), int(atom_radius))
            if draw_momentum:
                draw_vector(screen, atom_origin, atom.p * scale)


class LinePlot:
    def __init__(self, size, dpi=80, xlabel='', ylabel='', color='k'):
        self.color, self.size, self.dpi = color, size, dpi
        self.fig = pylab.figure(figsize=size, dpi=dpi)
        self.cav = agg.FigureCanvasAgg(self.fig)
        self.ax = self.fig.gca()
        self.ax.set_xlabel(xlabel)
        self.ax.set_ylabel(ylabel)

    def plot(self, screen, origin, data):
        self.ax.set_xlim(0, len(data))
        self.ax.plot(data, color=self.color)
        self.cav.draw()
        raw_data = self.cav.get_renderer().tostring_rgb()
        surface = pg.image.fromstring(raw_data, self.size * self.dpi, "RGB")
        screen.blit(surface, origin)


###############################################################################
# Simulation
###############################################################################

def main():
    r = 10
    N = 2**r
    P = r + (1 - r % 2)
    R = 2*r + 1
    dt = 1 / (2*P)
    W, H = 800, 1000

    screen = pg.display.set_mode([W, H], pg.DOUBLEBUF)
    plot = LinePlot(np.array([10, 4]), xlabel='time', ylabel='entr')
    world = World(N, P, R)
    scale = 10
    entropy = []
    finish = False

    while not finish:
        for event in pg.event.get():
            if event.type == pg.QUIT:
                finish = True

        world.update(dt)
        entropy += [world.gas_entropy()]

        screen.fill(0)
        world.draw(screen, np.array([40, 360]), scale=scale, draw_momentum=False)
        plot.plot(screen, np.array([0, 0]), entropy)
        pg.display.flip()

    pg.quit()


if __name__ == '__main__':
    main()
