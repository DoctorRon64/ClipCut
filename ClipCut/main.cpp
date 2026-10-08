#include "pch.h"

#include "src/audioFile.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    AudioFile audio;
    if (!audio.load("song.wav")) {
        return 1;
    }

    if (!audio.readSamples()) {
        return 1;
    }

    std::cout << "Sample rate: " << audio.getSampleRate() << '\n';
    std::cout << "Channels: " << audio.getChannels() << '\n';
    std::cout << "Frames: " << audio.getFrameCount() << '\n';

    const auto& samples = audio.getSamples();
    for (size_t i = 0; i < samples.size() && i < 10; i++) {
        std::cout << "Samples[i]" << samples[i] << '\n';
    }

    size_t waveformWidth = 800;
    size_t samplesPerColumn = samples.size() / waveformWidth;
    std::cout << "Samples per column: " << samplesPerColumn << '\n';

    size_t column = 0;
    for (size_t start = 0; start < samples.size(); start += samplesPerColumn) {
        size_t end = start + samplesPerColumn;
        float maxSample = audio.getMaxSample(start, end);

        //        std::cout << "max sample: " << maxSample << '\n';

        std::cout << "column " << column << ": " << maxSample << '\n';
        column++;
    }

    // float maxSample = audio.getMaxSample(0, 1000);
    // std::cout << "Max sample 0-1000: " << maxSample << '\n';

    // float maxSample2 = audio.getMaxSample(1000, 2000);
    // std::cout << "Max sample 1000-2000: " << maxSample2 << '\n';

    SDL_Window* window = SDL_CreateWindow("ClipCut", 800, 600, 0);
    bool running = true;
    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}