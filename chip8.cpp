#include"chip8.h"
#include<string.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<stdlib.h>

uint8_t RAM[4096]; //RAM: 4096 bytes

//General urpose registers
uint8_t V[16];

//Index register
uint16_t IX;

//Timing registers
uint8_t DT;//Delay
uint8_t ST;//Sound

//Program Counter
uint16_t PC;

//Stack
uint16_t S[16];
int _STop;
void SPush(uint16_t val) {
	if (_STop < 15) {
		_STop++;
		S[_STop] = val;
	}
}
void SPop(uint16_t *val) {
	if (_STop >= 0) {
		*val = S[_STop];
		S[_STop] = 0;
		_STop--;
	}
}

//Display
uint8_t DISPLAY[64][32];

//Special
int gKEY;
int gKeyStatus[16];

//Methods
void InitChip8() {
	memset(RAM, 0, sizeof(RAM));
	memset(V, 0, sizeof(V));
	IX = 0;
	DT = 0;
	ST = 0;
	PC = PROGRAM_START;
	memset(S, 0, sizeof(S));
	_STop = -1;
	for (int i = 0; i < 64; i++) {
		memset(DISPLAY[i], 0, 32 * sizeof(uint8_t));
	}

	//Load font
	uint8_t font_data[] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	int loc = FONT_START;
	int i = 0;
	while (i < (4 * 16)) {
		RAM[loc + i] = font_data[i];
		i++;
	}

	gKEY = 0;
	memset(gKeyStatus, 0, sizeof(gKeyStatus));
}

void LoadROMFromFile(std::string file) {
	std::stringstream ss;
	std::ifstream f;
	f.open(file, std::ios::binary|std::ios::in);

	ss << f.rdbuf();

	f.close();
	std::string d = ss.str();
	const char* rom = d.c_str();
	for (int i = 0; i < d.size(); i++) {
		RAM[PROGRAM_START + i] = (uint8_t)rom[i];
	}
}

void LoadTestROM() {
	unsigned char program[] = {
		0x00, 0xE0,
		0x60, 0x00,
		0xF0, 0x29,
		0x60, 0x01,
		0x61, 0x01,
		0xD0, 0x15,
		0x00, 0x00
	};
	for (int i = 0; i < sizeof(program); i++) {
		RAM[PROGRAM_START + i] = (uint8_t)program[i];
	}
}

//Executor
uint32_t F;
uint32_t X;
uint32_t Y;
uint32_t N;
uint32_t NN;
uint32_t NNN;

int displayModified;

static inline void NI() {
	PC += 2;
}

int chip8_exec() {
	//Returns 1 if display was modified
	displayModified = 0;
	//Fetch
	uint16_t current_instruction = (uint16_t)(RAM[PC]<<8);
	current_instruction += RAM[PC + 1];
	/*std::cout << "PC: " << std::hex << PC << "  I: ";
	if (current_instruction == 0x00EE) {
		std::cout << "00";
	}
	if (current_instruction == 0x0000) {
		std::cout << "000";
	}
	std::cout << std::hex << current_instruction << std::endl;*/

	//Decode
	F = (uint32_t)(current_instruction & 0xF000) >> 12;
	X = (uint32_t)(current_instruction & 0x0F00) >> 8;
	Y = (uint32_t)(current_instruction & 0x00F0) >> 4;
	N = (uint32_t)(current_instruction & 0x000F);
	NN = (uint32_t)(current_instruction & 0x00FF);
	NNN = (uint32_t)(current_instruction & 0x0FFF);
	switch (F) {
	case 0:
		if (current_instruction == 0x00E0) {
			exec_00E0();
		}
		else if (current_instruction == 0x00EE) {
			exec_00EE();
		}
		else {
			exec_0NNN();
		}
		break;
	case 1:
		exec_1NNN();
		break;
	case 2:
		exec_2NNN();
		break;
	case 3:
		exec_3XNN();
		break;
	case 4:
		exec_4XNN();
		break;
	case 5:
		exec_5XY0();
		break;
	case 6:
		exec_6XNN();
		break;
	case 7:
		exec_7XNN();
		break;
	case 8:
		switch (N) {
		case 0:
			exec_8XY0();
			break;
		case 1:
			exec_8XY1();
			break;
		case 2:
			exec_8XY2();
			break;
		case 3:
			exec_8XY3();
			break;
		case 4:
			exec_8XY4();
			break;
		case 5:
			exec_8XY5();
			break;
		case 6:
			exec_8XY6();
			break;
		case 7:
			exec_8XY7();
			break;
		case 0xE:
			exec_8XYE();
			break;
		}
		break;
	case 9:
		exec_9XY0();
		break;
	case 0xA:
		exec_ANNN();
		break;
	case 0xB:
		exec_BNNN();
		break;
	case 0xC:
		exec_CXNN();
		break;
	case 0xD:
		exec_DXYN();
		break;
	case 0xE:
		switch (NN) {
		case 0x9E:
			exec_EX9E();
			break;
		case 0xA1:
			exec_EXA1();
			break;
		}
		break;
	case 0xF:
		switch (NN) {
		case 0x07:
			exec_FX07();
			break;
		case 0x0A:
			exec_FX0A();
			break;
		case 0x15:
			exec_FX15();
			break;
		case 0x18:
			exec_FX18();
			break;
		case 0x1E:
			exec_FX1E();
			break;
		case 0x29:
			exec_FX29();
			break;
		case 0x33:
			exec_FX33();
			break;
		case 0x55:
			exec_FX55();
			break;
		case 0x65:
			exec_FX65();
			break;
		}
		break;
	}
	return displayModified;
}
void exec_0NNN() {
	std::cout << "Error:Cannot execute 0NNN" << std::endl;
}
void exec_00E0() {
	for (int i = 0; i < 64; i++) {
		memset(DISPLAY[i], 0, 32 * sizeof(uint8_t));
	}
	displayModified = 1;
	NI();
}
void exec_00EE() {
	if (_STop == -1) {
		std::cout << "Attempt to return from empty stack" << std::endl;
	}
	SPop(&PC);
	NI();
}
void exec_1NNN() {
	PC = NNN;
}
void exec_2NNN() {
	if (_STop == 15) {	
		std::cout << "Attempt to push to full stack" << std::endl;
	}
	SPush(PC);
	PC = NNN;
}
void exec_3XNN() {
	if (V[X] == NN) {
		NI();
	}
	NI();
}
void exec_4XNN() {
	if (V[X] != NN) {
		NI();
	}
	NI();
}
void exec_5XY0() {
	if (V[X] == V[Y]) {
		NI();
	}
	NI();
}
void exec_6XNN() {
	V[X] = NN;
	NI();
}
void exec_7XNN() {
	V[X] += NN;
	NI();
}
void exec_8XY0() {
	V[X] = V[Y];
	NI();
}
void exec_8XY1() {
	V[X] = V[X] | V[Y];
	NI();
}
void exec_8XY2() {
	V[X] = V[X] & V[Y];
	NI();
}
void exec_8XY3() {
	V[X] = V[X] ^ V[Y];
	NI();
}
void exec_8XY4() {
	if (V[X] > (0xFF - V[Y])) {
		V[0xF] = 1;
	}
	else {
		V[0xF] = 0;
	}
	V[X] = V[X] + V[Y];
	NI();
}
void exec_8XY5() {
	if (V[X] >= V[Y]) {
		V[0xF] = 1;
	}
	else {
		V[0xF] = 0;
	}
	V[X] = V[X] - V[Y];
	NI();
}
void exec_8XY6() {
	V[0xF] = V[X] & 0x01;
	V[X] = V[Y];
	V[X] = V[X] >> 1;
	NI();
}
void exec_8XY7() {
	if (V[Y] >= V[X]) {
		V[0xF] = 1;
	}
	else {
		V[0xF] = 0;
	}
	V[X] = V[Y] - V[X];
	NI();
}
void exec_8XYE() {
	V[0xF] = V[X] & 0x80;
	if (V[0xF] == 0x80) V[0xF] = 1;
	V[X] = V[Y];
	V[X] = V[X] << 1;
	
	NI();
}
void exec_9XY0() {
	if (V[X] != V[Y]) {
		NI();
	}
	NI();
}
void exec_ANNN() {
	IX = NNN;
	NI();
}
void exec_BNNN() {
	PC = NNN + V[0];
}
void exec_CXNN() {
	uint16_t r = rand() % 256;
	V[X] = r & NN;
	NI();
}
void exec_DXYN() {
	uint8_t x = V[X] % 64;
	uint8_t y = V[Y] % 32;
	V[0xF] = 0;
	for (int i = 0; i < N; i++) {
		x = V[X] % 64;
		uint8_t sprite_byte = RAM[IX + i];
		for (int j = 0; j < 8; j++) {
			int msb = sprite_byte & 128;
			if (DISPLAY[x][y] == 1 && msb == 128) {
				DISPLAY[x][y] = 0;
				V[0xF] = 1;
			}
			else if (DISPLAY[x][y] == 0 && msb == 128) {
				DISPLAY[x][y] = 1;
				displayModified = 1;
			}
			x++;
			if (x == 64) {
				break;
			}
			sprite_byte = sprite_byte << 1;
		}
		y++;
		if (y == 32) {
			break;
		}
	}		
	NI();
}
void exec_EX9E() {
	if (gKeyStatus[V[X]] == 1) {
		NI();
	}
	NI();
}
void exec_EXA1() {
	if (gKeyStatus[V[X]] != 1) {
		NI();
	}
	NI();
}
void exec_FX07() {
	V[X] = DT;
	NI();
}
void exec_FX0A() {
	static int initialized = 0;
	static int key_copy[16];

	if (initialized == 0) {
		memcpy(key_copy, gKeyStatus, sizeof(gKeyStatus));
		initialized = 1;
	}
	else {
		for (int i = 0; i < 16; i++) {
			if (key_copy[i] == 1 && gKeyStatus[i] == 0) {
				V[X] = i;
				NI();
				initialized = 0;
				break;
			}
			if (key_copy[i] == 0 && gKeyStatus[i] == 1) {
				key_copy[i] = 1;
			}
		}
	}
}
void exec_FX15() {
	DT = V[X];
	NI();
}
void exec_FX18() {
	ST = V[X];
	NI();
}
void exec_FX1E() {
	IX += V[X];
	if (IX > 0x1000) {
		V[0xF] = 1;
	}
	NI();
}
void exec_FX29() {
	IX = FONT_START + ((V[X] & 0x0F) * 5);
	NI();
}
void exec_FX33() {
	RAM[IX] = V[X] / 100;
	RAM[IX + 1] = (V[X] - RAM[IX] * 100) / 10;
	RAM[IX + 2] = V[X] - (RAM[IX] * 100) - (RAM[IX + 1] * 10);
	NI();
}
void exec_FX55() {
	for (int i = 0; i <= X; i++) {
		RAM[IX + i] = V[i];
	}
	NI();
}
void exec_FX65() {
	for (int i = 0; i <= X; i++) {
		V[i] = RAM[IX+i];
	}
	NI();
}