#pragma once

#include <PSG/PSG.h>

namespace m2d
{
namespace ESP32
{
	class AY38910 : PSG
	{
	private:
		gpio_num_t latchPin, clockPin, dataPin, BC1Pin, BCDIRPin;
		unsigned int tp_ay[128] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4050, 3822, 3608, 3405, 3214, 3034, 2863, 2703, 2551, 2408, 2273, 2145, 2025, 1911, 1804, 1703, 1607, 1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012, 956, 902, 851, 804, 758, 716, 676, 638, 602, 568, 536, 506, 478, 451, 426, 402, 379, 358, 338, 319, 301, 284, 268, 253, 239, 225, 213, 201, 190, 179, 169, 159, 150, 142, 134, 127, 119, 113, 106, 100, 95, 89, 84, 80, 75, 71, 67, 63, 60, 56, 53, 50, 47, 45, 42, 40, 38, 36, 34, 32, 30, 28, 27, 25, 24, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 13, 12, 11, 11, 10
		};

	public:
		AY38910(gpio_num_t latch, gpio_num_t clock, gpio_num_t data, gpio_num_t BC1, gpio_num_t BCDIR)
		    : latchPin(latch)
		    , clockPin(clock)
		    , dataPin(data)
		    , BC1Pin(BC1)
		    , BCDIRPin(BCDIR)
		{
			gpio_config_t io_conf;
			uint64_t bit_mask = (1ULL << latchPin) | (1ULL << clockPin) | (1ULL << dataPin) | (1ULL << BC1Pin) | (1ULL << BCDIRPin);
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
			unsigned int t = this->tp_ay[noteNumber];
			writeData(0x00 + channel, t & 0xff);
			writeData(0x01 + channel, (t >> 8) & 0x0f);
		}

		void setVolume(PSG::Channel channel, uint8_t volume)
		{
			writeData(0x08 + channel, volume & 0x0f);
		}

		void setEnvelope(PSG::Channel channel, uint8_t mode)
		{
			writeData(0x0d, 0x08 + (mode & 0x0f));
		}

		void setEnvelopeTime(uint8_t envelopeTime)
		{
			writeData(0x0b, envelopeTime & 0xff);
			writeData(0x0c, (envelopeTime >> 8) & 0xff);
		}

		void latchMode()
		{
			gpio_set_level(BC1Pin, 1);
			gpio_set_level(BCDIRPin, 1);
		}

		void writeMode()
		{
			gpio_set_level(BC1Pin, 0);
			gpio_set_level(BCDIRPin, 1);
		}

		void invalidate()
		{
			gpio_set_level(BC1Pin, 0);
			gpio_set_level(BCDIRPin, 0);
		}

		void writeData(uint8_t address, uint8_t data)
		{
			invalidate();
			gpio_set_level(latchPin, 0);
			shiftOut(dataPin, clockPin, MSBFIRST, address);
			gpio_set_level(latchPin, 1);
			latchMode();
			invalidate();
			writeMode();
			gpio_set_level(latchPin, 0);
			shiftOut(dataPin, clockPin, MSBFIRST, data);
			gpio_set_level(latchPin, 1);
			invalidate();
		}
	};
}
}
