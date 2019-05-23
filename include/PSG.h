#pragma once

#include <driver/gpio.h>

namespace m2d
{
namespace ESP32{
class PSG
{
	typedef enum
	{
        left = 0,
        right = 1
	} Channel;

public:
	virtual void setNote(PSG::Channel channel, uint8_t noteNumber) = 0;
	virtual void setVolume(PSG::Channel channel, uint8_t volume) = 0;
	virtual void setEnvelope(uint8_t mode) = 0;
	virtual void setEnvelopeTime(uint8_t envelopeTime) = 0;
	virtual void latchMode() = 0;
	virtual void writeMode() = 0;
	virtual void invalidate() = 0;

private:
	typedef enum
	{
		LSBFIRST,
		FSBFIRST
	} BitOrder;
	virtual void writeData(uint8_t address, uint8_t data) = 0;
	static void shiftOut(uint8_t dataPin, uint8_t clockPin, PSG::BitOrder bitOrder, uint8_t val) {
		for(uint8_t i = 0; i < 8; i++) {
			if(bitOrder == BitOrder::LSBFIRST) {
				gpio_set_level(static_cast<gpio_num_t>(dataPin), !!(val & (1 << i)));
			}
			else {
				gpio_set_level(static_cast<gpio_num_t>(dataPin), !!(val & (1 << (7 - i))));
			}

			gpio_set_level(static_cast<gpio_num_t>(clockPin), 1);
			gpio_set_level(static_cast<gpio_num_t>(clockPin), 0);
		}
	}

	static inline unsigned get_clock_count(void)
	{
		unsigned r;
		asm volatile ("rsr %0, ccount" : "=r"(r));
		return r;
	}

	static void IRAM_ATTR delayMicroseconds(uint32_t us)
	{
		long startCount = get_clock_count();
		while (get_clock_count() - startCount < 160 * us) {
			__asm__ __volatile__ ("nop");
		}
	}
};
}
}