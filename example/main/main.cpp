#include <PSG/SAA1099.h>
#include <PSG/SN76489.h>
#include <PSG/AY38910.h>

extern "C" {
void app_main();
}

#define LATCH GPIO_NUM_32
#define CLOCK GPIO_NUM_33
#define DATA GPIO_NUM_12
#define WE GPIO_NUM_13
#define CS GPIO_NUM_25
#define AZ GPIO_NUM_26

#define CE GPIO_NUM_25

#define BC1 GPIO_NUM_25
#define BCDIR GPIO_NUM_25

using namespace m2d::ESP32;
void app_main()
{
	auto saa = SAA1099(LATCH, CLOCK, DATA, WE, CS, AZ);
	auto sn = SN76489(LATCH, CLOCK, DATA, WE, CE);
	auto ay = AY38910(LATCH, CLOCK, DATA, BC1, BCDIR);
	while (1) {
		saa.setNote(PSG::Channel::c0, 2);
		sn.setNote(PSG::Channel::c0, 2);
		ay.setNote(PSG::Channel::c0, 2);
	}
	return;
}
