#pragma once

#include <driver/gpio.h>

namespace m2d
{
namespace ESP32
{
	namespace PSG
	{
		enum BitOrder : uint8_t
		{
			LSBFIRST = 0,
			MSBFIRST = 1
		};

		enum Channel : uint8_t
		{
			Channel1 = 0,
			Channel2 = 1,
			Channel3 = 2,
			Channel4 = 3,
			Channel5 = 4,
			Channel6 = 5
		};

		class Interface
		{
		protected:
			virtual bool validateChannel(uint8_t channel) = 0;
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
}