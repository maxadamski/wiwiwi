# Błażej Huminiecki, Dariusz Max Adamski, Hubert Kamieniarz, Konrad Socha, Aleksander Hauziński
# albumy = [136674, 136800, 136716, 136731, 136717]

from random import uniform, randint
from math import log
from scipy.special import factorial
import numpy as np
import pygame as pg
import pygame.gfxdraw
import matplotlib as plt
import matplotlib.backends.backend_agg as agg
import pylab


###############################################################################
# Utilities
###############################################################################

def clip(value, min_value, max_value):
    return min(max(value, min_value), max_value)


###############################################################################
# Structures
###############################################################################

class Atom:
    def __init__(self, r, p, color):
        self.r, self.p, self.color = r, p, color
        self.pycolor = pg.Color(color << 8 | 0xFF)

    @property
    def hash(self):
        (rx, ry), (px, py) = self.r.astype(int), self.p.astype(int)
        return (rx, ry), (px, py)


class World:
    def __init__(self, N, P, R, color=True):
        self.size = width, height = np.array([2*R, 2*R])
        self.atoms = []

        for i in range(N):
            r = np.array([0, i * height / N])
            p = np.array([uniform(-P, P), uniform(-P, P)])
            c = randint(0, 0xFFFFFF) if color else 0
            self.atoms += [Atom(r, p, c)]

    ################
    # Computations

    def update(self, dt):
        X, Y = self.size

        for i, a in enumerate(self.atoms):
            (rx, ry), (px, py) = a.r, a.p
            if ry == Y-1 or ry == 0:
                py *= -1
            if rx == X-1 or rx == 0:
                px *= -1
            rx, ry = rx + px*dt, ry + py*dt
            rx, ry = clip(rx, 0, X-1), clip(ry, 0, Y-1)
            self.atoms[i].r = np.array([rx, ry])
            self.atoms[i].p = np.array([px, py])

        self.groups = {}
        for a in self.atoms:
            hash = a.hash[0]
            if hash not in self.groups:
                self.groups[hash] = 0
            self.groups[hash] += 1

    def gas_probability(self):
        ns = list(self.groups.values())
        N = len(self.atoms)
        return factorial(N) / np.prod(factorial(ns))

    def gas_entropy(self):
        return log(self.gas_probability())

    ###########
    # Drawing

    def draw(self, screen, origin, scale=10, squares=False,
             discretize=False, draw_momentum=False):
        size, offset, radius = self.size, origin, 1
        pg.draw.rect(screen, 0xFFFFFF, [*offset, *size*scale])
        for atom in self.atoms:
            atom_origin = atom.r.astype(int) if discretize else atom.r
            x, y = (atom_origin * scale + offset).astype(int)
            r = atom_radius = int(radius * scale / 2)
            c = atom.pycolor

            if squares:
                pg.draw.rect(screen, c, [x, y, scale, scale])
            else:
                x += r
                y += r
                pg.gfxdraw.filled_circle(screen, x, y, r, c)
                pg.gfxdraw.aacircle(screen, x, y, r, c)

            if draw_momentum:
                (ox, oy), (dx, dy) = (x, y), atom.p*scale
                pg.draw.line(screen, 0xFF0000, (ox, oy), (ox+dx, oy+dy), 1)


class MatLinePlot:
    def __init__(self, size, dpi=80, xlabel='', ylabel='', color='k'):
        self.color, self.size, self.dpi = color, size, dpi
        self.xlabel, self.ylabel = xlabel, ylabel
        self.fig = pylab.figure(figsize=self.size, dpi=self.dpi)
        self.clear()

    def clear(self):
        self.fig.clear()
        self.canvas = agg.FigureCanvasAgg(self.fig)
        self.ax = self.fig.gca()
        self.ax.set_xlabel(self.xlabel)
        self.ax.set_ylabel(self.ylabel)

    def plot(self, screen, origin, data):
        self.ax.set_xlim(0, len(data))
        self.ax.plot(data, color=self.color)
        self.canvas.draw()
        raw_data = self.canvas.get_renderer().tostring_rgb()
        surface = pg.image.fromstring(raw_data, self.size * self.dpi, "RGB")
        screen.blit(surface, origin)


###############################################################################
# Simulation
###############################################################################

def main():
    r = 6
    N = 2**r
    P = r + (1 - r % 2)
    R = 2*r + 1
    dt = 1 / (2*P)
    W, H = 800, 1000
    scale = 10
    clear = False

    screen = pg.display.set_mode([W, H], pg.DOUBLEBUF)
    plot = MatLinePlot(np.array([10, 4]), xlabel='t - czas', ylabel='S - entropia')
    world = World(N, P, R, color=True)
    clock = pg.time.Clock()
    entropy = []
    finish = False

    while not finish:
        clock.tick(30)

        for event in pg.event.get():
            if event.type == pg.QUIT:
                finish = True

        world.update(dt)
        entropy += [world.gas_entropy()]
        if clear and len(entropy) > clear:
            entropy = entropy[clear:]
            plot.clear()


        screen.fill(0xDDDDDD)
        world.draw(screen, np.array([40, 360]), scale=scale,
                   squares=False, discretize=False)
        plot.plot(screen, np.array([0, 0]), entropy)
        pg.display.flip()

    pg.quit()


if __name__ == '__main__':
    main()
