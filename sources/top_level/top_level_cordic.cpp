/*
 *
 * Copyright 2022 Camille "DrasLorus" Monière.
 *
 * This file is part of CORDIC_Abs_APFX.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU
 * Lesser General Public License as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "top_level_cordic.hpp"

void cordic_abs_16_4_6(
    ap_int<cordic_abs_t::In_W>     re_in,
    ap_int<cordic_abs_t::In_W>     im_in,
    ap_uint<cordic_abs_t::Out_W> & module_out) {

    // static constexpr const cordic_abs_t cordic_abs {};

    const ap_int<cordic_abs_t::In_W> x = re_in;
    const ap_int<cordic_abs_t::In_W> y = im_in;

    const ap_uint<cordic_abs_t::Out_W> m = cordic_abs_t::process(x, y);

    module_out = m;
}
