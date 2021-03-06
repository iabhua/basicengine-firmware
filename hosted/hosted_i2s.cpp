// SPDX-License-Identifier: MIT
// Copyright (c) 2018, 2019 Ulrich Hecht

extern "C" {
#include <nosdki2s.h>
}
#include <SDL/SDL.h>
#include <sound.h>
#include <malloc.h>

extern unsigned int i2sData[2][I2S_BUFLEN];
static int cubu = 0;

extern FILE *aud_fp;
uint64_t total_samples = 0;

void dump_yuv();

void slc_isr(void *userdata, Uint8 *stream, int len) {
	sound.render();

	int i;
	for (i = 0; i < len; ++i) {
		stream[i] = nosdk_i2s_curr_buf[i];
	}
	if (aud_fp)
	  fwrite(stream, len, 1, aud_fp);
        total_samples += len;

	cubu ^= 1;
	nosdk_i2s_curr_buf = i2sData[cubu];
	nosdk_i2s_curr_buf_pos = 0;
}

extern int hosting_mem_allocated;

int sound_reinit_rate = 0;

void reinit_sound()
{
	static bool inited = false;

	SDL_LockAudio();
	SDL_PauseAudio(1);

	if (inited)
		SDL_CloseAudio();
	SDL_UnlockAudio();

	nosdk_i2s_clear_buf();
	cubu = 0;
	nosdk_i2s_curr_buf = i2sData[cubu];
	nosdk_i2s_curr_buf_pos = 0;

	SDL_AudioSpec as, obtained;
	as.freq = sound_reinit_rate;
	as.format = AUDIO_U8;
	as.channels = 1;
	// This doesn't make sense, but when I request I2S_BUFLEN, I get
	// a buffer half the size. WTF?
	as.samples = I2S_BUFLEN*2;
	as.callback = slc_isr;

	int mem_before = mallinfo().uordblks;
	
	if (SDL_OpenAudio(&as, &obtained) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(1);
	}

	if (obtained.samples != I2S_BUFLEN)
		fprintf(stderr, "Odd audio buffer size, not starting.");
	else
		SDL_PauseAudio(0);

	if (!inited)
		hosting_mem_allocated += mallinfo().uordblks - mem_before;

	inited = true;
	sound_reinit_rate = 0;
}

extern "C" void InitI2S(uint32_t samplerate)
{
	sound_reinit_rate = samplerate;
}

void SendI2S()
{
}

const uint32_t fakePwm[] = {
	  0,   8,  16,  25,  33,  41,  49,  58,
	 66,  74,  82,  90,  99, 107, 115, 123,
	132, 140, 148, 156, 165, 173, 181, 189,
	197, 206, 214, 222, 230, 239, 247, 255,
};
