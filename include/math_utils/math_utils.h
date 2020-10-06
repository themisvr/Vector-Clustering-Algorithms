#pragma once



namespace math_utils {

    inline uint32_t exp_modulo(uint32_t base, uint16_t exp, uint32_t mod) {
        uint32_t res = 1;

        base %= mod;
        if (base == 0) return 0;

        while (exp > 0) {
            if (exp & 1) { res = (res * base) % mod; }
            exp >>= 1;
            base = (base * base) % mod;
        }
        return res;
    }

    inline uint32_t modulo(int i, uint32_t n) {
        return ((i % n) + n) % n;
    }


}