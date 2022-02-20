#include "timers.h"

uint8_t read_timer(TimerRegister *timer) {
	if (timer->set_ts_millis == 0) {
		return 0;
	}

	int64_t current_time = time_millis();
	int64_t ticks_elapsed = (60 * current_time) / 1000;

	if (ticks_elapsed > timer->set_value) {
		timer->set_value = 0;
		timer->set_ts_millis = 0;
		return 0;
	}

	return timer->set_value - ticks_elapsed;
}

void write_to_timer(TimerRegister *timer, uint8_t value) {
	int64_t current_time_millis = time_millis();
	timer->set_ts_millis = current_time_millis;
	timer->set_value = value;
}

void refresh_timers(CpuState *cpu_state) {
	read_timer(&cpu_state->delay_timer);
	uint8_t sound_ticks = read_timer(&cpu_state->sound_timer);
	set_beeper_state(cpu_state, sound_ticks > 0? true : false);
}

uint8_t read_delay_timer(CpuState *cpu_state) {
	return read_timer(&cpu_state->delay_timer);
}

void write_delay_timer(CpuState *cpu_state, uint8_t delay) {
	write_to_timer(&cpu_state->delay_timer, delay);
}

void write_sound_timer(CpuState *cpu_state, uint8_t delay) {
	write_to_timer(&cpu_state->sound_timer, delay);
}