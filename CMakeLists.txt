#
# Copyright 2022 Camille "DrasLorus" Monière.
#
# This file is part of CORDIC_Rotate_APFX.
#
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU
# Lesser General Public License as published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License along with this program.
# If not, see <https://www.gnu.org/licenses/>.
#

cmake_minimum_required (VERSION 3.16.0 FATAL_ERROR)
# setting this is required

project (
  CordicAbs
  LANGUAGES CXX
  VERSION 0.1
)

if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.2))
  message (
    FATAL_ERROR
      "This project require a GNU compiler version greater than 6.2."
      "\nNote: If you tried to use g++-4.6.3 as provided by Xilinx Vivado v2019.1, use directly the TCL script "
      "in the folder `hls_files/cordicabs_16_4_6`."
  )
endif ()

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# ##################################################################################################
# Options
# ##################################################################################################

option (EXPORT_COMMANDS "export compile commands, for use with clangd for example." ON)
option (ENABLE_XILINX "use Xilinx provided proprietary headers." OFF)
if (ENABLE_XILINX)
  option (XILINX_COSIM "use TCL Vivado/Vitis HLS script to co-simulate the design." OFF)
  if (XILINX_COSIM)
    option (XILINX_IP "use TCL Vivado/Vitis HLS script to generate an IP." OFF)
    if (XILINX_IP)
      option (XILINX_IMPL "use TCL Vivado/Vitis HLS script to run design implementation." OFF)
    endif ()
  endif ()
endif ()

option (ENABLE_TESTING "use Catch2 in conjunction with CTest as a test suite." ON)
option (ENABLE_SOFTWARE
        "use C++ standard library types (like std::complex). Unsuitable for synthesis." ON
)

option (PEDANTIC "use -Wall and -pedantic." ON)

# ##################################################################################################

if (PEDANTIC)
  add_compile_options (-Wall -pedantic)
endif ()

if (EXPORT_COMMANDS)
  set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif ()

if (ENABLE_XILINX)
  set (
    XILINX_HOME
    /opt/Xilinx
    CACHE PATH "path to Xilinx root folder."
  )
  set (
    XILINX_VER
    "2020.2"
    CACHE STRING "Xilinx software version to use."
  )

  if (XILINX_VER VERSION_GREATER_EQUAL "2020.1")
    set (
      AP_INCLUDE_DIR
      ${XILINX_HOME}/Vitis_HLS/${XILINX_VER}/include
      CACHE INTERNAL "Path to Xilinx includes" FORCE
    )
    find_program (
      XILINX_HLS vitis_hls HINTS ${XILINX_HOME}/Vitis_HLS/${XILINX_VER}/bin NO_CACHE REQUIRED
    )
  else ()
    message (
      FATAL_ERROR
        "Xilinx versions less than 2020.1 are not supported due to the outdated compiler version in use"
    )
  endif ()
  message (STATUS "AP headers must lie under ${AP_INCLUDE_DIR}")

else ()
  set (
    AP_TYPES_HINT
    /usr/include
    CACHE PATH "location of ap_types include directory."
  )

  find_file (
    AP_FIXED ap_fixed.h
    HINTS ${AP_TYPES_HINT}
    PATH_SUFFIXES ap_types hls_ap_types/include REQUIRED
  )

  get_filename_component (AP_INCLUDE_DIR ${AP_FIXED} DIRECTORY)
endif ()

if ((NOT EXISTS ${AP_INCLUDE_DIR}/ap_int.h) OR (NOT EXISTS ${AP_INCLUDE_DIR}/ap_fixed.h))
  message (FATAL_ERROR "Arbitrary precision headers not found in ${AP_INCLUDE_DIR}.\n"
                       "Please provide a suitable path to the headers."
  )
endif ()

if (ENABLE_SOFTWARE)
  add_compile_definitions (SOFTWARE=1)
endif ()

# ##################################################################################################

add_library (cordicabs sources/CCordicAbs/CCordicAbs.cpp)
target_include_directories (cordicabs PUBLIC sources)
target_include_directories (cordicabs SYSTEM PUBLIC ${AP_INCLUDE_DIR})

if (ENABLE_XILINX)
  add_executable (
    cordicabs_xilinx_tb sources/top_level/top_level_cordic_tb.cpp
                        sources/top_level/top_level_cordic.cpp
  )
  target_link_libraries (cordicabs_xilinx_tb cordicabs)
endif ()

if (ENABLE_TESTING)
  find_package (Catch2 REQUIRED)

  add_library (catch_common_${PROJECT_NAME} OBJECT sources/tb/main_catch2.cpp)
  target_link_libraries (catch_common_${PROJECT_NAME} PUBLIC Catch2::Catch2)

  add_executable (cordicabs_tb sources/tb/cordicabs_tb.cpp)
  target_link_libraries (cordicabs_tb catch_common_${PROJECT_NAME} cordicabs)

  include (CTest)
  include (Catch)
  catch_discover_tests (cordicabs_tb WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)

  if (ENABLE_XILINX)
    find_file (INPUT_DAT_TB input.dat HINTS data NO_CACHE REQUIRED)
    if (CMAKE_VERSION VERSION_LESS 3.19.0)
      set (TEST_NAME "\"Xilinx C-Simulation Testbench CordicAbs\"")
    else ()
      cmake_policy (SET CMP0110 NEW)
      set (TEST_NAME "Xilinx C-Simulation Testbench CordicAbs")
    endif ()
    add_test (NAME ${TEST_NAME} COMMAND cordicabs_xilinx_tb ${INPUT_DAT_TB})

    if (XILINX_COSIM)
      set (
        XILINX_TESTLINES
        "1000"
        CACHE STRING "Number of co-simulation passes"
      )

      if (XILINX_IP)
        set (
          EXPORT_IP
          "1"
          CACHE INTERNAL "EXPORT_IP"
        )
        if (XILINX_IMPL)
          set (
            RUN_IMPL
            "1"
            CACHE INTERNAL "RUN_IMPL"
          )
        endif ()
      endif ()
      add_custom_target (
        run_hls
        COMMAND ${XILINX_HLS} ${CMAKE_SOURCE_DIR}/hls_files/cordicabs_16_4_6/script_16_4_6.tcl
                ${XILINX_TESTLINES} ${EXPORT_IP} ${RUN_IMPL}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/hls_files/cordicabs_16_4_6
        USES_TERMINAL
      )

      if (CMAKE_VERSION VERSION_LESS 3.19.0)
        add_test (
          NAME "\"Xilinx HLS TCL Flow\""
          COMMAND ${XILINX_HLS} ${CMAKE_SOURCE_DIR}/hls_files/cordicabs_16_4_6/script_16_4_6.tcl
                  ${XILINX_TESTLINES} 0 0
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/hls_files/cordicabs_16_4_6
        )
      else ()
        cmake_policy (SET CMP0110 NEW)
        add_test (
          NAME "Xilinx HLS TCL Flow"
          COMMAND ${XILINX_HLS} ${CMAKE_SOURCE_DIR}/hls_files/cordicabs_16_4_6/script_16_4_6.tcl
                  ${XILINX_TESTLINES} 0 0
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/hls_files/cordicabs_16_4_6
        )
      endif ()
    endif ()
  endif ()
endif ()
