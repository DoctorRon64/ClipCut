#include "pch.h"

#include "src/audioFile.h"
#include "src/waveformRenderer.h"

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL initialization failed: "
            << SDL_GetError() << '\n';
        return 1;
    }

    AudioFile audio;

    if (!audio.load("song.wav")) {
        SDL_Quit();
        return 1;
    }

    if (!audio.readSamples()) {
        SDL_Quit();
        return 1;
    }

    const auto& samples = audio.getSamples();
    for (size_t i = 0; i < samples.size() && i < 20; ++i) {
        std::cout << "Sample " << i << ": " << samples[i] << '\n';
    }

    std::cout << "Sample rate: " << audio.getSampleRate() << '\n';
    std::cout << "Channels: " << audio.getChannels() << '\n';
    std::cout << "Frames: " << audio.getFrameCount() << '\n';

    const size_t windowWidth = 800;
    const size_t windowHeight = 600;
    const size_t waveformWidth = windowWidth;

    SDL_Window* window = SDL_CreateWindow("ClipCut", static_cast<int>(windowWidth), static_cast<int>(windowHeight), 0);

    if (!window) {
        std::cout << "Window creation failed: "
            << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer) {
        std::cout << "Renderer failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const int waveformHeight = 200;
    const int centerY = 300;

    WaveformRenderer waveformRenderer(renderer, static_cast<int>(windowWidth), static_cast<int>(windowHeight), waveformHeight, centerY);

    // Generate waveform data once.
    const size_t samplesPerColumn = std::max<size_t>(1, (audio.getSamples().size() + waveformWidth - 1) / waveformWidth);
    const std::vector<WaveformPeak> waveform = audio.getWaveformPeaks(samplesPerColumn);

    bool running = true;

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 220, 140, 255);
        waveformRenderer.PrintWave(waveform);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}