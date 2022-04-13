/*
 *
 * Copyright 2022 Camille "DrasLorus" Monière.
 *
 * This file is part of CORDIC_ABS_APFX.
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

#ifndef C_CORDIC_ABS_HPP
#define C_CORDIC_ABS_HPP

#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>

#include <complex>

#include <ap_fixed.h>
#include <ap_int.h>

#define cst_abs(x) (x > 0 ? x : -x)

template <unsigned TIn_W, unsigned TIn_I, unsigned Tnb_stages>
class CCordicAbs {
public:
    static constexpr double kn_values[7] = {
        0.70710678118655, 0.632455532033680, 0.613571991077900,
        0.608833912517750, 0.607648256256170, 0.607351770141300, 0.607277644093530};

    static constexpr const unsigned In_W      = TIn_W;
    static constexpr const unsigned In_I      = TIn_I;
    static constexpr const unsigned Out_W     = In_W + 2;
    static constexpr const unsigned Out_I     = In_I + 2;
    static constexpr const unsigned nb_stages = Tnb_stages;

    static constexpr unsigned kn_i             = unsigned(kn_values[nb_stages - 1] * double(1U << 3)); // 3 bits are enough
    static constexpr unsigned in_scale_factor  = unsigned(1U << (In_W - In_I));
    static constexpr unsigned out_scale_factor = unsigned(1U << (Out_W - Out_I));

    static constexpr int64_t scale_cordic(int64_t in) {
        return in * kn_i / 8U;
    }

#if !defined(__SYNTHESIS__) && defined(SOFTWARE)
    static constexpr int64_t process(const std::complex<int64_t> & x_in) {

        const int64_t re_x = x_in.real();
        const int64_t im_x = x_in.imag();

        int64_t A = cst_abs(re_x);
        int64_t B = cst_abs(im_x);

        for (uint16_t u = 1; u < nb_stages + 1; u++) {

            const bool sign_B = B > 0;

            const int64_t step_A = +B / int64_t(1U << (u - 1));
            const int64_t step_B = -A / int64_t(1U << (u - 1));

            B = sign_B ? B + step_B : B - step_B;
            A = sign_B ? A + step_A : A - step_A;
        }

        return A;
    }

    static constexpr double scale_cordic(double in) {
        return in * kn_values[nb_stages - 1];
    }

    static constexpr double process(std::complex<double> x_in) {
        const std::complex<int64_t> fx_x_in(int64_t(x_in.real() * double(in_scale_factor)),
                                            int64_t(x_in.imag() * double(in_scale_factor)));

        const int64_t fx_out = process(fx_x_in);
        return scale_cordic(double(fx_out)) / double(out_scale_factor);
    }

#endif

    constexpr CCordicAbs() = default;
    ~CCordicAbs()          = default;
};

#undef cst_abs

#endif
