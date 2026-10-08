#include "../pch.h"
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "audioFile.h"

AudioFile::AudioFile() {
}

AudioFile::~AudioFile() {
    ma_decoder_uninit(&decoder);
}

bool AudioFile::load(const char* path) {
    ma_result result = ma_decoder_init_file(path, nullptr, &decoder);
    if (result != MA_SUCCESS) {
        std::cout << "Failed to initialize decoder\n";
        return false;
    }

    result = ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount);
    if (result != MA_SUCCESS) {
        std::cout << "Failed to get frame count\n";
        return false;
    }

    return true;
}

bool AudioFile::readSamples() {
    samples.resize(frameCount * decoder.outputChannels);
    ma_uint64 framesRead = 0;
    ma_result result = ma_decoder_read_pcm_frames(&decoder, samples.data(), frameCount, &framesRead);

    if (result != MA_SUCCESS) {
        std::cout << "Failed to read samples\n";
        return false;
    }

    std::cout << "Frames read: " << framesRead << '\n';

    return true;
}

const std::vector<float>& AudioFile::getSamples() const {
    return samples;
}

ma_uint64 AudioFile::getFrameCount() const {
    return frameCount;
}

ma_uint32 AudioFile::getSampleRate() const {
    return decoder.outputSampleRate;
}

ma_uint32 AudioFile::getChannels() const {
    return decoder.outputChannels;
}