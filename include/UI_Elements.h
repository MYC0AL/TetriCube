#ifndef UI_ELEMENTS_H
#define UI_ELEMENTS_H

#include <vector>

// Struct declarations
struct UIButton {
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
};

struct Scene {
    const char * image;
    std::vector<UIButton> ui_elements;
};

// All buttons
const UIButton UI_START = {145,200,200,70};
const UIButton UI_BACK = {10,395,85,85};
const UIButton UI_RUBIKS = {60,180,135,135};
const UIButton UI_TETRIS = {285,180,135,130};
const UIButton UI_SETTINGS = {140,365,200,60};
const UIButton UI_HIGH_SCORES = {385,395,85,85};

const UIButton UI_TETRIS_LEFT = {30,175,100,100};
const UIButton UI_TETRIS_RIGHT = {355,175,100,100};
const UIButton UI_TETRIS_DOWN = {195,340,100,100};
const UIButton UI_TETRIS_ROTATE = {185,170,110,110};
const UIButton UI_TETRIS_PAUSE = {10,395,85,85};

const UIButton UI_RUBIKS_PAUSE = {195,195,90,90};

// Home Scene
const Scene SCENE_HOME = {"/home_screen_comp.jpg", {UI_START} };
const Scene SCENE_SELECT_GAME = {"/select_game_comp.jpg", {UI_BACK, UI_TETRIS, UI_RUBIKS, UI_SETTINGS, UI_HIGH_SCORES}};
const Scene SCENE_SETTINGS = {"/settings_comp.jpg", {UI_BACK} };
const Scene SCENE_HIGH_SCORES = {"/high_scores_comp.jpg", {UI_BACK}};

// Tetris Controller
const Scene SCENE_TETRIS_CONTROLS = {"/tetris_controller.jpg", {UI_TETRIS_LEFT, UI_TETRIS_RIGHT, UI_TETRIS_DOWN, UI_TETRIS_ROTATE,UI_TETRIS_PAUSE}};

// Rubiks Controller
const Scene SCENE_RUBIKS_CONTROLS = {"/",{UI_RUBIKS_PAUSE}};

// DEBUG Screen Num Select
const UIButton UI_SCREEN_0 = {120,190,100,100};
const UIButton UI_SCREEN_1 = {120,50,100,100};
const UIButton UI_SCREEN_2 = {240,190,100,100};
const UIButton UI_SCREEN_3 = {120,340,100,100};
const UIButton UI_SCREEN_4 = {0,190,100,100};
const UIButton UI_SCREEN_5 = {360,190,100,100};
const UIButton UI_SCREENS[6] = {UI_SCREEN_0,UI_SCREEN_1,UI_SCREEN_2,UI_SCREEN_3,UI_SCREEN_4,UI_SCREEN_5};

#endif