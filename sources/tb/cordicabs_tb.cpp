/*
 *
 * Copyright 2022 Camille "DrasLorus" Monière.
 *
 * This file is part of CORDIC_Rotate_APFX.
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

#include "CCordicAbs/CCordicAbs.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

#include <cmath>
#include <complex>

#include <catch2/catch.hpp>

using namespace std;

using Catch::Matchers::Floating::WithinAbsMatcher;

#if defined(SOFTWARE)
TEST_CASE("Constexpr CordicAbs works with C-Types", "[CORDICABS]") {
    SECTION("W:16 - I:4 - Stages:6 - double") {
        typedef CCordicAbs<16, 4, 6> cordic_abs;

        string input_fn  = "../data/input.dat";  // _8_14_4_17_5_19_7_12
        string output_fn = "../data/output.dat"; // _8_14_4_17_5_19_7_12

        constexpr unsigned n_lines = 100000;

        complex<double> values_in[n_lines];
        double          values_out[n_lines];

        double results[n_lines];

        FILE * INPUT = fopen(input_fn.c_str(), "r");

        // Init test vector
        for (unsigned i = 0; i < n_lines; i++) {
            double a, b, r;
            fscanf(INPUT, "%lf,%lf,%lf\n", &a, &b, &r);

            const complex<double> c {a, b};
            values_in[i] = c;

            const double ac = std::abs(c);
            results[i]      = ac;
        }

        fclose(INPUT);

        // Save the results to a file
        // ofstream outfile("results.dat");

        constexpr double abs_margin = double(1 << (cordic_abs::Out_I - 1)) * 2. / 100.;

        // Executing the encoder
        for (unsigned iter = 0; iter < n_lines; iter++) {
            // Execute

            values_out[iter] = cordic_abs::process(values_in[iter]);

            // Display the results
            // cout << "Series " << iter;
            // cout << " Outcome: ";

            // outfile << values_in[iter].real() << " " << values_in[iter].imag() << " " << values_out[iter] << " " << results[iter] << endl;

            REQUIRE_THAT(values_out[iter], WithinAbsMatcher(results[iter], abs_margin));
        }
        // outfile.close();
    }

    SECTION("W:16 - I:4 - Stages:6 - int64") {
        typedef CCordicAbs<16, 4, 6> cordic_abs;

        string input_fn  = "../data/input.dat";  // _8_14_4_17_5_19_7_12
        string output_fn = "../data/output.dat"; // _8_14_4_17_5_19_7_12

        constexpr unsigned n_lines = 100000;

        complex<int64_t> values_in[n_lines];
        int64_t          values_out[n_lines];

        double results[n_lines];

        FILE * INPUT = fopen(input_fn.c_str(), "r");

        // Init test vector
        for (unsigned i = 0; i < n_lines; i++) {
            double a, b, r;
            fscanf(INPUT, "%lf,%lf,%lf\n", &a, &b, &r);

            const complex<double>  c {a, b};
            const complex<int64_t> ic {(int64_t) floor(c.real() * cordic_abs::in_scale_factor),
                                       (int64_t) floor(c.imag() * cordic_abs::in_scale_factor)};
            values_in[i] = ic;

            const double ac = std::abs(c);
            results[i]      = ac;
        }

        fclose(INPUT);

        // Save the results to a file
        // ofstream outfile("results.dat");

        constexpr double abs_margin = double(1 << (cordic_abs::Out_I - 1)) * 2. / 100.;

        // Executing the encoder
        for (unsigned iter = 0; iter < n_lines; iter++) {
            // Execute

            values_out[iter] = cordic_abs::process(values_in[iter]);

            // Display the results
            // cout << "Series " << iter;
            // cout << " Outcome: ";

            // outfile << values_in[iter].real() << " " << values_in[iter].imag() << " " << values_out[iter] << " " << results[iter] << endl;

            REQUIRE_THAT(cordic_abs::scale_cordic(double(values_out[iter])) / cordic_abs::out_scale_factor, WithinAbsMatcher(results[iter], abs_margin));
        }
        // outfile.close();
    }
}
#endif

TEST_CASE("Constexpr CordicAbs works with AP-Types", "[CORDICABS]") {
    SECTION("W:16 - I:4 - Stages:6") {
        typedef CCordicAbs<16, 4, 6> cordic_abs;

        string input_fn = "../data/input.dat"; // _8_14_4_17_5_19_7_12

        constexpr unsigned n_lines = 100000;

        ap_int<cordic_abs::In_W>   re_values_in[n_lines];
        ap_int<cordic_abs::In_W>   im_values_in[n_lines];
        ap_uint<cordic_abs::Out_W> values_out[n_lines];

        double results[n_lines];

        FILE * INPUT = fopen(input_fn.c_str(), "r");

        if (!bool(INPUT)) {
            throw(string("fopen failed for ") + input_fn + string(": ") + string(strerror(errno)));
        }

        // Init test vector
        for (unsigned i = 0; i < n_lines; i++) {
            double a, b, r;
            fscanf(INPUT, "%lf,%lf,%lf\n", &a, &b, &r);

            re_values_in[i] = int64_t(floor(a * cordic_abs::in_scale_factor));
            im_values_in[i] = int64_t(floor(b * cordic_abs::in_scale_factor));

            const double ac = std::abs(complex<double> {a, b});
            results[i]      = ac;
        }

        fclose(INPUT);

        // Save the results to a file
        // ofstream outfile("results.dat");

        constexpr double abs_margin = double(1 << (cordic_abs::Out_I - 1)) * 2. / 100.;

        // Executing the encoder
        for (unsigned iter = 0; iter < n_lines; iter++) {
            // Execute

            values_out[iter] = cordic_abs::process(re_values_in[iter], im_values_in[iter]);

            // Display the results
            // cout << "Series " << iter;
            // cout << " Outcome: ";

            // outfile << re_values_in[iter].to_double() / cordic_abs::in_scale_factor << " "
            //         << im_values_in[iter].to_double() / cordic_abs::in_scale_factor << " "
            //         << cordic_abs::scale_cordic(values_out[iter].to_double()) / cordic_abs::out_scale_factor << " "
            //         << results[iter] << endl;

            const double dbl_res = cordic_abs::scale_cordic(values_out[iter].to_double()) / cordic_abs::out_scale_factor;

            REQUIRE_THAT(dbl_res, WithinAbsMatcher(results[iter], abs_margin));
        }
        // outfile.close();

        // Compare the results file with the golden results
        // int retval = 0;
        // Return 0 if the test passed
    }
}

#if defined(SOFTWARE)
TEST_CASE("Constexpr CordicAbs are evaluated during compilation.", "[CORDICABS]") {
    SECTION("W:16 - I:4 - Stages:6 - C-Types") {
        typedef CCordicAbs<16, 4, 6> cordic_abs;

        constexpr const complex<int64_t> value_in[3] = {(1U << 12) * 97, -(1U << 12) * 33, (1U << 3) * 12};

        constexpr int64_t res10 = cordic_abs::process(value_in[0]);
        constexpr int64_t res20 = cordic_abs::process(value_in[0]);
        static_assert(res10 == res20, "Test");
        REQUIRE_FALSE(res10 == cordic_abs::process(complex<int64_t>(1, 0)));
        REQUIRE(res10 == cordic_abs::process(value_in[0]));

        constexpr int64_t res11 = cordic_abs::process(value_in[1]);
        constexpr int64_t res21 = cordic_abs::process(value_in[1]);
        static_assert(res11 == res21, "Test");
        REQUIRE_FALSE(res11 == cordic_abs::process(complex<int64_t>(1, 0)));
        REQUIRE(res11 == cordic_abs::process(value_in[1]));

        constexpr int64_t res12 = cordic_abs::process(value_in[2]);
        constexpr int64_t res22 = cordic_abs::process(value_in[2]);
        static_assert(res12 == res22, "Test");
        REQUIRE_FALSE(res12 == cordic_abs::process(complex<int64_t>(1, 0)));
        REQUIRE(res12 == cordic_abs::process(value_in[2]));
    }
}
#endif