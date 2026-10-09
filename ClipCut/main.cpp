#include "pch.h"

#include "src/audioFile.h"
#include "src/waveformRenderer.h"

size_t ScreenXToFrame(int x, int waveformWidth, size_t frameCount) {
    x = std::clamp(x, 0, waveformWidth);
    return static_cast<size_t>((static_cast<double>(x) / waveformWidth) * frameCount);
}

int FrameToScreenX(size_t frame, size_t frameCount, int waveformWidth) {
    if (frameCount == 0) {
        return 0;
    }

    return static_cast<int>((static_cast<double>(frame) / frameCount) * waveformWidth);
}

size_t FindNearestZeroCrossing(
    const std::vector<float>& samples,
    size_t targetFrame,
    size_t frameCount,
    size_t channels,
    size_t searchRadius
) {
    if (channels == 0 || frameCount < 2) {
        return targetFrame;
    }

    size_t availableFrames = std::min(
        frameCount,
        samples.size() / channels
    );

    if (availableFrames < 2) {
        return targetFrame;
    }

    targetFrame = std::min(targetFrame, availableFrames - 1);

    size_t searchStart = (targetFrame > searchRadius)
        ? targetFrame - searchRadius
        : 0;

    size_t searchEnd = std::min(
        availableFrames - 1,
        targetFrame + std::min(searchRadius, availableFrames - 1 - targetFrame)
    );

    size_t bestFrame = targetFrame;
    size_t bestDistance = searchRadius + 1;

    for (size_t i = searchStart; i < searchEnd; i++) {
        float current = samples[i * channels];
        float next = samples[(i + 1) * channels];

        // Ignore flat sections where both samples are exactly zero.
        if (current == 0.0f && next == 0.0f) {
            continue;
        }

        if ((current <= 0 && next >= 0) ||
            (current >= 0 && next <= 0)) {

            size_t distance = (i > targetFrame)
                ? i - targetFrame
                : targetFrame - i;

            if (distance < bestDistance) {
                bestDistance = distance;
                bestFrame = i;
            }
        }
    }

    return bestFrame;
}

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

    bool success = audio.exportRange(
        "full_test.wav",
        0,
        static_cast<size_t>(audio.getFrameCount())
    );

    std::cout << (success
        ? "Full export succeeded\n"
        : "Full export failed\n");

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

    std::vector<ma_uint64> slicePoints;
    const int waveformHeight = 200;
    const int centerY = 300;
    WaveformRenderer waveformRenderer(renderer, static_cast<int>(windowWidth), static_cast<int>(windowHeight), waveformHeight, centerY);

    // Generate waveform data once.
    const size_t samplesPerColumn = std::max<size_t>(1, (audio.getSamples().size() + waveformWidth - 1) / waveformWidth);
    const std::vector<WaveformPeak> waveform = audio.getWaveformPeaks(samplesPerColumn);

    bool draggingStart = false;
    bool draggingEnd = false;
    bool running = true;
    bool hasSelection = false;

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                event.button.button == SDL_BUTTON_LEFT) {

                int mouseX = static_cast<int>(event.button.x);

                if (!hasSelection) {
                    waveformRenderer.SetSelectionStart(mouseX);
                    waveformRenderer.SetSelectionEnd(mouseX);
                    hasSelection = true;
                    draggingEnd = true;
                }
                else {
                    int startDistance = std::abs(mouseX - waveformRenderer.GetSelectionStartX());
                    int endDistance = std::abs(mouseX - waveformRenderer.GetSelectionEndX());

                    if (startDistance <= endDistance) {
                        draggingStart = true;
                    }
                    else {
                        draggingEnd = true;
                    }
                }
            }

            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                int mouseX = static_cast<int>(event.motion.x);

                if (draggingStart) {
                    waveformRenderer.SetSelectionStart(mouseX);
                }
                else if (draggingEnd) {
                    waveformRenderer.SetSelectionEnd(mouseX);
                }
            }


            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
                event.button.button == SDL_BUTTON_LEFT) {

                if (draggingStart || draggingEnd) {
                    size_t startFrame = ScreenXToFrame(
                        waveformRenderer.GetSelectionStartX(),
                        static_cast<int>(windowWidth),
                        static_cast<size_t>(audio.getFrameCount())
                    );

                    size_t endFrame = ScreenXToFrame(
                        waveformRenderer.GetSelectionEndX(),
                        static_cast<int>(windowWidth),
                        static_cast<size_t>(audio.getFrameCount())
                    );

                    if (draggingStart) {
                        startFrame = FindNearestZeroCrossing(
                            audio.getSamples(), startFrame,
                            audio.getFrameCount(), audio.getChannels(), 200
                        );
                    }
                    else {
                        endFrame = FindNearestZeroCrossing(
                            audio.getSamples(), endFrame,
                            audio.getFrameCount(), audio.getChannels(), 200
                        );
                    }

                    if (endFrame > startFrame) {
                        bool success = audio.exportRange(
                            "clip.wav", startFrame, endFrame
                        );

                        std::cout << (success
                            ? "Export successful: clip.wav\n"
                            : "Export failed!\n");
                    }
                    else {
                        std::cout << "Export skipped: selection is empty.\n";
                    }
                }

                draggingStart = false;
                draggingEnd = false;
            }
        }

        //background
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        //waveform
        SDL_SetRenderDrawColor(renderer, 0, 210, 200, 255);
        waveformRenderer.Draw(waveform);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}