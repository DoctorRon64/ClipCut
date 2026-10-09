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
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);
    ma_result result = ma_decoder_init_file(path, &config, &decoder);

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

float AudioFile::getMaxSample(size_t minimum, size_t maximum) const {
    float maxSample = 0.0f;
    maximum = std::min(maximum, samples.size());
    for (size_t i = minimum; i < maximum; ++i) {
        float magnitude = std::abs(samples[i]);
        maxSample = std::max(maxSample, magnitude);
    }
    return maxSample;
}

size_t AudioFile::getColumnCount(size_t samplesPerColumn) const {
    if (samplesPerColumn == 0 || samples.empty()) {
        return 0;
    }

    return (samples.size() + samplesPerColumn - 1) / samplesPerColumn;
}

std::vector<float> AudioFile::getWaveformData(size_t samplesPerColumn) const {
    std::vector<float> waveform;

    if (samplesPerColumn == 0 || samples.empty()) {
        return waveform;
    }

    waveform.reserve(getColumnCount(samplesPerColumn));

    for (size_t start = 0; start < samples.size(); start += samplesPerColumn) {
        size_t end = start + samplesPerColumn;
        float maxSample = getMaxSample(start, end);

        waveform.push_back(maxSample);
    }

    return waveform;
}

std::vector<WaveformPeak> AudioFile::getWaveformPeaks(size_t samplesPerColumn) const {
    std::vector<WaveformPeak> peaks;

    if (samples.empty())
        return peaks;

    samplesPerColumn = std::max<size_t>(1, samplesPerColumn);

    peaks.reserve((samples.size() + samplesPerColumn - 1) / samplesPerColumn);
    for (size_t start = 0; start < samples.size(); start += samplesPerColumn) {
        const size_t end = std::min(
            start + samplesPerColumn,
            samples.size()
        );

        float minSample = 0.0f;
        float maxSample = 0.0f;

        for (size_t i = start; i < end; ++i) {
            minSample = std::min(minSample, samples[i]);
            maxSample = std::max(maxSample, samples[i]);
        }

        peaks.push_back({ minSample, maxSample });
    }

    return peaks;
}

bool AudioFile::readSamples() {
    std::cout << "Output format: " << decoder.outputFormat << '\n';
    samples.resize(frameCount * decoder.outputChannels);
    ma_uint64 framesRead = 0;
    ma_result result = ma_decoder_read_pcm_frames(&decoder, samples.data(), frameCount, &framesRead);

    std::cout << "Frames read: " << framesRead << '\n';
    std::cout << "First raw bytes: ";

    unsigned char* bytes = reinterpret_cast<unsigned char*>(samples.data());

    for (int i = 0; i < 16; i++) {
        std::cout << static_cast<int>(bytes[i]) << ' ';
    }

    std::cout << '\n';

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