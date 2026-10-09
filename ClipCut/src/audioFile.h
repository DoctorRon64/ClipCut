#pragma once

#include <miniaudio.h>

struct WaveformPeak {
    float min;
    float max;
};

class AudioFile {
public:
    AudioFile();
    ~AudioFile();

    bool load(const char* path);
    bool readSamples();

    const std::vector<float>& getSamples() const;

    float getMaxSample(size_t minimum, size_t maximum) const;
    std::vector<float> getWaveformData(size_t samplesPerColumn) const;
    std::vector<WaveformPeak> getWaveformPeaks(size_t samplesPerColumn) const;
    size_t getColumnCount(size_t samplesPerColumn) const;

    ma_uint64 getFrameCount() const;
    ma_uint32 getSampleRate() const;
    ma_uint32 getChannels() const;

    bool exportRange(const char* path, size_t startFrame, size_t endFrame) const;

private:
    ma_decoder decoder{};
    ma_uint64 frameCount = 0;

    std::vector<float> samples;
};