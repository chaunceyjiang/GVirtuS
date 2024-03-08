cmake_minimum_required(VERSION 3.17)
project(gvirtus-plugin-cuda)
find_package(CUDA REQUIRED)

find_path(CUDADR_INCLUDE_DIRECTORY
        cuda.h
        PATHS ${CUDA_INCLUDE_DIRS})
if(NOT CUDADR_INCLUDE_DIRECTORY)
    message(FATAL_ERROR "cuda.h not found")
endif()
include_directories(${CUDADR_INCLUDE_DIRECTORY} ${CUDA_INCLUDE_DIRS} util)

get_filename_component(CUDA_LIBRARIES_PATH ${CUDA_CUDART_LIBRARY} DIRECTORY FALSE)
find_library(CUDADR_LIBRARY
        cuda
        PATHS ${CUDA_LIBRARIES_PATH})
if(NOT CUDADR_LIBRARY)
    message(FATAL_ERROR "cuda library not found")
endif()
message(STATUS "Found cuda library: ${CUDADR_LIBRARY}")

set(CUDADR_VERSION "${CUDADR_LIBRARY}")
while(IS_SYMLINK "${CUDADR_VERSION}")
    message(STATUS "${CUDADR_VERSION}")
    execute_process(COMMAND readlink "${CUDADR_VERSION}"
            OUTPUT_VARIABLE CUDADR_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE)
endwhile()
string(REGEX REPLACE ".*\\.so\\.([0-9]+\\.[0-9]+\\.[0-9]+)" "\\1" CUDADR_VERSION "${CUDADR_VERSION}")

message(STATUS "Found CUDADR: ${CUDADR_VERSION} (${CUDADR_INCLUDE_DIRECTORY}/cuda.h, ${CUDADR_LIBRARY})")

gvirtus_add_backend(cuda ${CUDA_VERSION}
        backend/CudaDrHandler.cpp
        backend/CudaDrHandler_context.cpp
        backend/CudaDrHandler_device.cpp
        backend/CudaDrHandler_event.cpp
        backend/CudaDrHandler_execution.cpp
        backend/CudaDrHandler_initialization.cpp
        backend/CudaDrHandler_memory.cpp
        backend/CudaDrHandler_module.cpp
        backend/CudaDrHandler_stream.cpp
        backend/CudaDrHandler_texture.cpp
        backend/CudaDrHandler_version.cpp
        util/CudaUtil.cpp
)
target_link_libraries(${PROJECT_NAME} ${CUDA_CUDART_LIBRARY})

gvirtus_add_frontend(cuda ${CUDA_VERSION}
        frontend/CudaDr.cpp
        frontend/CudaDr_context.cpp
        frontend/CudaDr_device.cpp
        frontend/CudaDr_event.cpp
        frontend/CudaDr_execution.cpp
        frontend/CudaDr_initialization.cpp
        frontend/CudaDr_memory.cpp
        frontend/CudaDr_module.cpp
        frontend/CudaDr_stream.cpp
        frontend/CudaDr_texture.cpp
        frontend/CudaDr_version.cpp
        frontend/CudaDrFrontend.cpp
        util/CudaUtil.cpp
)
