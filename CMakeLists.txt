
#mkdir -p build; cd build; cmake -G "MSYS Makefiles" ..; make -j

cmake_minimum_required(VERSION 3.12.4)

set(PROJECT "lvgl")
project(${PROJECT})

file(GLOB_RECURSE SOURCES  
   "lvgl/*.h" 
   "lvgl/*.c" 
   "lvgl/demos/*.h"
   "lvgl/demos/*.c" 
   "custom_get_tick.c"
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
   list (APPEND SOURCES ${IMX_SOURCES} ${IMX_INCLUDES})
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND NOT MINGW)
   set(TARGET "${CMAKE_SOURCE_DIR}/target/x86")
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
   list (APPEND SOURCES ${SDL_SOURCES} ${SDL_INCLUDES})
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64" OR MINGW)#MINGW if we are crosscompiling on linux host for windows target using mingw
   file(GLOB_RECURSE SDL_INCLUDES 
      "lv_port_pc_eclipse/*.h"
      "lv_drivers/display/*.h" 
      "lv_drivers/sdl/*.h" 
      )
   file(GLOB_RECURSE SDL_SOURCES 
      "lv_port_pc_eclipse/mouse_cursor_icon.c" 
      "lv_drivers/sdl/*.c" 
      )
   list (APPEND SOURCES ${SDL_SOURCES} ${SDL_INCLUDES})
else()
   message(FATAL_ERROR "target must be aarch64 or x86_64 or AMD64")
endif()

add_library(${PROJECT} SHARED ${SOURCES})
set (LVGL_INC_DIRS
   ${PROJECT_SOURCE_DIR}
   ${PROJECT_SOURCE_DIR}/..
   ${CMAKE_SOURCE_DIR}/utils
   ${CMAKE_SOURCE_DIR}/utils/cJSON
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/lvgl
   )
target_include_directories(${PROJECT} PRIVATE ${LVGL_INC_DIRS})
target_link_libraries(${PROJECT} PRIVATE ${SDL_LIBRARIES})
target_link_directories(${PROJECT} PRIVATE ${SDL_LIB_DIR}) 
add_definitions(-DLV_CONF_INCLUDE_SIMPLE -DUSE_WAYLAND -DLV_WAYLAND_WL_SHELL)#target_compile_ does not work with cmake in yocto! do it the old fashioned way

if (NOT MONITOR_HOR_RES OR NOT MONITOR_VER_RES)
   message(FATAL_ERROR "@@@@@@@@@@@@@@@@@ !!!!!!!!!!!!!!! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n"
      "Please define these settings in your top level CMakeLists.txt file\n"
      "set (USE_SDL 1)\n"
      "set (USE_MONITOR 1)\n"
      "set (MONITOR_HOR_RES 800)\n"
      "set (MONITOR_VER_RES 480)\n"
      "add_compile_definitions(USE_SDL=1)\n"
      "@@@@@@@@@@@@@@@@@ !!!!!!!!!!!!!!! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n"
)
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
   target_include_directories(${PROJECT} PRIVATE ${LVGL_INC_DIRS} "${SYSROOT_COMPONENTS}/aarch64/wayland/usr/include/")
   target_compile_definitions(${PROJECT} PRIVATE SCREEN_HOR_RES=800 SCREEN_VER_RES=480)
elseif((CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64") OR (CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64") OR MINGW)
   target_compile_definitions(${PROJECT} PRIVATE 
      LV_CONF_INCLUDE_SIMPLE 
      USE_SDL=${USE_SDL}
      USE_MONITOR=${USE_MONITOR}
      USE_WAYLAND
      LV_WAYLAND_WL_SHELL
      MONITOR_HOR_RES=${MONITOR_HOR_RES}
      MONITOR_VER_RES=${MONITOR_VER_RES}
      SDL_HOR_RES=${MONITOR_HOR_RES}
      SDL_VER_RES=${MONITOR_VER_RES}
      SDL_ZOOM=1
      SDL_DOUBLE_BUFFERED=0
      SDL_VIRTUAL_MACHINE=0
      SDL_DUAL_DISPLAY=0
   ) 
   set (INC_DIRS 
      ${INC_DIRS}
      ${LVGL_INC_DIRS}
   )
   target_include_directories(${PROJECT} PRIVATE ${INC_DIRS})
else()
   message(FATAL_ERROR "target must be aarch64 or x86_64 or mingw. Your CMAKE_SYSTEM_PROCESSOR is ${CMAKE_SYSTEM_PROCESSOR}")
endif()

if((CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64") OR MINGW)
endif()

if(USE_SDL)
   find_package(SDL2 REQUIRED SDL2)
   target_link_libraries(${PROJECT} PRIVATE SDL2::SDL2)
endif()