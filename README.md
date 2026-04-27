## TMX Loader for SDL3

# Features
- Scrolling
- Pixel Perfect rendering (no artifacts)
- Supports flipping of tiles (x, y and diagonal)
- Loads render layers and object layers
- Multiple tilesets per map
- Updates 60 times per second

This is based on and uses [tmxlite by fallahn](https://github.com/fallahn/tmxlite), he made some great samples for SDL2 and SFML so I grabbed the SDL2 sample and migrated it to SDL3 but it didn't supported flipping of tiles and scrolling, I added those from the SFML sample doing the necessary changes for SDL.

# Usage - Visual Studio 2022
- Create a Solution
- Copy all folders to the Project folder
- Add SDL3 Project to the Solution
- Open Project Properties
  - On C/C++ point to SDL3\include, $(ProjectDir)hdr, SDL3_image-3.x.y/include and tmxLite/include and /src
  - On Linker/General point to (ProjectDir)lib and SDL3_image-3.x.y/lib/$(LibrariesArchitecture)
  - On Linker/Input add SDL3_Image.lib and libtmxlite-s.lib (libtmxlite-s-d.lib for Debug)
- Add the code (\hdr and \src folders) to the Project
- Build and run it

You can also download it and test it from [Releases](https://github.com/GuidoBisocoli/SDL3-TMX-LOADER/releases/tag/v1.0)
