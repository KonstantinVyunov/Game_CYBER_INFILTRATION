#pragma once

#include <windows.h>
#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

//Game parameters
#define GAMEBOSS true
#define START_ROOM (&map.rooms[0])
#define END_ROOM (&map.rooms[9])
#define BOSS_ROOM (&map.rooms[9])
#define MICROCHIPS_TO_WIN 6

//Hero parameters
#define HERO_HP_MAX 100
#define HERO_SCORE 0
#define HERO_CRISTALS 0
#define HERO_MEDICINES 0
#define HERO_MICROCHIPS 0

//Revolver parameters
#define DAMAGE 10
#define MAGAZINE 6
#define BULLETS_IN_CYLINDER 6
#define MAX_BULLETS_IN_CYLINDER 6

//Lootbox parameters
#define HEAL_VALUE 50
#define BULLETS 6
#define MICROCHIP 1

//Shooter parameters
#define SHOOTER_HP 10
#define SHOOTER_ATTACK 2
#define SHOOTER_SCORE 1
#define SHOOTER_CRISTALS 4

//Stormtrooper parameters
#define STORMTROOPER_HP 30
#define STORMTROOPER_ATTACK 1
#define STORMTROOPER_SCORE 2
#define STORMTROOPER_CRISTALS 6

//Firetrooper parameters
#define FIRETROOPER_HP 10
#define FIRETROOPER_ATTACK 3
#define FIRETROOPER_SCORE 3
#define FIRETROOPER_CRISTALS 5

//LevelBoss parameters
#define LEVELBOSS_HP 50
#define LEVELBOSS_ATTACK 5
#define LEVELBOSS_SCORE 35
#define LEVELBOSS_CRISTALS 35