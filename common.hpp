#pragma once

namespace spw {
  enum Level {
    Info,
    Debug,
    Warning,
    Error,
    Fatal,
  };

  static unsigned int default_width = 600;
  static unsigned int default_height = 600;
}