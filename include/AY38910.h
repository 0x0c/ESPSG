#pragma once

#include <PSG.h>

namespace m2d
{
class AY38910 : PSG
{
private:
	gpio_num_t latchPin, clockPin, dataPin, BC1Pin, BCDIRPin;
	
public:
	AY38910(gpio_num_t latch, gpio_num_t clock, gpio_num_t data, gpio_num_t BC1, gpio_num_t BCDIR) : latchPin(latch)
    , clockPin(clock)
    , dataPin(data)
    , BC1Pin(BC1)
    , BCDIRPin(BCDIR)
	{
		gpio_config_t io_conf;
		uint64_t bit_mask = (1ULL<<latchPin) | (1ULL<<clockPin) | (1ULL<<dataPin) | (1ULL<<BC1Pin) | (1ULL<<BCDIRPin);
		io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
		io_conf.mode = GPIO_MODE_OUTPUT;
		io_conf.pin_bit_mask = bit_mask;
		io_conf.pull_down_en = 0;
		io_conf.pull_up_en = 0;
		gpio_config(&io_conf);
	}

	void setNote(Channel channel, uint8_t noteNumber) {
		unsigned int t = tp_ay[noteNumber];
		writeData(0x00 + channel, t & 0xff);
		writeData(0x01 + channel, (t >> 8) & 0x0f);
	}

	void setVolume(Channel channel, uint8_t volume) {
		writeData(0x08 + channel, volume & 0x0f);
	}

	void setEnvelope(uint8_t mode) {
		writeData(0x0d, 0x08 + mode & 0x0f);
	}

	void setEnvelopeTime(uint8_t envelopeTime) {
		writeData(0x0b, envelopeTime & 0xff);
		writeData(0x0c, (envelopeTime >> 8) & 0xff);
	}

	void latchMode() {
		gpio_set_level(BC1Pin, 1);
		gpio_set_level(BCDIRPin, 1);
	}

	void writeMode() {
		gpio_set_level(BC1Pin, 0);
		gpio_set_level(BCDIRPin, 1);
	}

	void invalidate() {
		gpio_set_level(BC1Pin, 0);
		gpio_set_level(BCDIRPin, 0);
	}

	void writeData(uint8_t address, uint8_t data) {
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
