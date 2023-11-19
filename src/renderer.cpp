#include "renderer.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

constexpr size_t max_colors = 4;
constexpr color colors[max_colors] = {{255, 255, 255, 255}, {255, 200, 150, 255}, {200, 150, 100, 255}, {150, 100, 50, 255}};

///
///
renderer::renderer(int screen_width, int screen_height) : screen_width_(screen_width), screen_height_(screen_height)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        std::cerr << "SDL could not initialize.\n";
        std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    }

    // Create Window
    sdl_window_ = SDL_CreateWindow("N-Body Simulation", SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, screen_width,
                                    screen_height, SDL_WINDOW_SHOWN);

    if (sdl_window_ == nullptr) 
    {
        std::cerr << "Window could not be created.\n";
        std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
    }

    // Create renderer
    sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, SDL_RENDERER_ACCELERATED);
    if (sdl_renderer_ == nullptr) 
    {
        std::cerr << "Renderer could not be created.\n";
        std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    }
}

///
///
renderer::~renderer() 
{
    SDL_DestroyWindow(sdl_window_);
    SDL_Quit();
}

///
///
auto renderer::update_window_title(uint64_t n_particles, double total_energy, int fps) -> void
{
    std::string title{"Particles: " + std::to_string(n_particles) + " Energy: " + std::to_string(total_energy) + " FPS: " + std::to_string(fps)};
    SDL_SetWindowTitle(sdl_window_, title.c_str());
}

///
///
auto renderer::render(const std::vector<particle>& particles, const camera& cam) -> void
{
    // Sort particles by their acceleration 
    std::vector<particle> reordered_particles = particles;
    std::sort(reordered_particles.begin(), reordered_particles.end(), [](const auto& first, const auto& second) 
    {
        return first.acc().length() > second.acc().length();
    });

    // Create SDL points from sorted particle list
    std::vector<SDL_Point> points;
    for (const auto& particle : reordered_particles)
    {
        SDL_Point point;
        const auto window_position = cam.transform(particle.pos());
        point.x = static_cast<int>(screen_width_ * window_position.x());
        point.y = static_cast<int>(screen_width_ * window_position.y());
        points.push_back(point);
    }

    // Clear screen
    SDL_SetRenderDrawColor(sdl_renderer_, 0x1E, 0x1E, 0x1E, 0xFF);
    SDL_RenderClear(sdl_renderer_);
  
    // Render particles
    auto band_size = static_cast<int>(std::ceil(static_cast<float_type>(points.size()) / max_colors));
    for (auto i = 0; i < max_colors; i++)
    {
        auto band_start = i * band_size;
        auto band_end = std::min((i + 1) * band_size, static_cast<int>(points.size()));
        auto corrected_band_size = band_end - band_start;
        auto current_color = colors[i];
        SDL_SetRenderDrawColor(sdl_renderer_, current_color.r, current_color.g, current_color.b, current_color.a);
        SDL_RenderDrawPoints(sdl_renderer_, points.data() + band_start, static_cast<int>(corrected_band_size));
    }
  
    // Update Screen
    SDL_RenderPresent(sdl_renderer_);
}
