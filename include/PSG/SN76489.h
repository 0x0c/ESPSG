#pragma once

#include <PSG/PSG.h>

namespace m2d
{
namespace ESP32
{
	namespace PSG
	{

		class SN76489 : Interface
		{
		private:
			unsigned int tp_sn[128] = {
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 992, 937, 884, 834, 788, 743, 702, 662, 625, 590, 557, 526, 496, 468, 442, 417, 394, 372, 351, 331, 313, 295, 278, 263, 248, 234, 221, 209, 197, 186, 175, 166, 156, 147, 139, 131, 124, 117, 110, 104, 98, 93, 88, 83, 78, 74, 70, 66, 62, 59, 55, 52, 49, 46, 44, 41, 39, 37, 35, 33, 31, 29, 28, 26, 25, 23, 22, 21, 20, 18, 17, 16, 16, 15, 14, 13, 12, 12, 11
			};

			gpio_num_t latchPin, clockPin, dataPin, WEPin, CEPin;

		public:
			const static PSG::Channel NoiseChannel = PSG::Channel::Channel4;
			SN76489() {}
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
				return channel < 3;
			}

			void setNote(PSG::Channel channel, uint8_t noteNumber)
			{
				unsigned int t = this->tp_sn[noteNumber];
				modeWrite();
				uint8_t tonedata1 = 0x80 | (0xff & (channel << 4)) | (t & 0x0f);
				writeData(tonedata1);
				uint8_t tonedata2 = (0xff & (t >> 4)) & 0x3f;
				writeData(tonedata2);
				modeInactive();
			}

			void setVolume(PSG::Channel channel, uint8_t volume)
			{
				uint8_t shift = (uint8_t)map(volume, 0, 127, 0, 3);
				uint8_t weight = 0;
				if (volume == 0) {
					weight = 0x0f;
				}
				else {
					weight = (0x01 << shift);
				}
				uint8_t reg = channel_to_register(channel, true) << 4;
				uint8_t volume_bit = 0x80 | reg | weight;
				writeData(volume_bit);
			}

			void setNoise(uint8_t fb, uint8_t nf1, uint8_t nf0)
			{
				uint8_t data = 0xd0 | (fb << 2) | (nf1 << 1) | nf0;
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
				modeInactive();
				modeWrite();
				gpio_set_level(latchPin, 0);
				shiftOut(dataPin, clockPin, MSBFIRST, data);
				// delayMicroseconds(100);
				gpio_set_level(latchPin, 1);
				delayMicroseconds(100);
				modeInactive();
			}

			void clear()
			{
				setVolume(PSG::Channel::Channel1, 0);
				setVolume(PSG::Channel::Channel2, 0);
				setVolume(PSG::Channel::Channel3, 0);
				setVolume(NoiseChannel, 0);
			}

			uint8_t channel_to_register(PSG::Channel channel, bool attenuation)
			{
				if (channel == PSG::Channel::Channel1) {
					return 0b00000000 + (attenuation ? 1 : 0);
				}
				else if (channel == PSG::Channel::Channel2) {
					return 0b00000010 + (attenuation ? 1 : 0);
				}
				else if (channel == PSG::Channel::Channel3) {
					return 0b00000100 + (attenuation ? 1 : 0);
				}
				else if (channel == NoiseChannel) {
					return 0b00000110 + (attenuation ? 1 : 0);
				}

				return 0x00;
			}
		};
	}
}
}