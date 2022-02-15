cmake_minimum_required(VERSION 3.20)

project(Howl)

include(FetchContent)

FetchContent_Declare(portaudio GIT_REPOSITORY https://github.com/PortAudio/portaudio)
FetchContent_Declare(hell      GIT_REPOSITORY https://github.com/mokchira/hell)

FetchContent_MakeAvailable(portaudio hell)

add_executable(main main.c)

target_link_libraries(main PUBLIC Hell::Hell PortAudio m)
set_target_properties(main PROPERTIES EXPORT_COMPILE_COMMANDS ON)
