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
const UIButton UI_START =               {145,200,200,70};
const UIButton UI_BACK =                {10,395,85,85};
const UIButton UI_RUBIKS =              {75,100,135,135};
const UIButton UI_TETRIS =              {270,100,135,130};
const UIButton UI_TTT =                 {75,255,135,135};
const UIButton UI_SETTINGS =            {140,410,200,60};
const UIButton UI_HIGH_SCORES =         {385,395,85,85};
const UIButton UI_HOME =                {395,395,85,85};

const UIButton UI_TETRIS_LEFT =         {30,175,100,100};
const UIButton UI_TETRIS_RIGHT =        {355,175,100,100};
const UIButton UI_TETRIS_DOWN =         {195,340,100,100};
const UIButton UI_TETRIS_ROTATE =       {185,170,110,110};
const UIButton UI_TETRIS_PAUSE =        {10,395,85,85};
const UIButton UI_TETRIS_BACK_TO_GAME = {10,395,85,85};
const UIButton UI_TETRIS_RESET =        {190,185,100,135};

const UIButton UI_RUBIKS_PAUSE =        {195,195,90,90};
const UIButton UI_RUBIKS_BACK_TO_GAME = {10,395,85,85};
const UIButton UI_RUBIKS_SCRAMBLE =     {145,120,180,130};
const UIButton UI_RUBIKS_SOLVE =        {180,280,115,115};

const UIButton UI_INITIAL_UP1 =         {65,95,80,80};
const UIButton UI_INITIAL_UP2 =         {195,95,80,80};
const UIButton UI_INITIAL_UP3 =         {330,95,80,80};
const UIButton UI_INITIAL_DOWN1 =       {75,270,80,80};
const UIButton UI_INITIAL_DOWN2 =       {205,270,80,80};
const UIButton UI_INITIAL_DOWN3 =       {340,270,80,80};
const UIButton UI_SUBMIT_INITIALS =     {40,390,390,65};

const UIButton UI_TTT_TILE_0 =          {0,0,150,150};
const UIButton UI_TTT_TILE_1 =          {165,0,150,150};
const UIButton UI_TTT_TILE_2 =          {330,0,150,150};
const UIButton UI_TTT_TILE_3 =          {0,165,150,150};
const UIButton UI_TTT_TILE_4 =          {165,165,150,150};
const UIButton UI_TTT_TILE_5 =          {330,165,150,150};
const UIButton UI_TTT_TILE_6 =          {0,330,150,150};
const UIButton UI_TTT_TILE_7 =          {165,330,150,150};
const UIButton UI_TTT_TILE_8 =          {330,330,150,150};

const UIButton UI_SETTINGS_TTT_MODE =   {400,90,50,50};
// Scenes
const Scene SCENE_HOME = {"/play.jpg", {UI_START} };
const Scene SCENE_SELECT_GAME = {"/select_game.jpeg", {UI_BACK, UI_TETRIS, UI_RUBIKS, UI_TTT, UI_SETTINGS, UI_HIGH_SCORES}};
const Scene SCENE_SETTINGS = {"/settings.jpg", {UI_BACK, UI_SETTINGS_TTT_MODE} };
const Scene SCENE_HIGH_SCORES = {"/high_scores.jpg", {UI_BACK}};

// Tetris Controller
const Scene SCENE_TETRIS_CONTROLS = {"/tetris_controller.jpg", {UI_TETRIS_LEFT, UI_TETRIS_RIGHT, UI_TETRIS_DOWN, UI_TETRIS_ROTATE,UI_TETRIS_PAUSE}};

// Tetris Pause
const Scene SCENE_TETRIS_PAUSE = {"/tetris_pause.jpg", {UI_TETRIS_BACK_TO_GAME,UI_HOME,UI_TETRIS_RESET}};

// Tetris Game Over
const Scene SCENE_TETRIS_GAME_OVER = {"/tetris_game_over.jpg", {UI_HOME}};

// Tetris Enter Initials
const Scene SCENE_ENTER_INITIALS = {"/enter_initials.jpg",{UI_INITIAL_UP1, UI_INITIAL_UP2, UI_INITIAL_UP3,
                                                             UI_INITIAL_DOWN1, UI_INITIAL_DOWN2, UI_INITIAL_DOWN3,
                                                             UI_SUBMIT_INITIALS}};

// Rubiks Controller
const Scene SCENE_RUBIKS_CONTROLS = {"/",{UI_RUBIKS_PAUSE}};

// Rubiks Pause
const Scene SCENE_RUBIKS_PAUSE = {"/cube_pause.jpg",{UI_RUBIKS_BACK_TO_GAME,UI_HOME,UI_RUBIKS_SCRAMBLE,UI_RUBIKS_SOLVE}};

// TTT
const Scene SCENE_TTT_PAUSE = {"/tic_tac_toe_reset.jpeg",{UI_TETRIS_RESET,UI_HOME}};

// Pong
const Scene SCENE_PONG_PAUSE = {"/pong_pause.jpeg",{UI_TETRIS_BACK_TO_GAME,UI_HOME,UI_TETRIS_RESET}};

// DEBUG Screen Num Select
const UIButton UI_SCREEN_0 = {120,190,100,100};
const UIButton UI_SCREEN_1 = {120,50,100,100};
const UIButton UI_SCREEN_2 = {240,190,100,100};
const UIButton UI_SCREEN_3 = {120,340,100,100};
const UIButton UI_SCREEN_4 = {0,190,100,100};
const UIButton UI_SCREEN_5 = {360,190,100,100};
const UIButton UI_SCREENS[6] = {UI_SCREEN_0,UI_SCREEN_1,UI_SCREEN_2,UI_SCREEN_3,UI_SCREEN_4,UI_SCREEN_5};

#endif