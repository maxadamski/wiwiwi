#include "util.hh"
#include "engine.hh"
#include <cmath>

//using namespace std;
using namespace glm;

u32 KEY_DIR_U = GLFW_KEY_W;
u32 KEY_DIR_L = GLFW_KEY_A;
u32 KEY_DIR_D = GLFW_KEY_S;
u32 KEY_DIR_R = GLFW_KEY_D;
u32 KEY_FLY_U = GLFW_KEY_SPACE;
u32 KEY_FLY_D = GLFW_KEY_LEFT_SHIFT;

i32 windowWidth = 1024, windowHeight = 720;
f32 aspectRatio = (f32) windowWidth / windowHeight;
Window *window;

f32 FoV = 60.0;
f32 movementSpeed = 10.0;

bool mouseLocked = false;
bool flyingEnabled = false;
bool drinking = false;

f32 mouseSpeed = 0.001;
f32 mouseWheel = 0.0;
f32 lastMouseWheel = mouseWheel;

f64 mouseRX = 0.0;
f64 mouseLimitRX = 1.4;

f32 clickDelayMax = 1.0;
f32 clickDelay = clickDelayMax;

f32 drunk = 0.0; // [0, 1]
f32 bottleAnim = 0.0;
f32 camAnim = 0.0;
f32 suitAnim = 0.0;

f32 playerHeight = 14;

Camera camera(vec3(4, 14, 23), vec3(-0.15, -PI, 0));

Model sky("res/models/skydome/skydome.obj");
Model cube("res/models/cube/cube.obj");

Object *heldObject = nullptr;
//Object *heldBottle = nullptr;

Scene scene;
mat4 P, V;

// ----------------------------------------------------------------------------

bool didClick(f32 dt, u32 button) {
    if (clickDelay > 0) {
        clickDelay -= dt;
        return false;
    }

    if (GLFW_PRESS == glfwGetMouseButton(window, button)) {
        clickDelay = clickDelayMax;
        return true;
    }
    
    return false;
}

void centerMouse() {
    if (mouseLocked) {
        glfwSetCursorPos(window, windowWidth / 2.0, windowHeight / 2.0);
    }
}

void unlockMouse() {
    if (!mouseLocked) return;
    mouseLocked = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void lockMouse() {
    if (mouseLocked) return;
    mouseLocked = true;
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    centerMouse();
}

void toggleFlying() {
    flyingEnabled = !flyingEnabled;
    if (flyingEnabled)
        eprintf("info: flying enabled\n");
    else
        eprintf("info: flying disabled\n");
}

void reloadShaders() {
    for (auto& [k, v] : scene.shaders) v->reload();
}

bool checkBoundaries(f32 x1, f32 x2, f32 z1, f32 z2){
    return camera.pos.x > x1 && camera.pos.x < x2
        && camera.pos.z > z1 && camera.pos.z < z2;
}


bool collisionDetection(){
    if (flyingEnabled) return false;
    return  camera.pos.x > 28 || camera.pos.x < -29 
        || camera.pos.z > 27 || camera.pos.z < -21 
        || checkBoundaries(-13, 21.5, -26, -4) 
        || checkBoundaries(-10, 18.5, -4, 0) 
        || checkBoundaries(-26.5, -4, 11.5, 18.5) 
        || checkBoundaries(-20, -13.5, 4, 26) 
        || checkBoundaries(1, 23.5, 10, 19) 
        || checkBoundaries(8.5, 15, 3.5, 25) 
        || checkBoundaries(-20, -8, 13.5, 20.5) 
        || checkBoundaries(6.5, 18.5, 11.5, 21);
}

void onUpdate(f32 dt) {
    f64 xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    centerMouse();

    f32 drx = mouseSpeed * (windowHeight / 2.0 - ypos);
    f32 dry = mouseSpeed * (windowWidth / 2.0 - xpos);
    if (mouseLocked) {
        if (abs(mouseRX + drx) < mouseLimitRX) {
            mouseRX += drx;
            camera.offsetPitch(drx);
        }

        camera.offsetYaw(dry);

    }

    {
        f32 slowdown = drunk * movementSpeed * 0.5;
        f32 slalom = drunk * 0.05 * sin(camAnim);

        f32 speed = movementSpeed - slowdown;

        f32 dx = 0, dy = 0, dz = 0;
        if (glfwGetKey(window, KEY_DIR_U) == GLFW_PRESS) dz += dt * speed;
        if (glfwGetKey(window, KEY_DIR_D) == GLFW_PRESS) dz -= dt * speed;
        if (glfwGetKey(window, KEY_DIR_R) == GLFW_PRESS) dx += dt * speed;
        if (glfwGetKey(window, KEY_DIR_L) == GLFW_PRESS) dx -= dt * speed;
        if (glfwGetKey(window, KEY_FLY_U) == GLFW_PRESS) dy += dt * speed;
        if (glfwGetKey(window, KEY_FLY_D) == GLFW_PRESS) dy -= dt * speed;

        if (dz > 0) {
            dx += slalom;
        }

        camera.offsetRight(dx);
        camera.offsetFront(dz);

        if (collisionDetection()) {
            camera.offsetRight(-dx);
            camera.offsetFront(-dz);
        }

        if (flyingEnabled) playerHeight += dy;
        camera.pos.y = playerHeight;

        f32 deltaWheel = mouseWheel - lastMouseWheel;
        lastMouseWheel = mouseWheel;

        if (heldObject && flyingEnabled) {
            Object &x = *heldObject;
            x.origin = camera.pos + camera.front * vec3(10);
            x.rotation = camera.rot.y + PI;
            x.scale += deltaWheel / 10.0;
        }
    }

    clickDelay -= dt;

    f32 pulse = drunk * 10 * sin(camAnim);
    vec3 swirl = drunk * 0.1f * vec3(sin(camAnim));
    camAnim += dt;
	suitAnim += dt * 0.1;

    P = perspective(radians(FoV + pulse), aspectRatio, 1.0f, 500.0f);
    V = lookAt(camera.pos, camera.pos + camera.front + swirl, camera.up);
}

// ----------------------------------------------------------------------------

void onDraw(f32 dt) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        Program &shader = *scene.shaders["sky"];
        glUseProgram(shader.id);
        mat4 M(1.0);
        M = translate(M, camera.pos + vec3(0, -50, 0));
        M = scale(M, vec3(300));

        glDepthMask(false);
        glUniformMatrix4fv(shader.u("PVM"), 1, false, value_ptr(P*V*M));
        sky.draw(shader);
        glDepthMask(true);

        M = mat4(1.0);
        M = translate(M, vec3(0, -100, 0));
        M = scale(M, vec3(500, 1, 500));

        glUniformMatrix4fv(shader.u("PVM"), 1, false, value_ptr(P*V*M));
        cube.draw(shader);
    }

    {
        mat4 M(1.0);
        M = scale(M, vec3(62, 50, 58));

        Program &shader = *scene.shaders["phong+mat"];
        glUseProgram(shader.id);
        glUniformMatrix4fv(shader.u("P"), 1, false, value_ptr(P));
        glUniformMatrix4fv(shader.u("V"), 1, false, value_ptr(V));
        glUniformMatrix4fv(shader.u("E"), 1, false, value_ptr(camera.pos));
        glUniformMatrix4fv(shader.u("M"), 1, false, value_ptr(M));
        cube.draw(shader);
    }

    for (Object *objectPtr : scene.objects) {
        if (objectPtr == heldObject) continue;
        Object &object = *objectPtr;
        if (!object.visible) continue;
        Model &model = *object.model;
        Program &shader = *object.shader;

		if (drunk >= 1.0 && model.key == "suit") {
			vec3 dir = camera.pos - object.origin;
			object.rotation = atan(dir.x, dir.z);
		}

        mat4 M(1.0);
        M = translate(M, object.origin);
        M = rotate(M, object.rotation, vec3(0, 1, 0));
        M = scale(M, vec3(object.scale));

        if (!heldObject && object.selectable) {
            vec3 worldMax = M * vec4(model.maxCoords, 1);
            vec3 worldMin = M * vec4(model.minCoords, 1);
            bool selected = testAABB(camera.pos, camera.front, worldMin, worldMax) &&
                length(camera.pos - object.origin) < 15;

            if (selected) {
                M = scale(M, vec3(1.1));

                if (didClick(dt, GLFW_MOUSE_BUTTON_LEFT)) heldObject = objectPtr;

                if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
                    dprintf("o: (%.2f %.2f %.2f), s: (%.2f), r: (0 %.2f 0)\n",
                            object.origin.x, object.origin.y, object.origin.z, object.scale, object.rotation);
                }
            }
        }

        glUseProgram(shader.id);
        glUniformMatrix4fv(shader.u("P"), 1, false, value_ptr(P));
        glUniformMatrix4fv(shader.u("V"), 1, false, value_ptr(V));
        glUniformMatrix4fv(shader.u("E"), 1, false, value_ptr(camera.pos));
        glUniformMatrix4fv(shader.u("M"), 1, false, value_ptr(M));
        model.draw(shader);
    }

    if (heldObject) {
        Object &object = *heldObject;
        Model &model = *object.model;
        Program &prog = *object.shader;

        glUseProgram(prog.id);
        glUniformMatrix4fv(prog.u("P"), 1, false, value_ptr(P));
        glUniformMatrix4fv(prog.u("V"), 1, false, value_ptr(V));
        glUniformMatrix4fv(prog.u("E"), 1, false, value_ptr(camera.pos));

        mat4 M(1.0);

        if (drinking || !flyingEnabled) {
            M = translate(M, camera.pos + camera.front * vec3(1.8, 1, 1.8));
            M = translate(M, vec3(0, -0.8, 0));
            M = scale(M, vec3(object.scale) * 0.5f);
            M = rotate(M, camera.rot.y + PI, vec3(0, 1, 0));
        } else {
            M = translate(M, object.origin);
            M = rotate(M, object.rotation, vec3(0, 1, 0));
            M = scale(M, vec3(object.scale));

        }

        if (drinking) {
            M = rotate(M, bottleAnim, vec3(1, 0, 0));

            bottleAnim += 0.008;
            if (bottleAnim > 1) {
                bottleAnim = 0;
                object.visible = false;
                heldObject = nullptr;
                drinking = false;
                if (drunk >= 1.0) {
                    dprintf("totally wasted!\n");
                } else {
                    drunk += 0.05;
                    dprintf("drunk = %.2f%%\n", drunk*100);
                }
            }
        } else {
            //M = rotate(M, camera.rot.x, vec3(1, 0, 0));

            if (!drinking && didClick(dt, GLFW_MOUSE_BUTTON_LEFT)) {
                heldObject = nullptr;
            }

            if (object.drinkable && didClick(dt, GLFW_MOUSE_BUTTON_RIGHT)) {
                drinking = true;
            }
        }

        glUniformMatrix4fv(prog.u("M"), 1, false, value_ptr(M));
        model.draw(prog);
    }

    glfwSwapBuffers(window);
}

void onInit() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    scene.read("res/scenes/bar.json");
    scene.load();
    sky.load();
    cube.load();
    glBindVertexArray(0);
}

void onExit() {
    scene.unload();
    sky.unload();
    cube.unload();
}

void onResize(Window *window, i32 width, i32 height) {
    windowWidth = width, windowHeight = height;
    if (height == 0) return;
    glViewport(0, 0, width, height);
    aspectRatio = (f32) width / (f32) height;
    centerMouse();
}

void onMouseScroll(Window *window, f64 xOffset, f64 yOffset) {
    mouseWheel += (f32) yOffset;
}

void onMouseClick(Window *window, i32 button, i32 action, i32 mods) {
    if (action == GLFW_PRESS) {
        f64 x, y;
        glfwGetCursorPos(window, &x, &y);
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (!mouseLocked) lockMouse();
        }
    }
}

void onKeyboard(Window *window, i32 key, i32 code, i32 action, i32 mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) unlockMouse();
        if (key == GLFW_KEY_M) toggleFlying();
        if (key == GLFW_KEY_R) reloadShaders();
        if (key == GLFW_KEY_P) dprintf("pos: (%.2f %.2f %.2f)\n", camera.pos.x, camera.pos.y, camera.pos.z);
        if (key == GLFW_KEY_J) scene.save("res/scenes/bar.json");
    }
}

void onError(i32 code, char *message) {
    eprintf("error %d: %s\n", code, message);
}

int main(void) {
    ilInit();

    if (!glfwInit()) fatalError("failed to init GLEW");
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "CG&V Project", NULL, NULL);
    if (!window) fatalError("failed to open GLFW window");
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetWindowSizeCallback(window, onResize);
    glfwSetKeyCallback(window, onKeyboard);
    glfwSetScrollCallback(window, onMouseScroll);
    glfwSetMouseButtonCallback(window, onMouseClick);
    glfwPollEvents();

    glewExperimental = true;
    if (glewInit() != GLEW_OK) fatalError("failed to init GLEW");

    onInit();
    glfwSetTime(0);
    while (!glfwWindowShouldClose(window)) {
        f32 dt = glfwGetTime();
        glfwSetTime(0);
        onUpdate(dt);
        onDraw(dt);
        glfwPollEvents();
    }

    onExit();
    glfwTerminate();
    exit(0);
}

