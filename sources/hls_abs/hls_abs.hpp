#ifndef _HLS_ABS_HPP_
#define _HLS_ABS_HPP_

#include <ap_int.h>

template <bool balanced>
struct hls_abs {
    template <int _Tsize>
    static ap_uint<_Tsize> abs(ap_int<_Tsize> value);

    template <int _Tsize>
    static ap_uint<_Tsize> abs(ap_uint<_Tsize> value) {
        return value;
    }
};

template <>
struct hls_abs<true> {
    template <int _Tsize>
    static ap_uint<_Tsize> abs(ap_int<_Tsize> value) {
        const ap_uint<_Tsize - 1> u_value    = value.range(_Tsize - 2, 0);
        const bool                sign_value = value.bit(_Tsize - 1);

        const ap_uint<_Tsize> a_value = sign_value
                                          ? ap_uint<_Tsize>((~u_value) + 1U)
                                          : ap_uint<_Tsize>(u_value);

        return a_value;
    }
};

template <>
struct hls_abs<false> {
    template <int _Tsize>
    static ap_uint<_Tsize + 1> abs(ap_int<_Tsize> value) {
        const ap_uint<_Tsize - 1> u_value    = value.range(_Tsize - 2, 0);
        const bool                sign_value = value.bit(_Tsize - 1);

        const ap_uint<_Tsize + 1> a_value = sign_value
                                              ? ap_uint<_Tsize + 1>((~u_value) + true)
                                              : ap_uint<_Tsize + 1>(u_value);

        return a_value;
    }
};

#endif