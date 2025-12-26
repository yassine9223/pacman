cmake_minimum_required(VERSION 3.29)
project(pacman VERSION 0.1.0 LANGUAGES C)

# ==== C Standard ====
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ==== Output directories ====
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ==== Options ====
option(USE_SYSTEM_SDL3 "Prefer system SDL3 (if OFF, use bundled libs/SDL)" ON)
option(USE_SYSTEM_CMOCKA "Prefer system CMocka (if OFF, use bundled libs/cmocka)" ON)

# ==== Game sources ====
set(
        SRCS
        src/main.c
        src/framework.c
        src/firstLevel.c
)

set(
        HEADER
        src/main.h
        src/framework.h
        src/firstLevel.h
)

add_executable(pacman ${SRCS} ${HEADER})
target_include_directories(pacman PRIVATE src)

# =====================================================
# == SDL3 detection (system preferred, fallback to submodule)
# =====================================================

if (USE_SYSTEM_SDL3)
    find_package(SDL3 CONFIG QUIET)
endif()

if (TARGET SDL3::SDL3 OR SDL3_FOUND)
    message(STATUS "Using system SDL3")
elseif (EXISTS "${CMAKE_SOURCE_DIR}/libs/SDL")
    message(STATUS "Using bundled SDL3 at ${CMAKE_SOURCE_DIR}/libs/SDL")
    add_subdirectory("${CMAKE_SOURCE_DIR}/libs/SDL" EXCLUDE_FROM_ALL)
else()
    message(FATAL_ERROR
            "SDL3 not found. Install SDL3 system-wide or provide submodule at libs/SDL.\n"
            "Set -DUSE_SYSTEM_SDL3=OFF to use the bundled version."
    )
endif()

target_link_libraries(pacman PRIVATE SDL3::SDL3)

# Windows WIN32 subsystem
if (WIN32)
    set_target_properties(pacman PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# ==== Copy resources after build ====
add_custom_command(TARGET pacman POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:pacman>/resources"
)

# ==== Enable CTest ====
include(CTest)
enable_testing()

# =====================================================
# ===================== CMOCKA ========================
# =====================================================

# System CMocka
if (USE_SYSTEM_CMOCKA)
    find_package(CMocka QUIET)
endif()

if (TARGET CMocka::CMocka OR CMocka_FOUND)
    message(STATUS "Using system CMocka")
elseif (EXISTS "${CMAKE_SOURCE_DIR}/libs/cmocka")
    message(STATUS "Using bundled CMocka at ${CMAKE_SOURCE_DIR}/libs/cmocka")
    # Build a static library from the bundled CMocka source
    add_library(cmocka STATIC
            ${CMAKE_SOURCE_DIR}/libs/cmocka/src/cmocka.c
    )

    target_include_directories(cmocka PUBLIC
            ${CMAKE_SOURCE_DIR}/libs/cmocka/include
    )

    add_library(CMocka::CMocka ALIAS cmocka)

else()
    message(FATAL_ERROR
            "CMocka not found. Install libcmocka-dev or provide submodule libs/cmocka.\n"
            "Set -DUSE_SYSTEM_CMOCKA=OFF to force use of bundled version."
    )
endif()

# =====================================================
# ===================== Test Suite ====================
# =====================================================

set(TEST_SOURCES
        tests/test.c
        tests/framework_mock.c
        tests/firstLevel_stub.c
)

add_executable(test_suite
        ${TEST_SOURCES}
        src/main.c
)

target_include_directories(test_suite PRIVATE
        src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL/include
)

# Link CMocka
if (TARGET CMocka::CMocka)
    target_link_libraries(test_suite PRIVATE CMocka::CMocka)
else()
    message(FATAL_ERROR "CMocka target is missing despite successful detection")
endif()

# Define compile-time flag for tests
target_compile_definitions(test_suite PRIVATE PACMAN_TESTS)

# Link SDL3 for tests
if (TARGET SDL3::SDL3 OR SDL3_FOUND)
    target_link_libraries(test_suite PRIVATE SDL3::SDL3)
endif()

# Register the test
add_test(NAME pacman_tests COMMAND test_suite)

# ==== Package ====
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
