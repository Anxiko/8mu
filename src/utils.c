#include "utils.h"

/*
 * Memory
 */

uint16_t read_word_from_array(uint8_t *ptr, intptr_t offset_in_bytes) {
	uint16_t rv;
	uint8_t *byte_ptr = ptr + offset_in_bytes;

	rv = byte_ptr[0] << 8 | byte_ptr[1];
	return rv;
}

void write_word_to_array(uint8_t *ptr, intptr_t offset_in_bytes, uint16_t value) {
	uint8_t first_byte = (value & 0xFF00) >> 8;
	uint8_t second_byte = value * 0xFF;

	ptr[offset_in_bytes] = first_byte;
	ptr[offset_in_bytes + 1] = second_byte;
}

/*
 * Field extraction
 */

uint16_t extract_field(uint16_t instruction, uint16_t bitmask, uint8_t offset) {
	return (instruction & bitmask) >> offset;
}

uint8_t extract_register_from_xnn(uint16_t instruction) {
	return extract_field(
		instruction,
		INSTRUCTION_FIELD_REGISTER_XNN_BITMASK, INSTRUCTION_FIELD_REGISTER_XNN_OFFSET
	);
}

uint8_t extract_immediate_from_xnn(uint16_t instruction) {
	return extract_field(
		instruction,
		INSTRUCTION_FIELD_IMMEDIATE_XNN_BITMASK, INSTRUCTION_FIELD_IMMEDIATE_XNN_OFFSET
	);
}

uint8_t extract_first_register_from_xy(uint16_t instruction) {
	return extract_field(
		instruction,
		INSTRUCTION_FIELD_FIRST_REGISTER_XY_BITMASK, INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET
	);
}

uint8_t extract_second_register_from_xy(uint16_t instruction) {
	return extract_field(
		instruction,
		INSTRUCTION_FIELD_SECOND_REGISTER_XY_BITMASK, INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET
	);
}

uint16_t extract_immediate_from_nnn(uint16_t instruction) {
	return extract_field(
		instruction,
		INSTRUCTION_FIELD_ADDRESS_NNN_BITMASK, INSTRUCTION_FIELD_ADDRESS_NNN_OFFSET
	);
}

uint8_t extract_first_register_from_xyn(uint16_t instruction) {
	return extract_field(
		instruction,
		INSTRUCTION_FIELD_FIRST_REGISTER_XYN_BITMASK, INSTRUCTION_FIELD_FIRST_REGISTER_XYN_OFFSET
	);
}

uint8_t extract_second_register_from_xyn(uint16_t instruction) {
	return extract_field(
		instruction,
		INSTRUCTION_FIELD_SECOND_REGISTER_XYN_BITMASK, INSTRUCTION_FIELD_SECOND_REGISTER_XYN_OFFSET
	);
}

uint8_t extract_immediate_from_xyn(uint16_t instruction) {
	return extract_field(
		instruction,
		INSTRUCTION_FIELD_IMMEDIATE_XYN_BITMASK, INSTRUCTION_FIELD_IMMEDIATE_XYN_OFFSET
	);
}

uint8_t extract_register_from_x(uint16_t instruction) {
	return extract_field(
		instruction,
		INSTRUCTION_FIELD_REGISTER_X_BITMASK, INSTRUCTION_FIELD_REGISTER_X_OFFSET
	);
}