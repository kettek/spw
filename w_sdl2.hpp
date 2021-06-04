#pragma once
#include <iostream>
#include "common.hpp"
#include <sdl.h>

namespace spw {
  bool createSDL2Window(spw::Level level, const std::string title, const std::string body) {
    uint32_t flags = 0;
    switch(level) {
      case spw::Warning:
        flags = SDL_MESSAGEBOX_WARNING;
      break;
      case spw::Error:
      case spw::Fatal:
        flags = SDL_MESSAGEBOX_ERROR;
      break;
      case spw::Info:
      case spw::Debug:
        flags = SDL_MESSAGEBOX_INFORMATION;
      break;
    }
    SDL_ShowSimpleMessageBox(flags, title, body, nullptr);
  }
}
