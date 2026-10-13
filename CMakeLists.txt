cmake_minimum_required(VERSION 3.22)
project(SimpleNES LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(BUILD_STATIC "Link external libraries statically" OFF)
set(SFML_STATIC_LIBRARIES ${BUILD_STATIC})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use the existing Homebrew installation on macOS, including Apple Silicon.
# Explicit SFML_DIR / CMAKE_PREFIX_PATH settings continue to take precedence.
if(APPLE)
    find_program(HOMEBREW_EXECUTABLE brew)
    if(HOMEBREW_EXECUTABLE)
        execute_process(COMMAND "${HOMEBREW_EXECUTABLE}" --prefix sfml
            OUTPUT_VARIABLE HOMEBREW_SFML_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            RESULT_VARIABLE HOMEBREW_SFML_RESULT)
        if(HOMEBREW_SFML_RESULT EQUAL 0)
            list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_SFML_PREFIX}")
        endif()
    endif()
endif()

find_package(SFML 3 CONFIG REQUIRED COMPONENTS Graphics)
find_package(Threads REQUIRED)

file(GLOB SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/APU/*.cpp")
add_executable(SimpleNES main.cpp ${SOURCES} vendor/miniaudio/miniaudio.c)
target_compile_features(SimpleNES PRIVATE cxx_std_17)
set_target_properties(SimpleNES PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(SimpleNES PRIVATE include vendor/miniaudio)
target_link_libraries(SimpleNES PRIVATE SFML::Graphics Threads::Threads ${CMAKE_DL_LIBS})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(SimpleNES PRIVATE "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-Wall;-Wextra>")
    if(BUILD_STATIC AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(SimpleNES PRIVATE -static-libstdc++ -static-libgcc)
    endif()
endif()

configure_file(keybindings.conf keybindings.conf COPYONLY)

# Will add __FILENAME__ macros for all source files, which is the filename without full find_path
# Courtesy of SO
function(define_file_basename_for_sources targetname)
    get_target_property(source_files "${targetname}" SOURCES)
    foreach(sourcefile ${source_files})
        # Get source file's current list of compile definitions.
        get_property(defs SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS)
        # Add the FILE_BASENAME=filename compile definition to the list.
        get_filename_component(basename "${sourcefile}" NAME)
        list(APPEND defs "__FILENAME__=\"${basename}\"")
        # Set the updated compile definitions on the source file.
        set_property(
            SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS ${defs})
    endforeach()
endfunction()

define_file_basename_for_sources(SimpleNES)
install(TARGETS SimpleNES RUNTIME DESTINATION bin)
