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
#include <fstream>
#include <iostream>

#include <catch2/catch.hpp>

using namespace std;

using Catch::Matchers::Floating::WithinAbsMatcher;

#if defined(SOFTWARE)
TEST_CASE("Constexpr CordicAbs works with C-Types", "[CORDIC]") {
    SECTION("W:16 - I:4 - Stages:6 - q:64") {
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
        ofstream outfile("results.dat");

        constexpr double abs_margin = double(1 << (cordic_abs::Out_I - 1)) * 2. / 100.;

        // Executing the encoder
        for (unsigned iter = 0; iter < n_lines; iter++) {
            // Execute

            values_out[iter] = cordic_abs::process(values_in[iter]);

            // Display the results
            // cout << "Series " << iter;
            // cout << " Outcome: ";

            outfile << values_in[iter].real() << " " << values_in[iter].imag() << " " << values_out[iter] << " " << results[iter] << endl;

            REQUIRE_THAT(values_out[iter], WithinAbsMatcher(results[iter], abs_margin));
        }
        outfile.close();

        // Compare the results file with the golden results
        // int retval = 0;
        // Return 0 if the test passed
    }
}
#endif

// TEST_CASE("ROM-based Cordic works with AP-Types", "[CORDIC]") {
//     constexpr unsigned n_lines = 100000;

//     SECTION("W:16 - I:4 - Stages:6 - q:64") {
//         typedef CCordicAbs<16, 4, 6> cordic_abs;

//         static constexpr cordic_abs cordic {};

//         string input_fn = "../data/input.dat";

//         constexpr double   rotation = cordic_abs::rotation;
//         constexpr double   q        = cordic_abs::rom_cordic.q;
//         constexpr uint64_t cnt_mask = 0xFF; // Value dependant of the way the ROM is initialized

//         constexpr unsigned Out_W = cordic_abs::Out_W;
//         constexpr unsigned In_W  = cordic_abs::In_W;

//         ap_int<In_W>  values_re_in[n_lines];
//         ap_int<In_W>  values_im_in[n_lines];
//         ap_int<Out_W> values_re_out[n_lines];
//         ap_int<Out_W> values_im_out[n_lines];

//         double results_re[n_lines];
//         double results_im[n_lines];

//         // ofstream out_stream;

//         ifstream INPUT(input_fn);

//         // Init test vector
//         for (unsigned i = 0; i < n_lines; i++) {
//             double a, b, r;
//             INPUT >> a >> b >> r;

//             const complex<double> c {a, b};
//             values_re_in[i] = int64_t(a * double(cordic_abs::in_scale_factor));
//             values_im_in[i] = int64_t(b * double(cordic_abs::in_scale_factor));

//             const complex<double> e = c * exp(complex<double>(0., rotation / q * (i & cnt_mask)));
//             results_re[i]           = e.real();
//             results_im[i]           = e.imag();
//         }

//         INPUT.close();

//         // Save the results to a file
//         // out_stream.open("results_ap.dat");
//         // FILE * romf = fopen("rom.dat", "w");

//         constexpr double abs_margin = double(1 << (cordic.Out_I - 1)) * 2. / 100.;

//         // Executing the encoder
//         for (unsigned iter = 0; iter < n_lines; iter++) {
//             // Execute
//             const uint8_t counter = uint8_t(iter & cnt_mask);

//             // if (iter < cnt_mask + 1)
//             //     fprintf(romf, "%03d\n", (uint16_t) cordic.rom_cordic.rom[counter]);

//             cordic_abs::process(
//                 values_re_in[iter], values_im_in[iter],
//                 counter,
//                 values_re_out[iter], values_im_out[iter]);

//             // Display the results
//             // cout << "Series " << iter;
//             // cout << " Outcome: ";

//             // out_stream << values_re_out[iter].to_int64() << " " << values_im_out[iter].to_int64() << " " << results_re[iter] << " " << results_im[iter] << endl;

//             REQUIRE_THAT(values_re_out[iter].to_double() * 5. / 8. / cordic_abs::out_scale_factor, WithinAbsMatcher(results_re[iter], abs_margin));
//             REQUIRE_THAT(values_im_out[iter].to_double() * 5. / 8. / cordic_abs::out_scale_factor, WithinAbsMatcher(results_im[iter], abs_margin));
//         }
//         // out_stream.close();
//         // fclose(romf);

//         // Compare the results file with the golden results
//         // int retval = 0;
//         // Return 0 if the test passed
//     }

//     SECTION("W:16 - I:4 - Stages:6 - q:64 - internal scaling") {
//         typedef CCordicAbs<16, 4, 6> cordic_abs;
//         static constexpr cordic_abs                  cordic {};

//         string input_fn = "../data/input.dat";

//         constexpr double   rotation = cordic_abs::rotation;
//         constexpr double   q        = cordic_abs::rom_cordic.q;
//         constexpr uint64_t cnt_mask = 0xFF; // Value dependant of the way the ROM is initialized

//         constexpr unsigned Out_W = cordic_abs::Out_W;
//         constexpr unsigned In_W  = cordic_abs::In_W;

//         ap_int<In_W>  values_re_in[n_lines];
//         ap_int<In_W>  values_im_in[n_lines];
//         ap_int<Out_W> values_re_out[n_lines];
//         ap_int<Out_W> values_im_out[n_lines];

//         double results_re[n_lines];
//         double results_im[n_lines];

//         // ofstream out_stream;

//         ifstream INPUT(input_fn);

//         // Init test vector
//         for (unsigned i = 0; i < n_lines; i++) {
//             double a, b, r;
//             INPUT >> a >> b >> r;

//             const complex<double> c {a, b};
//             values_re_in[i] = int64_t(a * double(cordic_abs::in_scale_factor));
//             values_im_in[i] = int64_t(b * double(cordic_abs::in_scale_factor));

//             const complex<double> e = c * exp(complex<double>(0., rotation / q * (i & cnt_mask)));
//             results_re[i]           = e.real();
//             results_im[i]           = e.imag();
//         }

//         INPUT.close();

//         // Save the results to a file
//         // out_stream.open("results_ap.dat");
//         // FILE * romf = fopen("rom.dat", "w");

//         constexpr double abs_margin = double(1 << (cordic.Out_I - 1)) * 3. / 100.; // Internal scaling create noise

//         // Executing the encoder
//         for (unsigned iter = 0; iter < n_lines; iter++) {
//             // Execute
//             const uint8_t counter = uint8_t(iter & cnt_mask);

//             // if (iter < cnt_mask + 1)
//             //     fprintf(romf, "%03d\n", (uint16_t) cordic.rom_cordic.rom[counter]);

//             cordic_abs::process(
//                 values_re_in[iter], values_im_in[iter],
//                 counter,
//                 values_re_out[iter], values_im_out[iter]);

//             // Display the results
//             // cout << "Series " << iter;
//             // cout << " Outcome: ";

//             // out_stream << values_re_out[iter].to_int64() << " " << values_im_out[iter].to_int64() << " " << results_re[iter] << " " << results_im[iter] << endl;

//             REQUIRE_THAT(cordic_abs::scale_cordic(values_re_out[iter]).to_double() / cordic_abs::out_scale_factor,
//                          WithinAbsMatcher(results_re[iter],
//                                           abs_margin));
//             REQUIRE_THAT(cordic_abs::scale_cordic(values_im_out[iter]).to_double() / cordic_abs::out_scale_factor,
//                          WithinAbsMatcher(results_im[iter],
//                                           abs_margin));
//         }
//         // out_stream.close();
//         // fclose(romf);

//         // Compare the results file with the golden results
//         // int retval = 0;
//         // Return 0 if the test passed
//     }

//     SECTION("W:16 - I:4 - Stages:6 - q:64 - divider:4") {
//         typedef CCordicAbs<16, 4, 6> cordic_abs;

//         static constexpr cordic_abs cordic {};

//         string input_fn = "../data/input.dat";

//         constexpr double   rotation = cordic_abs::rotation;
//         constexpr double   q        = cordic_abs::rom_cordic.q;
//         constexpr uint64_t cnt_mask = 0xFF; // Value dependant of the way the ROM is initialized

//         constexpr unsigned Out_W = cordic_abs::Out_W;
//         constexpr unsigned In_W  = cordic_abs::In_W;

//         ap_int<In_W>  values_re_in[n_lines];
//         ap_int<In_W>  values_im_in[n_lines];
//         ap_int<Out_W> values_re_out[n_lines];
//         ap_int<Out_W> values_im_out[n_lines];

//         double results_re[n_lines];
//         double results_im[n_lines];

//         // ofstream out_stream;

//         ifstream INPUT(input_fn);

//         // Init test vector
//         for (unsigned i = 0; i < n_lines; i++) {
//             double a, b, r;
//             INPUT >> a >> b >> r;

//             const complex<double> c {a, b};
//             values_re_in[i] = int64_t(a * double(cordic_abs::in_scale_factor));
//             values_im_in[i] = int64_t(b * double(cordic_abs::in_scale_factor));

//             const complex<double> e = c * exp(complex<double>(0., rotation / q * (i & cnt_mask)));
//             results_re[i]           = e.real();
//             results_im[i]           = e.imag();
//         }

//         INPUT.close();

//         // Save the results to a file
//         // out_stream.open("results_ap.dat");
//         // FILE * romf = fopen("rom.dat", "w");

//         constexpr double abs_margin = double(1 << (cordic.Out_I - 1)) * 2. / 100.;

//         // Executing the encoder
//         for (unsigned iter = 0; iter < n_lines; iter++) {
//             // Execute
//             const uint8_t counter = uint8_t(iter & cnt_mask);

//             // if (iter < cnt_mask + 1)
//             //     fprintf(romf, "%03d\n", (uint16_t) cordic.rom_cordic.rom[counter]);

//             cordic_abs::process(
//                 values_re_in[iter], values_im_in[iter],
//                 counter,
//                 values_re_out[iter], values_im_out[iter]);

//             // Display the results
//             // cout << "Series " << iter;
//             // cout << " Outcome: ";

//             // out_stream << values_re_out[iter].to_int64() << " " << values_im_out[iter].to_int64() << " " << results_re[iter] << " " << results_im[iter] << endl;

//             REQUIRE_THAT(values_re_out[iter].to_double() * 5. / 8. / cordic_abs::out_scale_factor, WithinAbsMatcher(results_re[iter], abs_margin));
//             REQUIRE_THAT(values_im_out[iter].to_double() * 5. / 8. / cordic_abs::out_scale_factor, WithinAbsMatcher(results_im[iter], abs_margin));
//         }
//         // out_stream.close();
//         // fclose(romf);

//         // Compare the results file with the golden results
//         // int retval = 0;
//         // Return 0 if the test passed
//     }

//     SECTION("W:16 - I:4 - Stages:6 - q:64 - divider:4 - internal scaling") {
//         typedef CCordicAbs<16, 4, 6> cordic_abs;
//         static constexpr cordic_abs                     cordic {};

//         string input_fn = "../data/input.dat";

//         constexpr double   rotation = cordic_abs::rotation;
//         constexpr double   q        = cordic_abs::rom_cordic.q;
//         constexpr uint64_t cnt_mask = 0xFF; // Value dependant of the way the ROM is initialized

//         constexpr unsigned Out_W = cordic_abs::Out_W;
//         constexpr unsigned In_W  = cordic_abs::In_W;

//         ap_int<In_W>  values_re_in[n_lines];
//         ap_int<In_W>  values_im_in[n_lines];
//         ap_int<Out_W> values_re_out[n_lines];
//         ap_int<Out_W> values_im_out[n_lines];

//         double results_re[n_lines];
//         double results_im[n_lines];

//         ofstream out_stream;

//         ifstream INPUT(input_fn);

//         // Init test vector
//         for (unsigned i = 0; i < n_lines; i++) {
//             double a, b, r;
//             INPUT >> a >> b >> r;

//             const complex<double> c {a, b};
//             values_re_in[i] = int64_t(a * double(cordic_abs::in_scale_factor));
//             values_im_in[i] = int64_t(b * double(cordic_abs::in_scale_factor));

//             const complex<double> e = c * exp(complex<double>(0., rotation / q * (i & cnt_mask)));
//             results_re[i]           = e.real();
//             results_im[i]           = e.imag();
//         }

//         INPUT.close();

//         // Save the results to a file
//         // out_stream.open("results_ap.dat");
//         // FILE * romf = fopen("rom.dat", "w");

//         constexpr double abs_margin = double(1 << (cordic.Out_I - 1)) * 3. / 100.; // Internal scaling creates noise

//         // Executing the encoder
//         for (unsigned iter = 0; iter < n_lines; iter++) {
//             // Execute
//             const uint8_t counter = uint8_t(iter & cnt_mask);

//             // if (iter < cnt_mask + 1)
//             //     fprintf(romf, "%03d\n", (uint16_t) cordic.rom_cordic.rom[counter]);

//             cordic_abs::process(
//                 values_re_in[iter], values_im_in[iter],
//                 counter,
//                 values_re_out[iter], values_im_out[iter]);

//             // Display the results
//             // cout << "Series " << iter;
//             // cout << " Outcome: ";

//             // out_stream << cordic_abs::scale_cordic(values_re_out[iter]).to_double() / cordic_abs::out_scale_factor << " "
//             //            << cordic_abs::scale_cordic(values_im_out[iter]).to_double() / cordic_abs::out_scale_factor << " "
//             //            << results_re[iter] << " "
//             //            << results_im[iter] << endl;

//             REQUIRE_THAT(cordic_abs::scale_cordic(values_re_out[iter]).to_double() / cordic_abs::out_scale_factor,
//                          WithinAbsMatcher(results_re[iter], abs_margin));
//             REQUIRE_THAT(cordic_abs::scale_cordic(values_im_out[iter]).to_double() / cordic_abs::out_scale_factor,
//                          WithinAbsMatcher(results_im[iter], abs_margin));
//         }
//         // out_stream.close();
//         // fclose(romf);

//         // Compare the results file with the golden results
//         // int retval = 0;
//         // Return 0 if the test passed
//     }
// }

#if defined(SOFTWARE)
TEST_CASE("Constexpr CordicAbs are evaluated during compilation.", "[CORDIC]") {
    SECTION("W:16 - I:4 - Stages:6 - q:64 - C-Types") {
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