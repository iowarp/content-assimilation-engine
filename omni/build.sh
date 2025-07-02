export CMAKE_PREFIX_PATH=$PREFIX
export CMAKE_INCLUDE_PATH=$PREFIX/include
mkdir build
cd build
cmake \
    -DAWS=ON -DPOCO=ON -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX ..
cmake --build . --config Release 
cmake --install . --config Release 







