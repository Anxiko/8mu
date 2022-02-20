#include "timers.h"

uint8_t read_timer(const TimerRegister *timer) {
	if (timer->set_ts_millis == 0) {
		return 0;
	}

	int64_t current_time = time_millis();
	int64_t time_elapsed;
	if (current_time >= timer->set_ts_millis) {
		time_elapsed = current_time - timer->set_ts_millis;
	} else {
		// This should never happen, since the current time should always be greater than the set timer.
		time_elapsed = 0;
	}

	int64_t ticks_elapsed = (60 * time_elapsed) / 1000;

	if (timer->set_value >= ticks_elapsed) {
		return timer->set_value - ticks_elapsed;
	}
	return 0;
}

void write_to_timer(TimerRegister *timer, uint8_t value) {
	int64_t current_time_millis = time_millis();
	timer->set_ts_millis = current_time_millis;
	timer->set_value = value;
}

void update_beeper_status(CpuState *cpu_state) {
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