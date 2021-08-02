cmake_minimum_required(VERSION 3.12)
project(HNS LANGUAGES C VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)

set(ignoreMe "${CMAKE_CXX_COMPILER}")

file(GLOB_RECURSE hns_SRC
        include/*.h
        src/*.c)

add_executable(HNS ${hns_SRC})

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions(-DCHECK_MEMORY_LEAKS=1)
        message("Debug build.")
else()
        add_definitions(-DCUSTOM_ALLOCATOR=1)
endif()

add_subdirectory(lib/mana buildmana)

set(HACK_AND_SLASH_RPG_SERVER_STATIC_LIBRARY ON CACHE BOOL "" FORCE)
add_subdirectory(lib/hack-and-slash-rpg-server buildhack-and-slash-rpg-server)

if (WIN32)
        #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xclang -fopenmp -qopenmp-link=static")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xclang -fopenmp")
endif (WIN32)

if(APPLE)
        add_subdirectory(lib/MoltenVK buildmoltenvk)
        find_package(OpenMP)
        if (OPENMP_FOUND)
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        endif()
endif()

if(UNIX AND NOT APPLE)
        # for Linux, BSD, Solaris, Minix
        find_package(OpenMP)
        if (OPENMP_FOUND)
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        endif()
endif()

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-visibility -Wno-pragma-pack -Wno-deprecated-declarations -mavx -mavx2")
# Compile with up to sse2
add_definitions(-DSIMD_SSE2=1)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-visibility -Wno-pragma-pack -Wno-deprecated-declarations -mavx -mavx2")

#if(CMAKE_BUILD_TYPE MATCHES Debug)
#        message("Debug build.")
#elseif(CMAKE_BUILD_TYPE MATCHES Release)
#        message("Release build.")
#else()
#        message("Some other build type.")
#endif()

set(HACK_AND_SLASH_RPG_BUILD_LIBRARIES
        HNS
        HNSSERVER
        mana)

if (WIN32 AND CMAKE_C_COMPILER_ID MATCHES "Clang")
        STRING(REPLACE " " "\ " PATH_NO_SPACE "$ENV{ProgramFiles}")
        set(HACK_AND_SLASH_RPG_BUILD_LIBRARIES ${HACK_AND_SLASH_RPG_BUILD_LIBRARIES} "${PATH_NO_SPACE}/LLVM/lib/libomp.lib" "${PATH_NO_SPACE}/LLVM/lib/libiomp5md.lib")
        add_custom_command (TARGET HNS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PATH_NO_SPACE}/LLVM/bin/libomp.dll ${CMAKE_CURRENT_BINARY_DIR}/libomp.dll)
endif ()

target_link_libraries(${HACK_AND_SLASH_RPG_BUILD_LIBRARIES})

set(includeList
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/hack-and-slash-rpg-server/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/cstorage/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/cnoise/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/cthreads/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/uber-math/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/mimalloc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/Vulkan-Headers/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/chaos/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/libsndfile/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/libsoundio/soundio
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/lib/cxml-parser/include
        ${OpenMP_CXX_LIBRARIES})

target_include_directories(HNS PUBLIC ${includeList})

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
else()
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslangValidator.exe")
endif()

file(GLOB_RECURSE GLSL_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/assets/shaders/opengl/*.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/assets/shaders/opengl/*.vert)

foreach(GLSL ${GLSL_SOURCE_FILES})
        get_filename_component(FILE_NAME ${GLSL} NAME)
        set(SPIRV ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/assets/shaders/spirv/${FILE_NAME}.spv)
        add_custom_command(
                OUTPUT ${SPIRV}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/lib/Mana/assets/shaders/spirv
                COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
                DEPENDS ${GLSL})
        list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(Shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(HNS Shaders)

file(GLOB_RECURSE ALL_ASSETS
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/assets/*)

add_custom_target(Assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/lib/mana/assets ${PROJECT_BINARY_DIR}/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/assets)

if(WIN32)
        target_sources(HNS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/icon.rc)
endif()

add_dependencies(HNS Assets)
