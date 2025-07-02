:: @echo off
cmake --version
setlocal EnableDelayedExpansion
set CMAKE_PREFIX_PATH=%PREFIX%;%LIBRARY_PREFIX%
set CMAKE_INCLUDE_PATH=%LIBRARY_PREFIX%\include
:: set OPENSSL_ROOT_DIR=%CONDA_PREFIX%\Library
:: set Poco_DIR=%CONDA_PREFIX%\pkgs\poco-1.14.2-h8b29dd2_0\Library
:: Create and enter build directory
mkdir build
cd build

:: Configure the project with CMake using Ninja generator
cmake -G "Visual Studio 17 2022" ^
      -DAWS=ON -DPOCO=ON ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_INSTALL_PREFIX:PATH=%LIBRARY_PREFIX% ..
::      .. || exit /b 1

:: Build the project
:: cmake --build . --config Release || exit /b 1
cmake --build . --config Release 

:: Install the project
:: cmake --install . --config Release || exit /b 1
cmake --install . --config Release 
:: Exit successfully
:: exit /b 0

