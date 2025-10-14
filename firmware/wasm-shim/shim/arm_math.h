#ifndef ARM_MATH_H_
#define ARM_MATH_H_

#include <algorithm>
#include <cmath>

typedef float float32_t;
typedef double float64_t;

// Provide math function shims if missing
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef abs
#define abs(x) ((x) < 0 ? -(x) : (x))
#endif

using std::ceil;
using std::ceilf;
using std::cos;
using std::floor;

#ifdef __cplusplus
extern "C"
{
#endif

    // Stub types for CMSIS-DSP compatibility
    typedef struct
    {
        int dummy;
    } arm_cfft_radix4_instance_q15;

    // Stub for FFT init function
    static inline int arm_cfft_radix4_init_q15(arm_cfft_radix4_instance_q15 *S, int fftLen, int ifftFlag, int bitReverseFlag)
    {
        // No-op stub for compatibility
        return 0;
    }

    typedef struct
    {
        int dummy;
    } arm_biquad_cascade_df2T_instance_f32;

    typedef struct
    {
        int dummy;
    } arm_fir_instance_q15;

    typedef struct
    {
        int dummy;
    } arm_fir_interpolate_instance_f32;

    typedef struct
    {
        int dummy;
    } arm_fir_decimate_instance_f32;

    typedef short q15_t;

#define ARM_MATH_SUCCESS 0

#ifdef __cplusplus
}
#endif

#endif // ARM_MATH_H_
// arm_math.h - ARM CMSIS DSP stub for WASM
#ifndef ARM_MATH_H_
#define ARM_MATH_H_

#include <algorithm>
#include <cmath>

typedef float float32_t;
typedef double float64_t;

// Provide math function shims if missing
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef abs
#define abs(x) ((x) < 0 ? -(x) : (x))
#endif

using std::ceil;
using std::ceilf;
using std::cos;
using std::floor;

#endif // ARM_MATH_H_
