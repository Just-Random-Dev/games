#include <vector>
#include <iostream>
#include <cstdlib>
#include <exception>
#include <cstdint>
#include <thread>
#include <chrono>
#include <conio.h>
#include <string>
#include <windows.h> 

typedef struct 
{
    uint64_t x;
    uint64_t y;
} pos_t;

enum eSnakeMovement
{
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_DEFAULT
};

char Winner[15];
pos_t FruitPos = { 0, 0 };