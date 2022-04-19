set VERSION [version -short]

set SCRIPT_DIR [file normalize [file dirname [info script]]]
set ROOT_DIR   "${SCRIPT_DIR}/../.."

# if {[expr {$VERSION < 2020.1}]} {
#     error "Version less than 2020.1 is not supported."
# }

if { [expr {$argc > 0}] } {
    set NLINES [lindex $argv 0]
} else {
    set NLINES 10000
}

if { [expr {$argc > 1}] } {
    set EXPORT_IP [lindex $argv 1]
} else {
    set EXPORT_IP 1
}

if { [expr {$argc > 2}] && $EXPORT_IP } {
    set RUN_IMPL [lindex $argv 2]
} else {
    set RUN_IMPL 0
}


set XILINX_MAJOR [expr {int($VERSION)}]
if {[expr {$VERSION < 2020.1}]} {
    set CFLAGS "-std=c++0x -DXILINX_MAJOR=${XILINX_MAJOR} -Wno-unknown-pragmas -Wno-unused-label -Wall -DNDEBUG -I${ROOT_DIR}/sources"
} else {
    set CFLAGS "-std=c++14 -Wno-unknown-pragmas -Wno-unused-label -Wall -DNDEBUG -I${ROOT_DIR}/sources"
}

set PROJECT_NAME "cordicabs_16_4_6"

open_project -reset ${PROJECT_NAME}
set_top cordic_abs_16_4_6
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/CCordicAbs/CCordicAbs.cpp"
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/CCordicAbs/CCordicAbs.hpp"
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/hls_abs/hls_abs.hpp"
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/top_level/top_level_cordic.cpp"
add_files -cflags "$CFLAGS" -tb "${ROOT_DIR}/sources/top_level/top_level_cordic_tb.cpp"

if {[expr {$VERSION < 2020.1}]} {
    open_solution -reset "solution_spartan7"
} else {
    open_solution -reset "solution_spartan7" -flow_target vivado
}
set_part {xc7s100-fgga484-1}
create_clock -period 10 -name default
set_clock_uncertainty 2
set_directive_pipeline cordic_abs_16_4_6
set_directive_interface cordic_abs_16_4_6 -mode ap_ctrl_none
csim_design -argv "${ROOT_DIR}/data/input.dat" -clean -O
csynth_design
cosim_design -O -argv "${ROOT_DIR}/data/input.dat ${NLINES}"

if { $EXPORT_IP } {
    config_export -format ip_catalog            \
            -display_name "${PROJECT_NAME}"    \
            -ipname "${PROJECT_NAME}_spartan7" \
            -version "0.1.${XILINX_MAJOR}"
    if { [expr {$VERSION >= 2020.1}] } {
        config_export -output "${ROOT_DIR}/hls_files/cordicabs_16_4_6/ip/${XILINX_MAJOR}_spartan7"
    }
    if { $RUN_IMPL } {
        export_design -flow impl
    } else {
        export_design
    }
    if { [expr {$VERSION < 2020.1}] } {
        set IP_FILE [glob -directory "${SCRIPT_DIR}/${PROJECT_NAME}/solution_spartan7/impl/ip" *.zip]
        file copy -force "${IP_FILE}" "${ROOT_DIR}/hls_files/cordicabs_16_4_6/ip/${XILINX_MAJOR}_spartan7.zip"
    }
}

close_solution

if {[expr {$VERSION < 2020.1}]} {
    open_solution -reset "solution_genesys2"
} else {
    open_solution -reset "solution_genesys2" -flow_target vivado
}
set_part {xc7k325tffg900-2}
create_clock -period 10 -name default
set_clock_uncertainty 2
set_directive_pipeline cordic_abs_16_4_6
set_directive_interface cordic_abs_16_4_6 -mode ap_ctrl_none
csim_design -argv "${ROOT_DIR}/data/input.dat" -clean -O
csynth_design
cosim_design -O -argv "${ROOT_DIR}/data/input.dat ${NLINES}"

if { $EXPORT_IP } {
    config_export -format ip_catalog            \
            -display_name "${PROJECT_NAME}"    \
            -ipname "${PROJECT_NAME}_genesys2" \
            -version "0.1.${XILINX_MAJOR}"
    if { [expr {$VERSION >= 2020.1}] } {
        config_export -output "${ROOT_DIR}/hls_files/cordicabs_16_4_6/ip/${XILINX_MAJOR}_genesys2"
    }
    if { $RUN_IMPL } {
        export_design -flow impl
    } else {
        export_design
    }
    if { [expr {$VERSION < 2020.1}] } {
        set IP_FILE [glob -directory "${SCRIPT_DIR}/${PROJECT_NAME}/solution_genesys2/impl/ip" *.zip]
        file copy -force "${IP_FILE}" "${ROOT_DIR}/hls_files/cordicabs_16_4_6/ip/${XILINX_MAJOR}_genesys2.zip"
    }
}

close_solution
close_project

exit 0
