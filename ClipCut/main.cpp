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
    std::cout << "Samples vector size: " << samples.size() << '\n';
    for (size_t i = 0; i < samples.size() && i < 100; i++) {
        std::cout << samples[i] << '\n';
    }

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