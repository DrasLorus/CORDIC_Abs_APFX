
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

#include <complex>
#include <fstream>
#include <ostream>
#include <string>

using std::complex;
using std::ofstream;
using std::string;

int main(int argc, char ** argv) {
    const string testname    = "Constexpr CordicAbs works with AP-Types and Xilinx synthesis";
    const string testsection = "W:16 - I:4 - Stages:6";

    printf("# TEST:     %s\n", testname.c_str());
    printf("# SETTINGS: %s\n", testsection.c_str());
    printf("## Compile date: %s\n", __DATE__);
    printf("##         time: %s\n", __TIME__);

    string input_fn = "../data/input.dat"; // _8_14_4_17_5_19_7_12

    if (argc > 1) {
        input_fn = string(argv[1]);
    }

    constexpr unsigned max_lines = 100000;
    unsigned           n_lines   = max_lines;
    if (argc > 2) {
        n_lines = std::min(unsigned(std::stoi(string(argv[2]))), max_lines);
    }

    ap_int<cordic_abs_t::In_W>   re_values_in[max_lines];
    ap_int<cordic_abs_t::In_W>   im_values_in[max_lines];
    ap_uint<cordic_abs_t::Out_W> values_out[max_lines];

    double results[max_lines];

    FILE * INPUT = fopen(input_fn.c_str(), "r");
    if (!bool(INPUT)) {
        perror("Can't open input file");
        exit(EXIT_FAILURE);
    }

    // Init test vector
    for (unsigned i = 0; i < n_lines; i++) {
        double a, b, r;
        fscanf(INPUT, "%lf,%lf,%lf\n", &a, &b, &r);

        re_values_in[i] = int64_t(floor(a * cordic_abs_t::in_scale_factor));
        im_values_in[i] = int64_t(floor(b * cordic_abs_t::in_scale_factor));

        const double ac = std::abs(complex<double> {a, b});
        results[i]      = ac;
    }

    fclose(INPUT);

    // Save the results to a file
    ofstream outfile("results.dat");

    constexpr double abs_margin = double(1 << (cordic_abs_t::Out_I - 1)) * 2. / 100.;

    int counted_errors = 0;

    // Executing the encoder
    for (int iter = 0; iter < int(n_lines); iter++) {
        // Execute

        cordic_abs_16_4_6(re_values_in[iter], im_values_in[iter], values_out[iter]);

        // Display the results
        // cout << "Series " << iter;
        // cout << " Outcome: ";

        outfile << re_values_in[iter].to_double() / cordic_abs_t::in_scale_factor << " "
                << im_values_in[iter].to_double() / cordic_abs_t::in_scale_factor << " "
                << cordic_abs_t::scale_cordic(values_out[iter].to_double()) / cordic_abs_t::out_scale_factor << " "
                << results[iter] << std::endl;

        const double dbl_res = cordic_abs_t::scale_cordic(values_out[iter].to_double()) / cordic_abs_t::out_scale_factor;

        if (std::abs(dbl_res - results[iter]) > abs_margin) {
            counted_errors++;
            if (counted_errors < 100) {
                std::cerr << dbl_res << " is not in the margin of " << abs_margin << " from " << results[iter] << std::endl;
            } else if (counted_errors == 100) {
                std::cerr << "Too much errors to be reported on terminal." << std::endl;
            }
        }
    }
    outfile.close();

    printf("RESULT: %s!\n", counted_errors == 0 ? "SUCCESS" : "FAILURE");

    // Return 0 if the test passed
    return counted_errors;
}
