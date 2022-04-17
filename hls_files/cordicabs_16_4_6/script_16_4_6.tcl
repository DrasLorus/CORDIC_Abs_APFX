set SCRIPT_DIR [file normalize [file dirname [info script]]]
set ROOT_DIR   "${SCRIPT_DIR}/../.."

set CFLAGS "-std=c++14 -Wno-unknown-pragmas -Wno-unused-label -Wall -DNDEBUG -I${ROOT_DIR}/sources"

open_project -reset cordicabs_16_4_6
set_top cordic_abs_16_4_6
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/CCordicAbs/CCordicAbs.cpp"
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/CCordicAbs/CCordicAbs.hpp"
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/hls_abs/hls_abs.hpp"
add_files -cflags "$CFLAGS" "${ROOT_DIR}/sources/top_level/top_level_cordic.cpp"
add_files -cflags "$CFLAGS" -tb "${ROOT_DIR}/sources/top_level/top_level_cordic_tb.cpp"

open_solution "solution_spartan7" -flow_target vivado
set_part {xc7s100fgga484-1}
create_clock -period 10 -name default
set_clock_uncertainty 2
#source "./cordicabs_16_4_6/solution/directives.tcl"
csim_design -argv "${ROOT_DIR}/data/input.dat" -clean -O
csynth_design
cosim_design -O -argv "${ROOT_DIR}/data/input.dat"
export_design -format ip_catalog
