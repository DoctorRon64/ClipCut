
#include "../pch.h"
#include "waveformRenderer.h"

WaveformRenderer::WaveformRenderer(SDL_Renderer* renderer, int width, int height, int waveformHeight, int centerY)
    : renderer(renderer), width(width), height(height), waveformHeight(waveformHeight), centerY(centerY) {
}

void WaveformRenderer::SetSize(int width, int height) {
    this->width = width;
    this->height = height;
}

void WaveformRenderer::SetSelectionStart(int x) {
    x = std::clamp(x, 0, width - 1);

    if (selectionEndX >= 0) {
        selectionStartX = std::min(x, selectionEndX);
    }
    else {
        selectionStartX = x;
    }
}

void WaveformRenderer::PrintSelection() {
    if (selectionEndX < 0 || selectionEndX <= selectionStartX) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, selectionColor.r, selectionColor.g, selectionColor.b, selectionColor.a);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_FRect selectionRect = {
        static_cast<float>(selectionStartX),0.0f,static_cast<float>(selectionEndX - selectionStartX),static_cast<float>(height)
    };

    SDL_RenderFillRect(renderer, &selectionRect);
}

void WaveformRenderer::SetSelectionEnd(int x) {
    x = std::clamp(x, 0, width - 1);
    selectionEndX = std::max(x, selectionStartX);
}

void WaveformRenderer::SetWaveformBounds(int waveformHeight, int centerY) {
    this->waveformHeight = waveformHeight;
    this->centerY = centerY;
}

void WaveformRenderer::PrintWave(const std::vector<WaveformPeak>& waveform) {
    if (renderer == nullptr || waveform.empty() || width <= 0 || height <= 0 || waveformHeight <= 0)
        return;

    for (int x = 0; x < width; ++x) {
        // Map this screen pixel to the waveform data.
        size_t start = static_cast<size_t>((static_cast<double>(x) / width) * waveform.size());
        size_t end = static_cast<size_t>((static_cast<double>(x + 1) / width) * waveform.size());

        start = std::min(start, waveform.size() - 1);
        end = std::clamp(end, start + 1, waveform.size());

        float minSample = 0.0f;
        float maxSample = 0.0f;

        SDL_SetRenderDrawColor(renderer, waveColor.r, waveColor.g, waveColor.b, waveColor.a);

        // Combine waveform bins that fall within this pixel.
        for (size_t i = start; i < end; ++i) {
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

void WaveformRenderer::PrintMarker() {
    SDL_SetRenderDrawColor(renderer, markerColor.r, markerColor.g, markerColor.b, markerColor.a);
    SDL_RenderLine(renderer, static_cast<float>(selectionStartX), 0.0f, static_cast<float>(selectionStartX), static_cast<float>(height));

    SDL_SetRenderDrawColor(renderer, markerColorEnd.r, markerColorEnd.g, markerColorEnd.b, markerColorEnd.a);
    if (selectionEndX >= 0) {
        SDL_RenderLine(renderer, static_cast<float>(selectionEndX), 0.0f, static_cast<float>(selectionEndX), static_cast<float>(height));
    }
}

void WaveformRenderer::Draw(const std::vector<WaveformPeak>& waveform) {
    PrintSelection();
    PrintWave(waveform);
    PrintMarker();
}

int WaveformRenderer::GetSelectionStartX() const {
    return selectionStartX;
}

int WaveformRenderer::GetSelectionEndX() const {
    return selectionEndX;
}

