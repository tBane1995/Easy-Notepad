# min version CMake
cmake_minimum_required(VERSION 3.10)

# project name
project(EasyNotepad)
set(PROJECT_VERSION_MAJOR 2)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

# add the executable
add_executable(${PROJECT_NAME} EasyNotepad/main.cpp)

# add the headers
target_sources(${PROJECT_NAME} PRIVATE
    EasyNotepad/SFML_intro.hpp
)

# set the standard C++ (after defining the target)
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

# compile features
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# find SFML
set(SFML_DIR "C:/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML 2.6.2 COMPONENTS graphics window system REQUIRED)

# link SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# copy files to project folders
set(ASSETS
	arial.ttf
	sfml-icon-big.png
)

set(DEBUG_DLLS
	libs/sfml-graphics-d-2.dll
	libs/sfml-system-d-2.dll
	libs/sfml-window-d-2.dll
)

set(RELEASE_DLLS
	libs/sfml-graphics-2.dll
	libs/sfml-system-2.dll
	libs/sfml-window-2.dll
)

# copy files to Release
foreach(ASSET ${RELEASE_DLLS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/EasyNotepad/${ASSET}
            ${CMAKE_BINARY_DIR}/Release
        COMMENT "Copying ${ASSET} to Release folder"
        VERBATIM
        CONFIGURATIONS Release
    )
endforeach()

# copy files to Debug
foreach(ASSET ${DEBUG_DLLS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Debug
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/EasyNotepad/${ASSET}
            ${CMAKE_BINARY_DIR}/Debug
        COMMENT "Copying ${ASSET} to Debug folder"
        VERBATIM
        CONFIGURATIONS Debug
    )
endforeach()

# copy Assets
foreach(ASSET ${ASSETS})
    foreach(CONFIG_TYPE Debug Release)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/EasyNotepad/${ASSET}
                ${CMAKE_BINARY_DIR}/${CONFIG_TYPE}
            COMMENT "Copying ${ASSET} to ${CONFIG_TYPE} folder"
            VERBATIM
            CONFIGURATIONS ${CONFIG_TYPE}
        )
    endforeach()
endforeach()

# set the Debugger Working Directory in Visual Studio (Debug/Release)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
)