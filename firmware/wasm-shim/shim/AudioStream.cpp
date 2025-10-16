#include "AudioStream.h"

uint16_t AudioStream::cpu_cycles_total = 0;
uint16_t AudioStream::cpu_cycles_total_max = 0;
uint16_t AudioStream::memory_used = 0;
uint16_t AudioStream::memory_used_max = 0;
AudioConnection *AudioStream::unused = NULL;
bool AudioStream::update_scheduled = false;
AudioStream *AudioStream::first_update = NULL;
audio_block_t *AudioStream::memory_pool = NULL;
uint32_t AudioStream::memory_pool_available_mask[] = {};
uint16_t AudioStream::memory_pool_first_mask = 0;