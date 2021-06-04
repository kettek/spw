-- premake5.lua
workspace "Testing"
   configurations { "Debug", "Release" }
   language "C++"
   includedirs { "../" }

project "basic"
   kind "ConsoleApp"
   targetdir "bin/%{cfg.buildcfg}"

   files { "basic.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   filter { "system:Windows" }
      defines { "USE_WINFORMS" }
   