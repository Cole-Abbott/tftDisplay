cmake_minimum_required(VERSION  3.12)

set (PICO_TOOLCHAIN_PATH "/Applications/ARM/bin")
#pull in pico sdk
include(pico_sdk_import.cmake)

project(main C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

#set board to pico W
set(PICO_BOARD pico_w)

#initialize pico sdk
pico_sdk_init()

#add executable
add_executable(main main.cpp)


#link pico standard library
#add pico_st7735????


add_library(pico_st7735 INTERFACE)

target_sources(pico_st7735 INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/st7735/ST7735_TFT.cpp
  ${CMAKE_CURRENT_LIST_DIR}/st7735/ST7735_TFT_graphics.cpp
  ${CMAKE_CURRENT_LIST_DIR}/st7735/ST7735_TFT_Print.cpp
  ${CMAKE_CURRENT_LIST_DIR}/st7735/ST7735_TFT_Font.cpp
  ${CMAKE_CURRENT_LIST_DIR}/st7735/ST7735_TFT_Bitmap_Data.cpp
)

add_library(picoGame INTERFACE)

target_sources(picoGame INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/display.cpp
  ${CMAKE_CURRENT_LIST_DIR}/entity.cpp
)

target_include_directories(pico_st7735 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)


target_link_libraries(main pico_stdlib hardware_spi pico_cyw43_arch_none pico_st7735 picoGame)

#create map/bin/hex/uf2 files
pico_add_extra_outputs(main)

#make print go to usb serial instead of default uart
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)