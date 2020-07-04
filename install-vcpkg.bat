call .\bootstrap-vcpkg.bat
vcpkg integrate install
vcpkg install sdl2:x64-windows sdl2:x86-windows tinyxml2:x64-windows tinyxml2:x86-windows  boost-algorithm[core]:x64-windows  boost-algorithm[core]:x86-windows spdlog:x86-windows spdlog:x64-windows aws-sdk-cpp[text-to-speech]:x86-windows aws-sdk-cpp[text-to-speech]:x64-windows rapidjson:x64-windows rapidjson:x86-windows --recurse