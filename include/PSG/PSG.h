#pragma once

#include <driver/gpio.h>

namespace m2d
{
namespace ESP32
{
	class PSG
	{
	public:
		typedef enum
		{
			LSBFIRST = 0,
			MSBFIRST = 1
		} BitOrder;

		typedef enum
		{
			c0 = 0,
			c1 = 1,
			c2 = 2,
			c3 = 3,
			c4 = 4,
			c5 = 5
		} Channel;

	protected:
		static void shiftOut(uint8_t dataPin, uint8_t clockPin, PSG::BitOrder bitOrder, uint8_t val)
		{
			for (uint8_t i = 0; i < 8; i++) {
				if (bitOrder == BitOrder::LSBFIRST) {
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
			asm volatile("rsr %0, ccount"
			             : "=r"(r));
			return r;
		}

		static void IRAM_ATTR delayMicroseconds(uint32_t us)
		{
			long startCount = get_clock_count();
			while (get_clock_count() - startCount < 160 * us) {
				__asm__ __volatile__("nop");
			}
		}
	};
}
}