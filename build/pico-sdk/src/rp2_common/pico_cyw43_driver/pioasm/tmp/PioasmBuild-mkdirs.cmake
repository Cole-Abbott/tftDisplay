# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/cole/code/piPico/pico-sdk/tools/pioasm"
  "/Users/cole/code/piPico/tftDisplay/build/pioasm"
  "/Users/cole/code/piPico/tftDisplay/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm"
  "/Users/cole/code/piPico/tftDisplay/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/Users/cole/code/piPico/tftDisplay/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
  "/Users/cole/code/piPico/tftDisplay/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/Users/cole/code/piPico/tftDisplay/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/cole/code/piPico/tftDisplay/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/cole/code/piPico/tftDisplay/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
