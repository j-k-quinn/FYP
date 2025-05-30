# ===================================================================================
#  The sl_hub CMake configuration file
#
#  Usage from an external project:
#    In your CMakeLists.txt, add these lines:
#
#    find_package(SL_IOT REQUIRED)
#    include_directories(${SL_HUB_INCLUDE_DIRS})
#    link_directories(${SL_HUB_LIBRARY_DIR})
#    target_link_libraries(MY_TARGET_NAME ${SL_HUB_LIBRARIES})
#
#    This file will define the following variables:
#      - SL_HUB_LIBRARIES           : The list of dynamic libraries (sl_hub) to link against.
#      - SL_HUB_LIBRARY_DIR         : The directory where lib files are.
#      - SL_HUB_INCLUDE_DIRS        : The sl_hub include directories.
#      - SL_HUB_ZED_SDK_VERSION     : The CUDA version required by sl_hub.
#      - CUDA_DEP_LIBRARIES_ZED     : The CUDA dependencies libraries used by sl_hub.
# ===================================================================================

set(SL_HUB_FOUND TRUE)

SET(SL_HUB_LIB_DIR /usr/local/sl_hub/lib/)
SET(SL_HUB_INCLUDE_DIR /usr/local/sl_hub/include/)
SET(SL_HUB_LIBRARIES sl_hub)
