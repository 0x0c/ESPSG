#pragma once

#include <PSG/PSG.h>

namespace m2d
{
namespace ESP32
{
	class SN76489 : PSG
	{
	private:
		unsigned int tp_sn[128] = {
			956, 902, 851, 804, 758, 716, 676, 638, 602, 568, 536, 506, 478, 451, 426, 402, 379, 358, 338, 319, 301, 284, 268, 253, 239, 225, 213, 201, 190, 179, 169, 159, 150, 142, 134, 127, 119, 113, 106, 100, 95, 89, 84, 80, 75, 71, 6, 1012, 956, 902, 851, 804, 758, 716, 676, 638, 602, 568, 536, 506, 478, 451, 426, 402, 379, 358, 338, 319, 301, 284, 268, 253, 239, 225, 213, 201, 190, 179, 169, 159, 150, 142, 134, 127, 119, 113, 106, 100, 95, 89, 84, 80, 75, 71, 67, 63, 60, 56, 53, 50, 47, 45, 42, 40, 38, 36, 34, 32, 30, 28, 27, 25, 24, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 13, 12, 11, 11, 10
		};

		gpio_num_t latchPin, clockPin, dataPin, WEPin, CEPin;

	public:
		SN76489(gpio_num_t latch, gpio_num_t clock, gpio_num_t data, gpio_num_t WE, gpio_num_t CE)
		    : latchPin(latch)
		    , clockPin(clock)
		    , dataPin(data)
		    , WEPin(WE)
		    , CEPin(CE)
		{
			gpio_config_t io_conf;
			uint64_t bit_mask = (1ULL << latchPin) | (1ULL << clockPin) | (1ULL << dataPin) | (1ULL << WEPin) | (1ULL << CEPin);
			io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
			io_conf.mode = GPIO_MODE_OUTPUT;
			io_conf.pin_bit_mask = bit_mask;
			io_conf.pull_down_en = (gpio_pulldown_t)0;
			io_conf.pull_up_en = (gpio_pullup_t)0;
			gpio_config(&io_conf);
		}

		bool validateChannel(uint8_t channel)
		{
			return channel <= 3;
		}

		void setNote(PSG::Channel channel, uint8_t noteNumber)
		{
			unsigned int t = this->tp_sn[noteNumber];
			uint8_t tonedata1 = 0x80 | (channel << 5) | (t & 0xf);
			writeData(tonedata1);
			uint8_t tonedata2 = (t >> 4) & 0x3f;
			writeData(tonedata2);
		}

		void setVolume(PSG::Channel channel, uint8_t volume)
		{
			uint8_t vol;
			vol = 0x90 | (channel << 5) | ((~volume) & 0xf);
			writeData(vol);
		}

		void setNoise(uint8_t fb, uint8_t f1, uint8_t f0)
		{
			uint8_t data = 0xd0 | (fb << 2) | (f1 << 1) | f0;
			writeData(data);
		}

		void modeWrite()
		{
			gpio_set_level(CEPin, 0);
			gpio_set_level(WEPin, 0);
			delayMicroseconds(20);
		}

		void modeInactive()
		{
			gpio_set_level(CEPin, 1);
			gpio_set_level(WEPin, 1);
			delayMicroseconds(20);
		}

		void writeData(uint8_t data)
		{
			modeWrite();
			gpio_set_level(latchPin, 0);
			shiftOut(dataPin, clockPin, LSBFIRST, data);
			gpio_set_level(latchPin, 1);
			delayMicroseconds(100);
			modeInactive();
		}

		void clear()
		{
			setVolume(PSG::Channel::c1, 0);
			setVolume(PSG::Channel::c2, 0);
			setVolume(PSG::Channel::c3, 0);
			setVolume(PSG::Channel::c4, 0);
		}
	};
}
}