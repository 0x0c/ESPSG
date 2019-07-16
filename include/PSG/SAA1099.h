#pragma once

#include <PSG/PSG.h>

namespace m2d
{
namespace ESP32
{
class SAA1099 : PSG
{
private:
	gpio_num_t latchPin, clockPin, dataPin, WEPin, CSPin, azPin;

public:
	SAA1099(gpio_num_t latch, gpio_num_t clock, gpio_num_t data, gpio_num_t WE, gpio_num_t CS, gpio_num_t AZ) : latchPin(latch)
    , clockPin(clock)
    , dataPin(data)
    , WEPin(WE)
    , CSPin(CS)
    , azPin(AZ)
	{
		gpio_config_t io_conf;
		uint64_t bit_mask = (1ULL<<latchPin) | (1ULL<<clockPin) | (1ULL<<dataPin) | (1ULL<<WEPin) | (1ULL<<CSPin) | (1ULL<<azPin);
		io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
		io_conf.mode = GPIO_MODE_OUTPUT;
		io_conf.pin_bit_mask = bit_mask;
		io_conf.pull_down_en = (gpio_pulldown_t)0;
		io_conf.pull_up_en = (gpio_pullup_t)0;
		gpio_config(&io_conf);
		gpio_set_level(CSPin, 0);
		reset();
	}

	void setNote(PSG::Channel channel, uint8_t noteNumber)
	{
		uint8_t o = (noteNumber / 12) - 1;
		uint8_t note_val = noteNumber - ((o + 1) * 12);
		uint8_t note_addr[] = { 5, 32, 60, 85, 110, 132, 153, 173, 192, 210, 227, 243 };
		setOctave(channel, o);
		setFreq(channel, note_addr[note_val]);
	}

	void setVolume(PSG::Channel channel, uint8_t volume)
	{
		uint8_t volL = 0xff & volume;
		uint8_t volR = 0xff & (volume << 4);
		switch (channel) {
			case c0:
				writeData(channel, volL);
				break;
			case c1:
				writeData(channel, volR);
				break;
			case c2:
				writeData(channel, volume);
				break;
			default:
				break;
		}
	}

	void setFreq(PSG::Channel channel, uint8_t freq)
	{
		writeData((0x08 | channel), freq);
	}

	void setOctave(PSG::Channel channel, uint8_t octave)
	{
		switch (channel) {
			case c0:
				writeData(0x10, octave);
				break;
			case c1:
				writeData(0x10, octave << 4 & 0xf0);
				break;
			case c2:
				writeData(0x11, octave);
				break;
			case c3:
				writeData(0x11, octave << 4 & 0xf0);
				break;
			case c4:
				writeData(0x12, octave);
				break;
			case c5:
				writeData(0x12, octave << 4 & 0xf0);
				break;
		}
	}
	
	void setFreqEnable(uint8_t channelbit)
	{
		writeData(0x14, channelbit & 0xff);
	}

	void setNoiseEnable(uint8_t channelbit)
	{
		writeData(0x15, channelbit & 0xff);
	}

	void setNoise(PSG::Channel noiseChannel, uint8_t mode)
	{
		writeData(0x16, mode << ((noiseChannel << 2) & 0x33));
	}

	void setEnvelope(PSG::Channel envelopeChannel, uint8_t mode)
	{
		writeData(0x18 + envelopeChannel, mode);
	}

	void soundEnable()
	{
		writeData(0x1C, 0x01);
	}

	void mute()
	{
		writeData(0x1C, 0x00);
	}

	void reset()
	{
		writeData(0x1C, 0x02);
	}

	void modeWrite()
	{
		gpio_set_level(WEPin, 0);
	}

	void modeInactive()
	{
		gpio_set_level(WEPin, 1);
	}

	void writeData(uint8_t address, uint8_t data)
	{
		gpio_set_level(azPin, 1);
		modeWrite();
		gpio_set_level(latchPin, 0);
		shiftOut(dataPin, clockPin, PSG::BitOrder::MSBFIRST, address);
		gpio_set_level(latchPin, 1);
		modeInactive();
		gpio_set_level(azPin, 0);
		modeWrite();
		gpio_set_level(latchPin, 0);
		shiftOut(dataPin, clockPin, PSG::BitOrder::MSBFIRST, data);
		gpio_set_level(latchPin, 1);
		modeInactive();
	}

};
}
}