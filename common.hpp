#pragma once

namespace spw {
  enum Level {
    Info,
    Debug,
    Warning,
    Error,
    Fatal,
  };

  #ifndef SPW_DEFAULT_WIDTH
  #define SPW_DEFAULT_WIDTH 600
  #endif
  #ifndef SPW_DEFAULT_HEIGHT
  #define SPW_DEFAULT_HEIGHT 600
  #endif

  static unsigned int default_width = SPW_DEFAULT_WIDTH;
  static unsigned int default_height = SPW_DEFAULT_HEIGHT;
}
