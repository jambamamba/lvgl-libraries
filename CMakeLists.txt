
#mkdir -p build; cd build; cmake -G "MSYS Makefiles" ..; make -j

cmake_minimum_required(VERSION 3.12.4)

project(lvgl)

file(GLOB_RECURSE LVGL_INCLUDES 
   "lvgl/*.h" 
   "lvgl/demos/*.h"  
    )
file(GLOB_RECURSE LVGL_SOURCES  
   "lvgl/*.c" 
   "lvgl/demos/*.c" 
    )
    
if (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
   set(TARGET "${CMAKE_SOURCE_DIR}/target/arm")
   file(GLOB_RECURSE IMX_INCLUDES 
      "lv_drivers/display/fbdev.h" 
      "lv_drivers/wayland/wayland.h" 
      )
   file(GLOB_RECURSE IMX_SOURCES 
      "lv_drivers/display/fbdev.c" 
      "lv_drivers/wayland/wayland.c" 
      )
   list (APPEND LVGL_INCLUDES ${IMX_INCLUDES})
   list (APPEND LVGL_SOURCES ${IMX_SOURCES})
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND NOT MINGW)
   set(TARGET "${CMAKE_SOURCE_DIR}/target/x86")
   find_package(SDL2 REQUIRED SDL2)
   include_directories(${SDL2_INCLUDE_DIRS})
   file(GLOB_RECURSE SDL_INCLUDES 
      "lv_port_pc_eclipse/*.h"
      "lv_drivers/display/*.h" 
      "lv_drivers/indev/*.h" 
      "lv_drivers/sdl/*.h" 
      )
   file(GLOB_RECURSE SDL_SOURCES 
      "lv_port_pc_eclipse/mouse_cursor_icon.c" 
      "lv_drivers/display/*.c" 
      "lv_drivers/indev/*.c"
      "lv_drivers/sdl/*.c" 
      )
   list (APPEND LVGL_INCLUDES ${SDL_INCLUDES})
   list (APPEND LVGL_SOURCES ${SDL_SOURCES})
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64" OR MINGW)#MINGW if we are crosscompiling on linux host for windows target using mingw
   find_package(SDL2 REQUIRED SDL2)
   include_directories(${SDL2_INCLUDE_DIRS})
   file(GLOB_RECURSE SDL_INCLUDES 
      "lv_port_pc_eclipse/*.h"
      "lv_drivers/display/*.h" 
      "lv_drivers/sdl/*.h" 
      )
   file(GLOB_RECURSE SDL_SOURCES 
      "lv_port_pc_eclipse/mouse_cursor_icon.c" 
      "lv_drivers/sdl/*.c" 
      )
   list (APPEND LVGL_INCLUDES ${SDL_INCLUDES})
   list (APPEND LVGL_SOURCES ${SDL_SOURCES})
else()
   message(FATAL_ERROR "target must be aarch64 or x86_64 or AMD64")
endif()

add_library(lvgl STATIC ${LVGL_SOURCES} ${LVGL_INCLUDES})
set (LVGL_INC_DIRS
   ${PROJECT_SOURCE_DIR}
   ${PROJECT_SOURCE_DIR}/..
   ${THIRD_PARTY_LIB_DIR}
   )
target_include_directories(lvgl PRIVATE ${LVGL_INC_DIRS})
add_definitions(-DLV_CONF_INCLUDE_SIMPLE -DUSE_WAYLAND -DLV_WAYLAND_WL_SHELL)#target_compile_ does not work with cmake in yocto! do it the old fashioned way


if (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
   target_include_directories(lvgl PRIVATE ${LVGL_INC_DIRS} "${SYSROOT_COMPONENTS}/aarch64/wayland/usr/include/")
   target_compile_definitions(lvgl PRIVATE SCREEN_HOR_RES=800 SCREEN_VER_RES=480)
elseif((CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64") OR (CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64"))
   target_compile_definitions(lvgl PRIVATE 
      LV_CONF_INCLUDE_SIMPLE 
      USE_SDL=1 
      USE_MONITOR=0
      USE_WAYLAND
      LV_WAYLAND_WL_SHELL
      SDL_INCLUDE_PATH="SDL2/SDL.h"
      SDL_HOR_RES=800
      SDL_VER_RES=480
      SDL_ZOOM=1
      SDL_DOUBLE_BUFFERED=0
      SDL_VIRTUAL_MACHINE=0
      SDL_DUAL_DISPLAY=0
   ) 
   set (INC_DIRS 
      ${INC_DIRS}
      ${LVGL_INC_DIRS}
   )
   target_include_directories(lvgl PRIVATE ${INC_DIRS})
else()
   message(FATAL_ERROR "target must be aarch64 or x86_64 or AMD64")
endif()

