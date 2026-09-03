#-----------------------------------------------------------------------------
# Umicom OS Module
# File: CMakeLists.txt
#
# PURPOSE:
#   Build the thin user-space Umicom OS Control Centre module. The full
#   distribution, Linux kernel, boot and recovery remain in umicom-os.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)

project(
    UmicomOsModule
    VERSION 0.1.0
    DESCRIPTION "Thin user-space Umicom OS Control Centre"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)
include(GNUInstallDirs)

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET Umicom::Framework)
    find_package(UmicomFramework CONFIG REQUIRED)
endif()

# Define the local fallback only when the parent build did not provide the shared helper.
if(NOT COMMAND umicom_apply_warnings)
    # Define the apply warnings build helper so parent and application projects apply one
    # consistent rule.
    function(umicom_apply_warnings target)
        # Select the warning options understood by the active compiler.
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4)
        # Use this fallback path when the earlier condition does not apply.
        else()
            target_compile_options(${target} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endfunction()
endif()

# Define the local fallback only when the parent build did not provide the shared helper.
if(NOT COMMAND umicom_apply_sanitizers)
    # Define the apply sanitizers build helper so parent and application projects apply one
    # consistent rule.
    function(umicom_apply_sanitizers target)
    endfunction()
endif()

add_library(umicom_os_module STATIC
    src/control_centre.c
)
add_library(Umicom::os_module ALIAS umicom_os_module)

target_include_directories(umicom_os_module PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(umicom_os_module PUBLIC
    Umicom::Framework
)
umicom_apply_warnings(umicom_os_module)
umicom_apply_sanitizers(umicom_os_module)

add_executable(umicom-os-control-centre
    src/console/main.c
)
target_link_libraries(umicom-os-control-centre PRIVATE
    Umicom::os_module
)
umicom_apply_warnings(umicom-os-control-centre)
umicom_apply_sanitizers(umicom-os-control-centre)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    add_executable(umicom-os-module-test
        tests/test_control_centre.c
    )
    target_link_libraries(umicom-os-module-test PRIVATE
        Umicom::os_module
    )
    umicom_apply_warnings(umicom-os-module-test)
    umicom_apply_sanitizers(umicom-os-module-test)
    add_test(
        NAME os.module
        COMMAND umicom-os-module-test
    )
endif()

install(TARGETS
    umicom_os_module
    umicom-os-control-centre
    EXPORT UmicomOsModuleTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY resources/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/applications/os)
# Install architecture records beside the application metadata so packaged
# systems preserve the reasoning and boundaries used to build the product.
install(DIRECTORY docs/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/applications/os/docs)
install(FILES application.umicom.yaml README.md LICENSE
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/applications/os)

message(STATUS "Umicom OS Module version: ${PROJECT_VERSION}")
message(STATUS "Linux kernel inside Framework: OFF")

include(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../framework/cmake/UmicomApplicationProductisationContribution.cmake")
umicom_add_application_productisation_contribution(
    NAME "os"
    TARGET "umicom_os_productisation"
    SOURCE "src/productisation_contribution.c"
    TEST_SOURCE "tests/test_productisation_contribution.c")
