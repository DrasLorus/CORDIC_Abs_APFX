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
    set EXPORT_IP 0
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
    set CFLAGS "-std=c++14 -DXILINX_MAJOR=${XILINX_MAJOR} -Wno-unknown-pragmas -Wno-unused-label -Wall -DNDEBUG -I${ROOT_DIR}/sources"
}

set PROJECT_NAME "cordicabs_16_4_6"

open_project -reset ${PROJECT_NAME}
set_top cordic_abs_16_4_6
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/CCordicAbs/CCordicAbs.cpp"
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/CCordicAbs/CCordicAbs.hpp"
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/hls_abs/hls_abs.hpp"
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/top_level/top_level_cordic.cpp"
add_files -cflags "$CFLAGS" -tb "${ROOT_DIR}/sources/top_level/top_level_cordic_tb.cpp"


set targets [ dict create spartan7 {xc7s100-fgga484-1} genesys2 {xc7k325tffg900-2} ]

foreach target { spartan7 genesys2 } {
    if {[expr {$VERSION < 2020.1}]} {
        open_solution -reset "solution_${target}"
    } else {
        open_solution -reset "solution_${target}" -flow_target vivado
    }
    set_part [ dict get $targets $target ]
    create_clock -period 10 -name default
    set_clock_uncertainty 2
    set_directive_pipeline cordic_abs_16_4_6
    set_directive_interface cordic_abs_16_4_6 -mode ap_ctrl_none

    csim_design -argv "${ROOT_DIR}/data/input.dat" -clean -O
    csynth_design
    cosim_design -O -argv "${ROOT_DIR}/data/input.dat ${NLINES}"

    config_export -format ip_catalog            \
            -display_name "${PROJECT_NAME}"     \
            -ipname "${PROJECT_NAME}_${target}"  \
            -version "0.1.${XILINX_MAJOR}"      \
            -vivado_optimization_level 2        \
            -vivado_phys_opt route

    if { $EXPORT_IP } {
        if [ expr {! [ file isdirectory "${ROOT_DIR}/ip" ] } ] {
            if { [ file exists "${ROOT_DIR}/ip" ] } {
                file remove "${ROOT_DIR}/ip"
            }
            file mkdir "${ROOT_DIR}/ip"
        }

        if { [expr {$VERSION >= 2020.1}] } {
            export_design -output "${ROOT_DIR}/hls_files/cordicabs_16_4_6/ip/${XILINX_MAJOR}_${target}"
        } else {
            export_design
            set IP_FILE [glob -directory "${SCRIPT_DIR}/${PROJECT_NAME}/solution_${target}/impl/ip" *.zip]
            file copy -force "${IP_FILE}" "${ROOT_DIR}/hls_files/cordicabs_16_4_6/ip/${XILINX_MAJOR}_${target}.zip"
        }
    }

    if { $RUN_IMPL } {
        export_design -flow impl
    }

    close_solution
}

close_project

exit 0
