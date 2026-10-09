#pragma once

#include "audioFile.h"

class WaveformRenderer {
public:
    WaveformRenderer(SDL_Renderer* renderer, int width, int height, int waveformHeight, int centerY);

    void Draw(const std::vector<WaveformPeak>& waveform);
    void PrintWave(const std::vector<WaveformPeak>& waveform);
    void PrintMarker();

    void SetSize(int width, int height);
    void SetWaveformBounds(int waveformHeight, int centerY);

    void SetSelectionStart(int x);
    void SetSelectionEnd(int x);

    int GetSelectionStartX() const;
    int GetSelectionEndX() const;
    void PrintSelection();

private:
    SDL_Renderer* renderer = nullptr;
    int width = 0;
    int height = 0;
    int waveformHeight = 0;
    int centerY = 0;

    int selectionStartX = 0;
    int selectionEndX = -1;

    SDL_Color waveColor = { 0, 210, 200, 255 };
    SDL_Color selectionColor = { 50, 100, 180, 70 };
    SDL_Color markerColor = { 255, 80, 80, 255 };
    SDL_Color markerColorEnd = { 80, 80, 255, 255 };
};