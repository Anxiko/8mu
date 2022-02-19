#include "state.h"

// Place from 0x050 to 0x09F
const uint8_t FONT[CHARACTER_HEIGHT * NUMBER_OF_CHARACTERS] = {
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

void initialize_memory(uint8_t *memory, const uint8_t *rom) {
	memset(memory, 0, MEMORY_SIZE);
	memcpy(memory + FONT_ADDRESS_START, FONT, CHARACTER_HEIGHT * NUMBER_OF_CHARACTERS);
	if (rom != NULL) {
		memcpy(memory + ROM_ADDRESS_START, rom, ROM_SIZE);
	}
}

void initialize_timer(TimerRegister *timer_register) {
	timer_register->set_ts_millis = 0;
	timer_register->set_value = 0;
}

void copy_timer(TimerRegister *dst, const TimerRegister *src) {
	dst->set_value = src->set_value;
	dst->set_ts_millis = src->set_ts_millis;
}

bool timer_equals(const TimerRegister *left, const TimerRegister *right) {
	return left->set_ts_millis == right->set_ts_millis && left->set_value == right->set_value;
}

void init_state(CpuState *cpu_state, const uint8_t *rom) {
	initialize_memory(cpu_state->memory, rom);
	memset(cpu_state->stack, 0, STACK_SIZE * 2);
	cpu_state->stack_size = 0;

	cpu_state->program_counter = ROM_ADDRESS_START;
	cpu_state->index_register = 0;
	memset(cpu_state->register_bank, 0, REGISTERS);

	memset(cpu_state->display, 0, SCREEN_SIZE_BYTES);
	cpu_state->sound_playing = false;

	memset(cpu_state->keyboard, 0, NUMBER_OF_KEYS);

	initialize_timer(&cpu_state->delay_timer);
	initialize_timer(&cpu_state->sound_timer);
}

void copy_state(CpuState *dst, const CpuState *src) {
	memcpy(dst->memory, src->memory, MEMORY_SIZE);
	memcpy(dst->stack, src->stack, STACK_SIZE * 2);
	dst->stack_size = src->stack_size;

	dst->program_counter = src->program_counter;
	dst->index_register = src->index_register;
	memcpy(dst->register_bank, src->register_bank, REGISTERS);

	memcpy(dst->display, src->display, SCREEN_SIZE_BYTES);
	dst->sound_playing = src->sound_playing;

	memcpy(dst->keyboard, src->keyboard, NUMBER_OF_KEYS);

	copy_timer(&dst->delay_timer, &src->delay_timer);
	copy_timer(&dst->sound_timer, &src->sound_timer);
}

bool state_equals(const CpuState *left, const CpuState *right) {
	return (
		memcmp(left->memory, right->memory, MEMORY_SIZE) == 0
		&&
		memcmp(left->stack, right->stack, STACK_SIZE * 2) == 0
		&&
		left->stack_size == right->stack_size
		&&
		left->program_counter == right->program_counter
		&&
		left->index_register == right->index_register
		&&
		memcmp(left->register_bank, right->register_bank, REGISTERS * 2) == 0
		&&
		memcmp(left->display, right->display, SCREEN_SIZE_BYTES) == 0
		&&
		left->sound_playing == right->sound_playing
		&&
		memcmp(left->keyboard, right->keyboard, NUMBER_OF_KEYS) == 0
		&&
		timer_equals(&left->delay_timer, &right->delay_timer)
		&&
		timer_equals(&left->sound_timer, &right->sound_timer)
	);
}