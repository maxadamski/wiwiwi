#pragma once

#include "util.hh"

const i32 bullet_speed = 0.3*1000;
const i32 bullet_decay = 1000*1;
const i32 max_energy = 3;
const i32 init_angle = -PI/2*1000;

i32 game_time = 5*60 * 1000;
i32 reset_time = 30 * 1000;
i32 map_size = 4 * 1000;

struct Player {
    i32 id, x, y, angle, spice, energy, shield, shield_time, shield_decay;
    string nick = "someone";
    bool game_over = false;
    i32 fd = -1;

    void enable_shield(i32 decay);
    void update_shield(i32 dt);

    inline string encode() const {
        return S(id)+","+S(x)+","+S(y)+","+S(angle)+","+S(spice)+","+S(energy)+","+S(shield)+","+S(game_over);
    }

    inline void update(i32 dt) {
        update_shield(dt);
    }
};

struct Rock {
    i32 id, x, y, angle, speed, size, health;
    bool disable = false;

    inline string encode() const {
        return S(id)+","+S(x)+","+S(y)+","+S(angle)+","+S(speed)+","+S(size)+","+S(health);
    }

    inline void update(i32 dt) {
        x += dt * speed * cos(angle / 1000.0);
        y += dt * speed * sin(angle / 1000.0);
    }
};

struct Bullet {
    i32 id, pid, x, y, angle, time;

    inline string encode() const {
        return S(id)+","+S(pid)+","+S(x)+","+S(y)+","+S(angle)+","+S(time);
    }

    inline void update(i32 dt) {
        x += dt * bullet_speed * cos(angle / 1000.0);
        y += dt * bullet_speed * sin(angle / 1000.0);
        time += dt;
    }
};

struct Pellet {
    i32 id, x, y, value, type;

    inline string encode() const {
        return S(id)+","+S(x)+","+S(y)+","+S(value)+","+S(type);
    }
};

struct Game {
    Table<Player> players;
    Table<Bullet> bullets;
    Table<Pellet> pellets;
    Table<Rock> rocks;

    bool finished = false;
    bool reset = false;
    i32 rock_count = (int)map_size/10;
    i32 until_stop;
    i32 until_reset = 0;
    i32 until_reset_max;

    uniform_int_distribution<> angle_dist{0, (i32)TAU*1000};
    uniform_int_distribution<> coord_dist{0, map_size*1000};

    normal_distribution<> rock_speed_dist{5, 2};
    normal_distribution<> rock_size_dist{60, 10};
    uniform_int_distribution<> unit_dist{0, 1000};

    void init();
    void terminate_player(Player &player);
    void did_hit_rock(Player &player);
    void did_hit_bullet(Player &player, Bullet &obj);
    void did_hit_pellet(Player &player, Pellet &obj);
    void step(float dt);
    Rock &spawn_rock();
    Player &spawn_player();
    Bullet &spawn_bullet(i32 pid, i32 x, i32 y, i32 angle);
    void spawn_pellets(Rock &rock);
    void spawn_pellets(Player &player);

    inline Game(i32 game_time, i32 reset_time) {
        until_stop = game_time;
        until_reset_max = reset_time;
    }

    inline i32 winner() {
        i32 bestScore = 0;
        i32 bestId = -1;
        for (auto &[player_id, player] : players.data) {
            if (player.spice > bestScore) {
                bestScore = player.spice;
                bestId = player_id;
            }
        }
        return bestId;
    }

    inline string encode() const {
        return S(map_size)+","+S(until_reset)+","+S(until_stop)+","+S(finished);
    }
};

