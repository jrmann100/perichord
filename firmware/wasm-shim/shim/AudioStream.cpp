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

AudioConnection::AudioConnection()
    : src(nullptr),
      dst(nullptr),
      src_index(0),
      dest_index(0),
      next_dest(nullptr),
      isConnected(false)
{
    // On real hardware this constructor registers the connection so that
    // AudioConnection::connect can splice it into the audio graph. In the
    // WebAssembly shim we only cache the pointers so the program can link.
}

AudioConnection::~AudioConnection()
{
    // The Teensy audio core removes the link between nodes here. We do not
    // build the connection list in the shim, so there is nothing to release.
    disconnect();
}

int AudioConnection::connect(void)
{
    // Hardware implementation inserts the connection into the source list and
    // increments bookkeeping counters. For the shim we simply flag success so
    // higher level code continues to run.
    isConnected = (src != nullptr && dst != nullptr);
    return isConnected ? 1 : 0;
}

int AudioConnection::connect(AudioStream &source, unsigned char sourceOutput,
                             AudioStream &destination, unsigned char destinationInput)
{
    // In the firmware this records routing information for the audio graph so
    // audio blocks flow between objects. We only remember the pointers and
    // mark the connection as active.
    src = &source;
    dst = &destination;
    src_index = sourceOutput;
    dest_index = destinationInput;
    next_dest = nullptr;
    return connect();
}

int AudioConnection::disconnect(void)
{
    // Teensy audio removes the node from the source's destination list. The
    // shim just clears the bookkeeping so any future connect() can succeed.
    isConnected = false;
    src = nullptr;
    dst = nullptr;
    src_index = 0;
    dest_index = 0;
    next_dest = nullptr;
    return 1;
}

void AudioStream::initialize_memory(audio_block_t *data, unsigned int num)
{
    // The Teensy core hands a statically allocated pool to the audio engine
    // here so that allocate()/release() can hand out buffers. The shim does not
    // process real audio, but we keep track of the pointer for completeness so
    // diagnostics that inspect the pool do not crash.
    memory_pool = data;
    (void)num; // pool size is unused in the shim implementation.
    memory_used = 0;
    memory_used_max = 0;
}

audio_block_t *AudioStream::allocate(void)
{
    // Real firmware would pop a buffer from the shared pool. Returning nullptr
    // keeps the shim lightweight while allowing code that checks for failed
    // allocations to continue.
    return nullptr;
}

void AudioStream::release(const audio_block_t *block)
{
    // On-device this decrements the reference count and returns the block to
    // the pool. Nothing was allocated in the shim, so there is nothing to do.
    (void)block;
}

void AudioStream::transmit(audio_block_t *block, unsigned char index)
{
    // The firmware pushes the audio block to every downstream connection. The
    // shim performs no routing, but clearing the ref count mirrors the release
    // that normally happens after transmission.
    (void)index;
    if (block != nullptr)
    {
        block->ref_count = 0;
    }
}

audio_block_t *AudioStream::receiveReadOnly(unsigned int index)
{
    // Hardware nodes pull from their input queues here. We do not generate
    // audio in the shim, so we always report that no block is available.
    (void)index;
    return nullptr;
}

audio_block_t *AudioStream::receiveWritable(unsigned int index)
{
    // Writable access requests a unique buffer; the shim has no audio data, so
    // we mirror the read-only stub and return nullptr.
    (void)index;
    return nullptr;
}