#pragma once
//All global variables that point to hardware of chip8
#include<stdint.h>
#include<string>

//#defines
#define PROGRAM_START 0x200
#define FONT_START 0x50
#define DEFAULT_IPS 700

extern uint8_t RAM[4096]; //RAM: 4096 bytes

//General urpose registers
extern uint8_t V[16];

//Index register
extern uint16_t IX;

//Timing registers
extern uint8_t DT;//Delay
extern uint8_t ST;//Sound

//Program Counter
extern uint16_t PC;

//Stack
extern uint16_t S[16];
extern int _STop;
void SPush(uint16_t val);
void SPop(uint16_t* val);

//Display
extern uint8_t DISPLAY[64][32];

//Special
extern int gKEY;
extern int gKeyStatus[16];

//Methods
void InitChip8();
void LoadROMFromFile(std::string file);
void LoadTestROM();

//Executor
int chip8_exec();
void exec_0NNN();
void exec_00E0();
void exec_00EE();
void exec_1NNN();
void exec_2NNN();
void exec_3XNN();
void exec_4XNN();
void exec_5XY0();
void exec_6XNN();
void exec_7XNN();
void exec_8XY0();
void exec_8XY1();
void exec_8XY2();
void exec_8XY3();
void exec_8XY4();
void exec_8XY5();
void exec_8XY6();
void exec_8XY7();
void exec_8XYE();
void exec_9XY0();
void exec_ANNN();
void exec_BNNN();
void exec_CXNN();
void exec_DXYN();
void exec_EX9E();
void exec_EXA1();
void exec_FX07();
void exec_FX0A();
void exec_FX15();
void exec_FX18();
void exec_FX1E();
void exec_FX29();
void exec_FX33();
void exec_FX55();
void exec_FX65();