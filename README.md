## tmx files loader for SDL3

# Features
- Scrolling
- Pixel Perfect rendering (no artifacts)
- Supports flipping of tiles (x, y and diagonal)
- Loads render layers and object layers
- Multiple tilesets per map
- Draws 60 times per second (avoid stuttering)

This is based on [tmxlite by fallahn](https://github.com/fallahn/tmxlite), he made some great samples for SDL2 and SFML but none for SDL3 so I grabbed the SDL2 sample and migrated it to SDL3 but it didn't supported flipping of tiles and scrolling, I added those from the SFML sample doing the necessary changes for SDL.

# Usage - Visual Studio 2022
- Create a Solution
- Add SDL3 Project to it
- Open Project Properties
- On C/C++ point to SDL3\include, $(ProjectDir)hdr, SDL3_image-x-y-z\include and tmxLite include and src
- On Linker/General point to SDL3_image-x-y-z\lib and $(ProjectDir)lib
- On Linker/Input add SDL3_image.lib and libtmxlite-s.lib (libtmxlite-s-d.lib for Debug)
- Add the code (\hdr and \src folders)
- Build it and run it
