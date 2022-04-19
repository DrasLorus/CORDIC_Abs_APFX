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

#if !defined (XILINX_MAJOR) || XILINX_MAJOR >= 2020

void cordic_abs_16_4_6(
    ap_int<cordic_abs_t::In_W>     re_in,
    ap_int<cordic_abs_t::In_W>     im_in,
    ap_uint<cordic_abs_t::Out_W> & module_out) {

    static constexpr const cordic_abs_t cordic_abs {};

    const ap_int<cordic_abs_t::In_W> x = re_in;
    const ap_int<cordic_abs_t::In_W> y = im_in;

    const ap_uint<cordic_abs_t::Out_W> m = cordic_abs.process(x, y);

    module_out = m;
}

#else

constexpr unsigned In_W  = cordic_abs_t::In_W;
constexpr unsigned Out_W = cordic_abs_t::Out_W;
constexpr unsigned nb_stages = cordic_abs_t::nb_stages;

static ap_uint<Out_W> process(ap_int<In_W> re_in, ap_int<In_W> im_in) {
    ap_int<Out_W> A[nb_stages + 1];
    ap_int<Out_W> B[nb_stages + 1];

    A[0] = hls_abs<false>::abs(re_in);
    B[0] = hls_abs<false>::abs(im_in);

    for (uint16_t u = 0; u < nb_stages; u++) {
        const bool sign_B = B[u] > 0;

        const ap_int<Out_W> step_A = B[u] >> u;
        const ap_int<Out_W> step_B = A[u] >> u;

        const ap_int<Out_W> tmp_B = sign_B
                                      ? ap_int<Out_W>(B[u] - step_B)
                                      : ap_int<Out_W>(B[u] + step_B);
        const ap_int<Out_W> tmp_A = sign_B
                                      ? ap_int<Out_W>(A[u] + step_A)
                                      : ap_int<Out_W>(A[u] - step_A);

        A[u + 1] = tmp_A;
        B[u + 1] = tmp_B;
    }

    return ap_uint<Out_W>(A[nb_stages]);
}

void cordic_abs_16_4_6(
    ap_int<cordic_abs_t::In_W>     re_in,
    ap_int<cordic_abs_t::In_W>     im_in,
    ap_uint<cordic_abs_t::Out_W> & module_out) {

    const ap_int<cordic_abs_t::In_W> x = re_in;
    const ap_int<cordic_abs_t::In_W> y = im_in;

    const ap_uint<cordic_abs_t::Out_W> m = process(x, y);

    module_out = m;
}

#endif
