#include "../pch.h"
#include "waveformRenderer.h"

WaveformRenderer::WaveformRenderer(SDL_Renderer* renderer, int width, int height, int waveformHeight, int centerY)
    : renderer(renderer), width(width), height(height), waveformHeight(waveformHeight), centerY(centerY) {
}

void WaveformRenderer::SetSize(int width, int height) {
    this->width = width;
    this->height = height;
}

void WaveformRenderer::SetWaveformBounds(int waveformHeight, int centerY) {
    this->waveformHeight = waveformHeight;
    this->centerY = centerY;
}

void WaveformRenderer::PrintWave(const std::vector<WaveformPeak>& waveform)
{
    if (waveform.empty() || width <= 0 || height <= 0)
        return;

    SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);

    for (int x = 0; x < width; ++x)
    {
        // Map this screen pixel to the waveform data.
        size_t start = static_cast<size_t>(
            (static_cast<double>(x) / width) * waveform.size()
            );

        size_t end = static_cast<size_t>(
            (static_cast<double>(x + 1) / width) * waveform.size()
            );

        end = std::max(start + 1, end);
        end = std::min(end, waveform.size());

        float minSample = 0.0f;
        float maxSample = 0.0f;

        // Combine waveform bins that fall within this pixel.
        for (size_t i = start; i < end; ++i)
        {
            minSample = std::min(minSample, waveform[i].min);
            maxSample = std::max(maxSample, waveform[i].max);
        }

        // Convert sample amplitudes (-1.0 to 1.0) to screen coordinates.
        int yTop = centerY - static_cast<int>(maxSample * waveformHeight / 2.0f);
        int yBottom = centerY - static_cast<int>(minSample * waveformHeight / 2.0f);

        // Keep drawing inside the window.
        yTop = std::clamp(yTop, 0, height - 1);
        yBottom = std::clamp(yBottom, 0, height - 1);

        SDL_RenderLine(renderer, static_cast<float>(x), static_cast<float>(yTop), static_cast<float>(x), static_cast<float>(yBottom));
    }
}