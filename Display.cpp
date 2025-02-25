#include "stdafx.h"

#include "Display.h"

using namespace glm;

using std::vector;

namespace softlit
{
    Display::Display(const uint32_t w, const uint32_t h, bool fullscreen)
        : m_width(w), m_height(h)
    {
        SDL_Init(SDL_INIT_EVERYTHING);

        m_window = SDL_CreateWindow("SoftLit", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_SHOWN | ((fullscreen) ? SDL_WINDOW_FULLSCREEN : 0));

        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

        SDL_RendererInfo info;
        SDL_GetRendererInfo(m_renderer, &info);
        printf("Renderer name: %s\n", info.name);
        printf("Texture formats: \n");
        for (uint32_t i = 0; i < info.num_texture_formats; i++)
        {
            printf("\t%s\n", SDL_GetPixelFormatName(info.texture_formats[i]));
        }

        m_renderTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, m_width, m_height);

        m_colorBuffer.resize(m_width * m_height * 4);
    }

    Display::~Display()
    {
        SDL_DestroyTexture(m_renderTexture);
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();

        m_colorBuffer.clear();
    }

    void Display::ClearRenderTarget()
    {
        //TODO: Refresh color buffer?!
        //fill(m_colorBuffer.begin(), m_colorBuffer.end(), 0x0);

        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(m_renderer);
    }

    void Display::Present()
    {
        SDL_UpdateTexture(m_renderTexture, nullptr, m_colorBuffer.data(), m_width * 4);

        SDL_RenderCopy(m_renderer, m_renderTexture, nullptr, nullptr);
        SDL_RenderPresent(m_renderer);
    }

    void Display::UpdateColorBuffer(const vector<vec4>& frameBuffer)
    {
        const int32_t fbDim = m_width * m_height * 4;

#pragma omp parallel for
        for (int32_t i = 0; i < fbDim; i += 4)
        {
            vec4 rgba = frameBuffer[i / 4];
            rgba = glm::clamp(rgba, 0.f, 1.f); // Clamp color value

            m_colorBuffer[i] = (uchar)(255 * rgba.b);
            m_colorBuffer[i + 1] = (uchar)(255 * rgba.g);
            m_colorBuffer[i + 2] = (uchar)(255 * rgba.r);
            m_colorBuffer[i + 3] = (uchar)(255 * rgba.a);
        }
    }
}