@echo off
echo Cloning vcpkg
git clone https://github.com/microsoft/vcpkg ..\vcpkg --depth=1
echo Bootstrapping vcpkg
pushd .
cd ..\vcpkg
call .\bootstrap-vcpkg.bat
echo Integrating vcpkg with Visual Studio
vcpkg integrate install
echo Installing packages
vcpkg install sdl2:x64-windows sdl2:x86-windows tinyxml2:x64-windows tinyxml2:x86-windows  boost-algorithm[core]:x64-windows  boost-algorithm[core]:x86-windows spdlog:x86-windows spdlog:x64-windows aws-sdk-cpp[text-to-speech]:x86-windows aws-sdk-cpp[text-to-speech]:x64-windows aws-sdk-cpp[polly]:x86-windows aws-sdk-cpp[polly]:x64-windows rapidjson:x64-windows rapidjson:x86-windows --recurse
popd