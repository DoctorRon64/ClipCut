#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "audioFile.h"


class WaveformRenderer {
public:
    WaveformRenderer(SDL_Renderer* renderer, int width, int height, int waveformHeight, int centerY);

    void PrintWave(const std::vector<WaveformPeak>& waveform);
    void SetSize(int width, int height);
    void SetWaveformBounds(int waveformHeight, int centerY);

private:
    SDL_Renderer* renderer = nullptr;
    int width = 0;
    int height = 0;
    int waveformHeight = 0;
    int centerY = 0;
};

