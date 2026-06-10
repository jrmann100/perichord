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
uint16_t AudioStream::memory_pool_size = 0;
AudioConnection *AudioStream::first_connection = NULL;

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
    // Store the routing information
    src = &source;
    dst = &destination;
    src_index = sourceOutput;
    dest_index = destinationInput;

    // Add to global connection list
    next_dest = AudioStream::first_connection;
    AudioStream::first_connection = this;

    // Mark both source and destination as active
    source.active = true;
    destination.active = true;

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
    // Store the memory pool and size
    memory_pool = data;
    memory_pool_size = num;
    memory_used = 0;
    memory_used_max = 0;

    // Initialize all blocks as available
    for (unsigned int i = 0; i < num; i++)
    {
        data[i].ref_count = 0;
        data[i].memory_pool_index = i;
    }
}

audio_block_t *AudioStream::allocate(void)
{
    // Find a free block in the pool
    if (memory_pool == nullptr || memory_pool_size == 0)
        return nullptr;

    for (uint16_t i = 0; i < memory_pool_size; i++)
    {
        if (memory_pool[i].ref_count == 0)
        {
            memory_pool[i].ref_count = 1;
            memory_used++;
            if (memory_used > memory_used_max)
                memory_used_max = memory_used;
            return &memory_pool[i];
        }
    }

    return nullptr;
}

void AudioStream::release(const audio_block_t *block)
{
    // Decrement reference count and return to pool
    if (block != nullptr)
    {
        audio_block_t *b = const_cast<audio_block_t *>(block);
        if (b->ref_count > 0)
        {
            b->ref_count--;
            if (b->ref_count == 0)
            {
                memory_used--;
            }
        }
    }
}

void AudioStream::transmit(audio_block_t *block, unsigned char index)
{
    // Push the audio block to all downstream connections
    if (block == nullptr)
        return;

    AudioConnection *conn = AudioStream::first_connection;
    while (conn)
    {
        if (conn->src == this && conn->src_index == index && conn->isConnected)
        {
            // Route this block to the destination's input queue. Like the
            // hardware library, only fill an empty slot — the destination
            // takes ownership of the queued block when it receives it.
            if (conn->dst && conn->dest_index < conn->dst->num_inputs)
            {
                if (conn->dst->inputQueue &&
                    conn->dst->inputQueue[conn->dest_index] == nullptr)
                {
                    conn->dst->inputQueue[conn->dest_index] = block;
                    block->ref_count++; // Increment for this destination
                }
            }
        }
        conn = conn->next_dest;
    }
}

audio_block_t *AudioStream::receiveReadOnly(unsigned int index)
{
    // Take the block out of the input queue, transferring ownership (and the
    // queue's reference) to the caller, which must release() it when done.
    // Leaving the pointer in the queue would replay the same block forever
    // once the upstream source stops transmitting.
    if (index < num_inputs && inputQueue != nullptr)
    {
        audio_block_t *in = inputQueue[index];
        inputQueue[index] = nullptr;
        return in;
    }
    return nullptr;
}

audio_block_t *AudioStream::receiveWritable(unsigned int index)
{
    // Like receiveReadOnly, but if other streams also hold a reference, hand
    // back a private copy so in-place modification can't corrupt their data.
    if (index >= num_inputs || inputQueue == nullptr)
        return nullptr;

    audio_block_t *in = inputQueue[index];
    inputQueue[index] = nullptr;
    if (in && in->ref_count > 1)
    {
        audio_block_t *p = allocate();
        if (p)
            memcpy(p->data, in->data, sizeof(p->data));
        release(in);
        in = p;
    }
    return in;
}

void AudioStream::update_all(void)
{
    // Walk through the linked list of all AudioStream objects and call update()
    // on each one. This simulates the Teensy Audio library's interrupt-driven
    // update cycle in a synchronous manner for the web worklet.
    AudioStream *p = first_update;
    while (p)
    {
        if (p->active)
        {
            p->update();
        }
        p = p->next_update;
    }
}