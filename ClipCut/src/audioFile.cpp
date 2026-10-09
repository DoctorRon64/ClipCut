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

bool AudioFile::exportRange(
    const char* path,
    size_t startFrame,
    size_t endFrame
) const {
    const size_t channels = getChannels();
    const size_t frameCount = getFrameCount();

    if (channels == 0 ||
        startFrame >= endFrame ||
        endFrame > frameCount) {
        return false;
    }

    const float* startSample = samples.data() + startFrame * channels;
    const size_t selectedFrames = endFrame - startFrame;

    ma_encoder_config config = ma_encoder_config_init(
        ma_encoding_format_wav,
        ma_format_f32,
        static_cast<ma_uint32>(channels),
        getSampleRate()
    );

    ma_encoder encoder;

    if (ma_encoder_init_file(path, &config, &encoder) != MA_SUCCESS) {
        return false;
    }

    ma_uint64 framesWritten = 0;

    float minSample = 1.0f;
    float maxSample = -1.0f;

    for (size_t i = 0; i < selectedFrames * channels; i++) {
        minSample = std::min(minSample, startSample[i]);
        maxSample = std::max(maxSample, startSample[i]);
    }

    std::cout << "Export sample range: "
        << minSample << " to " << maxSample << '\n';

    std::cout << "Channels: " << channels
        << ", Sample rate: " << getSampleRate()
        << ", Frames: " << selectedFrames << '\n';


    for (size_t i = 0; i < 10 && i < selectedFrames * channels; i++) {
        std::cout << startSample[i] << ' ';
    }
    std::cout << '\n';


    ma_result result = ma_encoder_write_pcm_frames(
        &encoder,
        startSample,
        selectedFrames,
        &framesWritten
    );

    std::cout << "Encoder format: " << config.format << '\n';
    std::cout << "Encoder channels: " << config.channels << '\n';
    std::cout << "Encoder sample rate: " << config.sampleRate << '\n';

    std::cout << "Export successful: clip.wav\n";
    std::cout << "Encoder result: " << result
        << ", frames written: " << framesWritten
        << " / " << selectedFrames << '\n';


    ma_encoder_uninit(&encoder);

    return result == MA_SUCCESS && framesWritten == selectedFrames;
}