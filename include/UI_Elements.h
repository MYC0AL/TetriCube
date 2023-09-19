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

//All buttons
const UIButton UI_START = {145,200,200,70};
const UIButton UI_BACK_TO_START = {10,395,85,85};
const UIButton UI_RUBIKS = {60,180,135,135};
const UIButton UI_TETRIS = {285,180,135,130};
const UIButton UI_SETTINGS = {140,365,200,60};
const UIButton UI_BACK_TO_SELECT = {10,395,85,85};

// Home Scene
const Scene SCENE_HOME = {"/home_screen_opt.jpg", {UI_START} };
const Scene SCENE_SELECT_GAME = {"/select_game_opt.jpg", {UI_BACK_TO_START, UI_TETRIS, UI_RUBIKS, UI_SETTINGS} };
const Scene SCENE_SETTINGS = {"/settings", {UI_BACK_TO_SELECT} };

#endif