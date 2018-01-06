from collections import namedtuple
from enum import Enum
import sys
import math

Point = namedtuple('Point', 'x y')
Size = namedtuple('Size', 'w h')

catch_min, catch_max = 90, 1760
map_w, map_h = 16000, 9000
release_max = 1600
stun_max = 1760
visibility = 2200
move_step = 800

def distance(a, b):
    return math.sqrt((a.x - b.x)**2 + (a.y - b.y)**2)

class Strategy(Enum):
    patrol_left, patrol_right, choose_target_ghost, choose_target_enemy, stun, chase, trap, carry_back, release = range(9)

class State(Enum):
    idle, carrying, stunned, trapping = range(4)

class Direction(Enum):
    right, left, up, down = range(4)

class World:
    entity_count = 0
    my_busters_list = []
    my_busters = {}
    enemies = []
    ghosts = []

    def __init__(buster_count, ghost_count, my_team_id):
        self.buster_count, self.ghost_count, self.my_team_id = \
            busters_count, ghost_count, my_team_id
        self.center = Point(map_w // 2, map_h // 2)
        self.size = Size(map_w, map_h)

    def busters_per_player(self):
        return self.buster_count / 2

    def base_position(self, team_id):
        return Point(0, 0) if team_id == 0 else Point(map_w, map_h)

    def base_direction(self, team_id):
        return Direction.left if team_id == 0 else Direction.right


class Ghost:
    def __init__(self, gid, position, chasing_count, stamina):
        self.gid, self.position, self.chasing_count, self.stamina = \
            gid, position, chasing_count, stamina

    def chased(self):
        return self.chasing_count > 0


class Buster:
    chase_target = None
    
    strategy = None
    strategy_history = []
    state = None
    state_history = []
    target = None
    target_history = []
    
    stun_delta = 20
    turn = 0
    
    visible_enemies = []
    visible_ghosts = []
    
    teammate_count = 0
    teammates = []
    
    order = 0
    
    def __init__(self, gid, team_id, world):
       self.gid, self.team_id, self.world = gid, team_id, world

    def update(self, position, ghost_id, state):
        self.position, self.ghost_id, self.state = position, ghost_id, state
        self.increment_turn()

    def increment_turn(self):
        self.stun_delta += 1
        self.turn += 1
        
    def last_patrol(self):
        for strategy in reversed(self.strategy_history):
            if strategy in [Strategy.patrol_left, Strategy.patrol_right]:
                return strategy
        return None
            
    def get_strategy(self):
        if self.state == State.carrying:
            if self.can_release(self.target):
                return Strategy.release
            else:
                return Strategy.carry_back
        elif self.state in [State.idle, State.stunned]:
            if self.target and self.is_visible(self.target):
                if self.can_stun(self.target):
                    return Strategy.stun
                elif self.can_trap(self.target):
                    return Strategy.trap
                else:
                    return Strategy.chase
            elif self.target_enemies():
                return Strategy.choose_target_enemy
            elif self.target_ghosts():
                return Strategy.choose_target_ghost
                
            if self.at_map_limit_right():
                return Strategy.patrol_left
            elif self.at_map_limit_left():
                return Strategy.patrol_right
            elif self.last_patrol():
                return self.last_patrol()
            elif self.world.base_direction(self.team_id) == Direction.left:
                return Strategy.patrol_right
            else:
                return Strategy.patrol_left
        elif self.state == State.trapping:
            return Strategy.trap
        return None
        
    def run_strategy(self, strategy):
        self.strategy = strategy
        
        print(self.gid, "<", strategy, ">", file=sys.stderr)
        if strategy == Strategy.trap:
            self.trap(self.target)
        elif strategy == Strategy.carry_back:
            self.move(self.base_position())
        elif strategy == Strategy.release:
            self.release()
        elif strategy == Strategy.stun:
            self.stun(self.target)
        elif strategy in [Strategy.choose_target_ghost, Strategy.choose_target_enemy]:
            self.target_history.append(self.target)
            if strategy == Strategy.choose_target_ghost:
                self.target = self.target_ghosts()[0]
            elif strategy == Strategy.choose_target_enemy:
                self.target = self.target_enemies()[0]
            self.move(self.target.position)
        elif strategy == Strategy.chase:
            self.move(self.target.position)
        elif strategy in [Strategy.patrol_left, Strategy.patrol_right]:
            self.move(self.patrol_destination())
        else:
            print("<", strategy, "> invalid strategy!", file=sys.stderr)

        self.strategy_history.append(strategy)
        
    def base_position(self):
        return self.world.base_position(self.team_id)
        
    def at_map_limit_right(self):
        return map_w - visibility <= self.position.x <= map_w
        
    def at_map_limit_left(self):
        return 0 <= self.position.x <= visibility
        
    def patrol_destination(self):
        W, H, b, v = map_w, map_h, self.teammate_count, visibility - 400
        Y = v + self.order*(H - 2*v)//(b - 1)
        if self.position.y == Y:
            X = map_w if self.strategy == Strategy.patrol_right else 0
            return Point(X, Y)
        return Point(self.position.x, Y)
    
    def is_visible(self, target):
        return any(target.gid == entity.gid for entity in self.visible_ghosts + self.visible_enemies)
        
    def can_trap(self, target):
        return type(target) == Ghost and catch_min <= distance(self.position, target.position) <= catch_max
        
    def can_release(self, target):
        return type(target) == Ghost and distance(self.position, self.base_position()) <= release_max
        
    def can_stun(self, target):
        return type(target) == Buster and self.stun_reloaded() and distance(self.position, target.position) <= stun_max
        
    def stun_reloaded(self):
        return self.stun_delta >= 20
        
    def reset_stun(self):
        self.stun_delta = 0
        
    def move(self, point):
        print("MOVE", point.x, point.y)
        
    def trap(self, ghost):
        print("BUST", ghost.gid)
        
    def release(self):
        print("RELEASE")
        
    def stun(self, buster):
        self.reset_stun()
        print("STUN", buster.gid)
    
    def trapping_ghost(self):
        return type(self.target) == Ghost and self.strategy in [Strategy.trap, Strategy.chase]
        
    def teammate_trapping_ghost(self, ghost):
        return any(buster.trapping_ghost() and buster.target.gid == ghost.gid for buster in self.teammates)
        
    def target_ghosts(self):
        targets = [ghost for ghost in self.visible_ghosts if not self.teammate_trapping_ghost(ghost)]
        targets.sort(key=lambda target: distance(self.position, target.position))
        #targets.sort(key=lambda target: target.stamina)
        return targets
        
    def target_enemies(self):
        targets = self.visible_enemies
        targets.sort(key=lambda target: target.state == State.carrying)
        return targets
            
        
def main():
    world = World(buster_count=int(input())*2, ghost_count=int(input()), my_team_id=int(input()))
    initialized_busters = False
    
    while True:
        world.entity_count = int(input())
        for i in range(world.entity_count):
            entity_id, x, y, entity_type, state, value = [int(j) for j in input().split()]
            position, team_id = Point(x, y), entity_type
            ghost_id = value if state in [State.carrying, State.trapping] else None
            
            if entity_type == -1:
                ghost = Ghost(entity_id, position, chasing_count=value, stamina=state)
                world.ghosts.append(ghost)
            elif entity_type == world.my_team_id:
                if entity_id not in world.my_busters: 
                    my_busters[entity_id] = Buster(entity_id, team_id)
                world.my_busters[entity_id].update(position, ghost_id, State(state))
            else:
                buster = Buster(entity_id, team_id)
                buster.update(position, ghost_id, State(state))
                world.enemies.append(buster)
        
        if not initialized_busters:
            initialized_busters = True
            world.my_busters_list = [world.my_busters[key] for key in sorted(world.my_busters.keys())]
            for buster in world.my_busters_list:
                buster.teammates = [b for b in world.my_busters_list if b.gid != buster.gid]
        
        for i, buster in enumerate(world.my_busters_list):
            buster.teammate_count = world.busters_per_player()
            buster.visible_enemies = [enemy for enemy in world.enemies if distance(buster.position, ghost.position) <= visibility]
            buster.visible_ghosts = [ghost for ghost in world.ghosts if distance(buster.position, ghost.position) <= visibility]
            buster.order = i
            buster.run_strategy(buster.get_strategy())

main()
