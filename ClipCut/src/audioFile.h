#pragma once

#include <miniaudio.h>

class AudioFile {
public:
    AudioFile();
    ~AudioFile();

    bool load(const char* path);

    bool readSamples();
    const std::vector<float>& getSamples() const;
    float getMaxSample(size_t minimum, size_t maximum) const;
    ma_uint64 getFrameCount() const;
    ma_uint32 getSampleRate() const;
    ma_uint32 getChannels() const;

private:
    ma_decoder decoder;
    ma_uint64 frameCount;

    std::vector<float> samples;
};