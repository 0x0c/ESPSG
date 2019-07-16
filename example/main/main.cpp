#include <SAA1099.h>

extern "C" {
void app_main();
}

#define LATCH GPIO_NUM_32
#define CLOCK GPIO_NUM_33
#define DATA GPIO_NUM_12
#define WE GPIO_NUM_13
#define CS GPIO_NUM_25
#define AZ GPIO_NUM_26

using namespace m2d::ESP32;
void app_main()
{
	auto saa = SAA1099(LATCH, CLOCK, DATA, WE, CS, AZ);
	while (1) {
		printf("aa");
		saa.setNote(PSG::Channel::left, 2);
	}
	return;
}
