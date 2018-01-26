"""
__author__ = "Max Adamski"

This program achieved rank 3 in the silver league
"""

from collections import namedtuple
from enum import Enum
import math, sys, random

Point = namedtuple('Point', 'x y')
Size = namedtuple('Size', 'w h')

# constants defined by the game rules
catch_min, catch_max = 90, 1760
map_w, map_h = 16000, 9000
stun_reload_delta = 20
release_max = 1600
stun_max = 1760
visibility = 2200
move_step = 800

# adjust these for optimal performance
vmap_w, vmap_h = 5, 3
visit_delta_min = 50

def distance(a, b):
    return math.sqrt((a.x - b.x)**2 + (a.y - b.y)**2)
    
def is_ghost(object):
    return isinstance(object, Ghost)
    
def is_buster(object):
    return isinstance(object, Buster)
    
class State(Enum):
    idle, carrying, stunned, trapping = range(4)

class World:
    def __init__(self, buster_count, ghost_count, my_team_id):
        self.buster_count, self.ghost_count, self.my_team_id = \
            buster_count, ghost_count, my_team_id
        self.visible_count, self.turn = 0, 0
        self.busters, self.ghosts = {}, {}
        self.map_size = Size(vmap_w, vmap_h)
        
        self.map = {}
        for x in range(self.map_size.w):
            for y in range(self.map_size.h):
                self.map[x, y] = (False, 0)
    
    @property
    def my_busters(self):
        return [it for it in self.busters.values() if it.team_id == self.my_team_id]
        
    @property
    def enemies(self):
        return [it for it in self.busters.values() if it.team_id != self.my_team_id]
        
    @property
    def enemy_team_id(self):
        return 1 if my_team_id == 0 else 0
        
    @property
    def busters_per_player(self):
        return self.buster_count / 2
        
    def update(self, visible_count):
        self.map = {p: (visited, delta + 1) for p, (visited, delta) in self.map.items()}
        self.turn += 1
        self.visible_count = visible_count
        
    def base_position(self, team_id):
        return Point(0, 0) if team_id == 0 else Point(map_w, map_h)
        
    def entity_exists(self, entity):
        buster_exists = is_buster(entity) and entity.gid in self.busters
        ghost_exists = is_ghost(entity) and entity.gid in self.ghosts
        return entity and entity.turn == self.turn and (buster_exists or ghost_exists)
        
    def map_position(self, point):
        x, y = point
        return Point(map_w / self.map_size.w * (x + 0.5), map_h / self.map_size.h * (y + 0.5))
    
    def visit_point(self, point):
        x, y = point
        self.map[x, y] = (True, 0)
        
    def random_point(self):
        return random.choice(list(self.map.keys()))
        
    def closest_unvisited(self, point):
        points = [p for p, (visited, delta) in self.map.items() \
            if not visited or delta >= visit_delta_min]
        by_distance = lambda p: distance(self.map_position(p), point)
        return min(points, key=by_distance, default=self.random_point())

class Entity:
    def __init__(self, gid):
        self.gid = gid
        self.turn = 0
        
    def did_update(self):
        self.turn += 1
        
    def update(self, position, gtype, state, value):
        self.position, self._gtype, self._state, self._value = position, gtype, state, value
        self.did_update()
        
    def distance_to(self, object):
        if isinstance(object, Point):
            return distance(self.position, object)
        elif isinstance(object, Entity):
            return distance(self.position, object.position)
        else:
            assert False, "distance_to: unknown type {}".format(type(object))
        
class Ghost(Entity):
    def __str__(self):
        return 'Ghost<gid: {}, stamina: {}, chasing: {}'.format(self.gid, self.stamina, self.chasing_count)
        
    @property
    def chasing_count(self):
        return self._value
        
    @property
    def stamina(self):
        return self._state
    
    def is_chased(self):
        return self.chasing_count > 0
        
class Buster(Entity):
    def __init__(self, gid, world):
        super().__init__(gid)
        self.world = world
        self.stun_delta = 0
        self.patrol_point = None

    def __str__(self):
        return 'Buster<gid: {}, state: {}>'.format(self.gid, self.state)
    
    def did_update(self):
        super().did_update()
        self.stun_delta += 1
        
    @property
    def state(self):
        return State(self._state)
        
    @property
    def team_id(self):
        return self._gtype
        
    @property
    def base(self):
        return self.world.base_position(self.team_id)
        
    @property
    def ghost_id(self):
        return self._value if self.state in [State.carrying, State.trapping] else None
        
    @property
    def stun_remaining(self):
        return self._value if self.state in [State.stunned] else 0
    
    @property
    def known_enemies(self):
        return [it for it in self.world.enemies if self.exists(it)]
        
    @property
    def known_ghosts(self):
        return [it for it in self.world.ghosts.values() if self.exists(it)]
        
    @property
    def teammates(self):
        return [it for it in self.world.my_busters if it is not self]
        
    def is_stun_reloaded(self):
        return self.stun_delta >= stun_reload_delta    
    
    def can_release(self):
        return self.distance_to(self.base) <= release_max
    
    def can_trap(self, entity):
        return is_ghost(entity) and catch_min <= self.distance_to(entity) <= catch_max
    
    def can_stun(self, entity):
        return is_buster(entity) and self.is_stun_reloaded() and self.distance_to(entity) <= stun_max
        
    def can_see(self, entity):
        return self.exists(entity) and self.distance_to(entity) <= visibility

    def exists(self, entity):
        return self.world.entity_exists(entity)
        
    def can_catch(self, enemy):
        return enemy.state != State.carrying or self.distance_to(enemy.base) < enemy.distance_to(enemy.base) + stun_max
        
    def best_destination(self):
        if self.state == State.carrying:
            return self.base
        if not self.patrol_point or self.distance_to(self.patrol_point) <= move_step:
            key = self.world.closest_unvisited(self.position)
            self.world.visit_point(key)
            self.patrol_point = self.world.map_position(key)
        return self.patrol_point
    
    def should_target_ghost(self, it):
        return self.state != State.carrying and \
            (it.stamina <= 25 or self.turn >= 30 or \
            self.world.ghost_count < 10)
            
    def should_target_enemy(self, it):
        return (self.state == State.carrying and self.can_stun(it)) or \
            (not it.state == State.stunned and \
            (it.state == State.carrying and self.can_catch(it)) or \
            (self.can_stun(it)))
        
    def best_target(self):
        by_distance = lambda it: self.distance_to(it)
        by_stamina = lambda it: it.stamina
        
        target_enemies = [it for it in self.known_enemies if self.should_target_enemy(it)]
        target_ghosts = [it for it in self.known_ghosts if self.should_target_ghost(it)]
        
        if target_enemies: 
            return min(target_enemies, key=by_distance)
        elif target_ghosts: 
            return min(target_ghosts, key=by_stamina)
        else: 
            return None
        
    def did_find_target(self, target):
        if self.can_stun(target):
            self.stun(target)
        elif self.can_trap(target):
            self.trap(target)
        else:
            self.move(target.position)
    
    def execute_turn(self):
        destination = self.best_destination()
        target = self.best_target()
        
        if self.can_release() and self.state == State.carrying:
            self.release()
        elif target:
            print(self, "has target", target, 'exists:', self.can_see(target), file=sys.stderr)
            self.did_find_target(target)
        else:
            self.move(destination)
        
    # -- Commands

    def move(self, point):
        print("MOVE", int(point.x), int(point.y))
        
    def trap(self, ghost):
        print("BUST", ghost.gid)
        
    def stun(self, buster):
        self.stun_delta = 0
        print("STUN", buster.gid)
      
    def release(self):
        print("RELEASE")
        
if __name__ == '__main__':
    buster_count = int(input()) * 2
    ghost_count = int(input())
    my_team_id = int(input())
    world = World(buster_count, ghost_count, my_team_id)
    
    while True:
        visible_count = int(input())
        world.update(visible_count)
        
        for i in range(world.visible_count):
            gid, x, y, gtype, state, value = [int(j) for j in input().split()]
            position = Point(x, y)
            
            if gtype == -1:
                if gid not in world.ghosts: 
                    world.ghosts[gid] = Ghost(gid)
                world.ghosts[gid].update(position, gtype, state, value)
                world.ghosts[gid].turn = world.turn
            else:
                if gid not in world.busters:
                    world.busters[gid] = Buster(gid, world)
                world.busters[gid].update(position, gtype, state, value)
                world.busters[gid].turn = world.turn

        for buster in sorted(world.my_busters, key=lambda it: it.gid):
            buster.execute_turn()

