cmake_minimum_required(VERSION 3.10)
project(EasyNotepad VERSION 1.0)

add_executable(${PROJECT_NAME} EasyNotepad/main.cpp)

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

set(SFML_DIR "C:/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML 2.6.2 COMPONENTS graphics window system REQUIRED)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# Ustaw working directory na katalog z assets
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/EasyNotepad"
)

# Ustaw PATH do DLL-i SFML, by nie kopiować
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;C:\\SFML-2.6.2\\bin"
)