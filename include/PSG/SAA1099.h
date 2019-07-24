#pragma once

#include <PSG/PSG.h>

namespace m2d
{
namespace ESP32
{
	namespace PSG
	{
		class SAA1099 : PSG::Interface
		{
		private:
			gpio_num_t latchPin, clockPin, dataPin, WEPin, CSPin, azPin;

		public:
			// See http://www.onasticksoftware.co.uk/dragon/saa1099.pdf, page 14
			enum EnvelopeWaveform : uint8_t
			{
				WaveA = 0b0000,
				WaveB = 0b0010,
				WaveC = 0b0100,
				WaveD = 0b0110,
				WaveE = 0b1000,
				WaveF = 0b1010,
				WaveG = 0b1100,
				WaveH = 0b1110,
				WaveI = 0b1111
			};

			enum OutputSide : uint8_t
			{
				Left = 0,
				Right,
				Both
			};

			SAA1099(gpio_num_t latch, gpio_num_t clock, gpio_num_t data, gpio_num_t WE, gpio_num_t CS, gpio_num_t AZ)
			    : latchPin(latch)
			    , clockPin(clock)
			    , dataPin(data)
			    , WEPin(WE)
			    , CSPin(CS)
			    , azPin(AZ)
			{
				gpio_config_t io_conf;
				uint64_t bit_mask = (1ULL << latchPin) | (1ULL << clockPin) | (1ULL << dataPin) | (1ULL << WEPin) | (1ULL << CSPin) | (1ULL << azPin);
				io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
				io_conf.mode = GPIO_MODE_OUTPUT;
				io_conf.pin_bit_mask = bit_mask;
				io_conf.pull_down_en = (gpio_pulldown_t)0;
				io_conf.pull_up_en = (gpio_pullup_t)0;
				gpio_config(&io_conf);
				gpio_set_level(CSPin, 0);
				reset();
			}

			bool validateChannel(uint8_t channel)
			{
				return channel < 6;
			}

			void setNote(PSG::Channel channel, uint8_t noteNumber)
			{
				uint8_t o = (noteNumber / 12) - 1;
				uint8_t note_val = noteNumber - ((o + 1) * 12);
				uint8_t note_addr[] = { 5, 32, 60, 85, 110, 132, 153, 173, 192, 210, 227, 243 };
				setOctave(channel, o);
				setFreq(channel, note_addr[note_val]);
			}

			void setVolume(PSG::Channel channel, SAA1099::OutputSide side, uint8_t volume)
			{
				uint8_t volL = 0xff & volume;
				uint8_t volR = 0xff & (volume << 4);
				switch (side) {
					case Left:
						writeData(channel, volL);
					case Right:
						writeData(channel, volR);
					case Both:
						writeData(channel, volume);
				}
			}

			void setFreq(PSG::Channel channel, uint8_t freq)
			{
				writeData((0x08 | channel), freq);
			}

			void setOctave(PSG::Channel channel, uint8_t octave)
			{
				switch (channel) {
					case Channel1:
						writeData(0x10, octave);
						break;
					case Channel2:
						writeData(0x10, octave << 4 & 0xf0);
						break;
					case Channel3:
						writeData(0x11, octave);
						break;
					case Channel4:
						writeData(0x11, octave << 4 & 0xf0);
						break;
					case Channel5:
						writeData(0x12, octave);
						break;
					case Channel6:
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

			void setEnvelope(PSG::Channel envelopeChannel, SAA1099::EnvelopeWaveform form)
			{
				writeData(0x18 + envelopeChannel, form);
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
}