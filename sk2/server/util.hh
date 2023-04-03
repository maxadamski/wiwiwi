#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <string>
#include <vector>
#include <stack>
#include <map>

#include <memory>
#include <algorithm>
#include <random>
#include <chrono>
#include <iterator>

#include <math.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/epoll.h> 
#include <sys/ioctl.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include <string>
#include <map>

//
// Shorthands
//

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::stack;
using std::map;
using std::shared_ptr;
using std::weak_ptr;

using namespace std;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

const int ZERO = 0;
const int ONE = 1;


//
// Input-Output
//

#define DEBUG

// Removing commas with '##' only works in GCC!
#define eprintf(f_, ...) fprintf(stderr, (f_), ##__VA_ARGS__)

#ifdef DEBUG
#define dprintf(f_, ...) fprintf(stderr, (f_), ##__VA_ARGS__)
#else
#define dprintf(f_, ...)
#endif


inline void fatal(const char *message) {
	fprintf(stderr, "fatal error: %s\n", message);
	exit(1);
}


//
// Math
//

#define PI 3.14159f
#define TAU (2*PI)

#define sizeof_val(X) (sizeof(X[0]))
#define sizeof_vec(X) (sizeof(X[0]) * X.size())


inline f64 clip(f64 value, f64 min, f64 max) {
    value = value < min ? min : value;
    value = value > max ? max : value;
    return value;
}

inline i32 clip(i32 value, i32 min, i32 max) {
    value = value < min ? min : value;
    value = value > max ? max : value;
    return value;
}

inline f64 rad(f64 deg) {
    return deg * PI / 180.0;
}

inline f64 distsq(f32 x1, f32 y1, f32 x2, f32 y2) {
    f32 dx = x2 - x1;
    f32 dy = y2 - y1;
    return dx*dx + dy*dy;
}

inline f64 dist(f32 x1, f32 y1, f32 x2, f32 y2) {
    return sqrt(distsq(x1, y1, x2, y2));
}


// 
// Random
// 

std::random_device rd{};
std::mt19937 gen{rd()};

inline f32 random_normal(f32 mean, f32 std) {
    std::normal_distribution<> dist{mean, std};
    return dist(gen);
}

inline i32 random_uniform(i32 lower, i32 upper) {
    std::uniform_int_distribution<> dist{lower, upper};
    return dist(gen);
}

//
// Time
//

inline auto now() {
    return chrono::high_resolution_clock::now();
}

inline u64 millis() {
    return now().time_since_epoch().count() / 1e6;
}

template <class T>
inline u64 millis(T delta) {
    return chrono::duration_cast<chrono::milliseconds>(delta).count();
}


//
// String
//

#define S(x) to_string(x)
#define I(x) (((x[0] >= '0' && x[0] <= '9') || x[0] == '-') ? stoi(x) : (throw "bad request"))

inline vector<string> split(string str, string token) {
    vector<string>result;
    while(str.size()) {
        u64 index = str.find(token);
        if (index != string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0) result.push_back(str);
        } else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

inline vector<string> split(i32 n, string str, string token) {
    auto res = split(str, token);
    for (i32 i = 0; i < n; i++) {
        if (res[i] == "") throw;
    }
    return res;
}


//
// Data structures
//

template <class Item>
struct Table {
    map<i32, Item> data;
    stack<i32> free_id;
    i32 next_id = 0;

    inline i32 new_id() {
        //if (free_id.empty()) {
            i32 id = next_id;
            next_id += 1;
            return id;
        //} else {
        //    i32 id = free_id.top();
        //    free_id.pop();
        //    return id;
        //}
    }

    inline i32 append(Item item) {
        i32 id = new_id();
        data[id] = item;
        return id;
    }

    inline void remove(i32 id) {
        data.erase(id);
        //free_id.push(id);
    }

    inline void remove(const unordered_set<i32> &id) {
        for (i32 i : id) data.erase(i);
    }

    inline bool contains(i32 id) {
        return data.find(id) != data.end();
    }
};


//
// File
//

inline bool readFile(const string &path, string &data) {
    ifstream file(path, ios::in);
    if (!file.is_open()) {
        fprintf(stderr, "error: couldn't open file '%s'\n", path.c_str());
        return false;
    }
    stringstream stream;
    stream << file.rdbuf();
    file.close();
    data = stream.str();
    return true;
}

inline bool fileExists(const string &name) {
    ifstream f(name.c_str());
    return f.good();
}

//
// Networking
//

struct NicePoll {
    map<i32, void (*)(i32, u32)> handlers;
    u64 max_events = 64;
    i32 epoll_fd;

    inline i32 create() {
        epoll_fd = epoll_create1(0);
        return epoll_fd;
    }

    inline void insert(i32 fd, u32 events, void (*callback)(i32, u32)) {
        epoll_event event;
        event.events = events;
        event.data.fd = fd;
        handlers[fd] = callback;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    }

    inline void erase(i32 fd) {
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
        handlers.erase(fd);
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }

    inline void handle(epoll_event event) {
        i32 fd = event.data.fd;
        handlers[fd](fd, event.events);
    }

    inline i32 wait(epoll_event *events, i32 count, i32 timeout = -1) {
        return epoll_wait(epoll_fd, events, count, timeout);
    }
};

inline i32 make_reusable(i32 fd) {
    const i32 one = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
}

inline i32 make_nonblocking(i32 fd) {
    const i32 one = 1;
    return ioctl(fd, FIONBIO, &one);
}


map<i32, string> inbox;

inline void xsend(i32 fd, string x) {
    x += "\n";
    const char *buffer = x.c_str();
    u32 length = x.size();
    u32 written = 0;
    while (written < length)
        written += write(fd, buffer + written, length - written);
}

inline vector<string> xrecv(i32 fd) {
    const i32 max_length = 1024;
    u8 buffer[max_length];
    vector<string> reqs;

    i32 length = read(fd, buffer, max_length);
    if (length <= 0) {
        return reqs;
    }
    
    string chunk(buffer, buffer + length);
    if (inbox.find(fd) != inbox.end()) {
        chunk = inbox[fd] + chunk;
    }

    vector<string> msgs = split(chunk, "\n");
    inbox[fd] = msgs.back();
    msgs.pop_back();
    return msgs;
}

inline void xclear(i32 fd) {
    inbox.erase(fd);
}
