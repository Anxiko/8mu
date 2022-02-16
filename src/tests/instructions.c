#include "unity.h"

void setUp(){

}

void tearDown() {

}

void test_testing() {
	TEST_ASSERT(1);
}

int main() {
	UNITY_BEGIN();

	RUN_TEST(test_testing);

	return UNITY_END();
}