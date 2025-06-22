cmake_minimum_required(VERSION 3.10)
project(EasyNotepad VERSION 1.0)

add_executable(${PROJECT_NAME} WIN32 EasyNotepad/main.cpp)

# hide the console window
target_link_options(${PROJECT_NAME} PRIVATE /ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS)

target_sources(${PROJECT_NAME} PRIVATE
    EasyNotepad/window.hpp
    EasyNotepad/SFML_intro.hpp
    EasyNotepad/mouse.hpp
    EasyNotepad/text_wrapper.hpp
    EasyNotepad/elementGUI.hpp
    EasyNotepad/scrollbar.hpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# set the SFML lib
set(SFML_DIR "C:/SFML-2.6.2/lib/cmake/SFML")

# set the packages SFML
find_package(SFML 2.6.2 COMPONENTS graphics window system REQUIRED)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# set the working directory (assets)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/EasyNotepad"
)

# set the path for SFML bin
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;C:\\SFML-2.6.2\\bin"
)