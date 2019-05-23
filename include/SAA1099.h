#pragma once

namespace m2d
{
namespace ESP32
{
class SAA1099
{
private:
	int latchPin, clockPin, dataPin, WEPin, CSPin, azPin;

public:
	SAA1099(int latch, int clock, int data, int WE, int CS, int AZ) : latchPin(latch)
    , clockPin(clock)
    , dataPin(data)
    , WEPin(WE)
    , CSPin(CS)
    , azPin(AZ)
	{
		gpio_config_t io_conf;
		uint64_t bit_mask = (1ULL<<latchPin) | (1ULL<<clockPin) | (1ULL<<dataPin) | (1ULL<<WEPin) | (1ULL<<CEPin) | (1ULL<<azPin);
		io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
		io_conf.mode = GPIO_MODE_OUTPUT;
		io_conf.pin_bit_mask = bit_mask;
		io_conf.pull_down_en = 0;
		io_conf.pull_up_en = 0;
		gpio_config(&io_conf);
		gpio_set_level(CSPin, 0);
		reset();
	}

	void setNote(byte channel, byte notenum)
	{
		byte o = (note / 12) - 1;
		byte note_val = note - ((o + 1) * 12);
		byte note_addr[] = { 5, 32, 60, 85, 110, 132, 153, 173, 192, 210, 227, 243 };
		setOctave(channel, o);
		setFreq(channel, note_addr[note_val]);
	}

	void setVolume(byte channel, byte volume, byte side)
	{
		byte volL = 0xff & volume;
		byte volR = 0xff & (volume << 4);
		switch (side) {
			case 0:
				writeData(channel, volL);
				break;
			case 1:
				writeData(channel, volR);
				break;
			case 2:
				writeData(channel, volume);
				break;
		}
	}

	void setFreq(byte channel, byte freq)
	{
		writeData((0x08 | channel), freq);
	}

	void setOctave(byte channel, byte octave)
	{
		switch (channel) {
			case 0:
				writeData(0x10, octave);
				break;
			case 1:
				writeData(0x10, octave << 4 & 0xf0);
				break;
			case 2:
				writeData(0x11, octave);
				break;
			case 3:
				writeData(0x11, octave << 4 & 0xf0);
				break;
			case 4:
				writeData(0x12, octave);
				break;
			case 5:
				writeData(0x12, octave << 4 & 0xf0);
				break;
		}
	}
	
	void setFreqEnable(byte channelbit)
	{
		writeData(0x14, channelbit & 0xff);
	}

	void setNoiseEnable(byte channelbit)
	{
		writeData(0x15, channelbit & 0xff);
	}

	void setNoise(byte noiseChannel, byte mode)
	{
		writeData(0x16, mode << ((noiseChannel << 2) & 0x33));
	}

	void setEnvelope(byte envelopeChannel, byte mode)
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

	void writeData(byte address, byte data)
	{
		gpio_set_level(azPin, 1);
		modeWrite();
		gpio_set_level(latchPin, 0);
		shiftOut(dataPin, clockPin, MSBFIRST, address);
		gpio_set_level(latchPin, 1);
		modeInactive();
		gpio_set_level(azPin, 0);
		modeWrite();
		gpio_set_level(latchPin, 0);
		shiftOut(dataPin, clockPin, MSBFIRST, data);
		gpio_set_level(latchPin, 1);
		modeInactive();
	}

};
}
}