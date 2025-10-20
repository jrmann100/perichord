/* dspinst_wasm.h
 *
 * WASM-friendly replacements of Teensy DSP intrinsics / asm.
 * Implements the same semantics as the ARM inline asm in the original
 * Teensy Audio Library, but in portable C for use under Emscripten/WASM.
 *
 * Usage:
 *   #include "dspinst_wasm.h"
 *
 * The original file used inline asm for __ARM_ARCH_7EM__ and KINETISL;
 * this header provides the equivalent implementations for non-ARM builds.
 */

#ifndef DSPINST_WASM_H_
#define DSPINST_WASM_H_

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

/* portable arithmetic right shift for 32-bit signed values (no UB) */
static inline int32_t arith_rshift32(int32_t v, int s)
{
	if (s <= 0)
		return v;
	/* shift unsigned bits then sign-extend if negative */
	uint32_t uv = (uint32_t)v;
	uint32_t shifted = uv >> (unsigned)s;
	if (v < 0)
	{
		uint32_t mask = ~((uint32_t)0 >> (unsigned)s);
		shifted |= mask;
	}
	return (int32_t)shifted;
}

/* clamp helpers */
static inline int32_t clamp_i32_from_i64(int64_t x)
{
	if (x > INT32_MAX)
		return INT32_MAX;
	if (x < INT32_MIN)
		return INT32_MIN;
	return (int32_t)x;
}
static inline int16_t clamp_i16_from_i32(int32_t x)
{
	if (x > INT16_MAX)
		return INT16_MAX;
	if (x < INT16_MIN)
		return INT16_MIN;
	return (int16_t)x;
}

/* ---------- public API (same signatures as original) ---------- */

/* computes limit((val >> rshift), 2**bits) */
static inline int32_t signed_saturate_rshift(int32_t val, int bits, int rshift) __attribute__((always_inline, unused));
static inline int32_t signed_saturate_rshift(int32_t val, int bits, int rshift)
{
	/* arithmetic right shift, then saturate to signed 'bits' width */
	int32_t v = arith_rshift32(val, rshift);
	if (bits <= 0)
		return 0;
	if (bits >= 32)
		return v;
	int32_t max = (1u << (bits - 1)) - 1;
	int32_t min = -(1u << (bits - 1));
	if (v > max)
		return max;
	if (v < min)
		return min;
	return v;
}

/* computes limit(val, 2**bits) */
static inline int16_t saturate16(int32_t val) __attribute__((always_inline, unused));
static inline int16_t saturate16(int32_t val)
{
	return clamp_i16_from_i32(val);
}

/* computes ((a[31:0] * b[15:0]) >> 16) */
static inline int32_t signed_multiply_32x16b(int32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int32_t signed_multiply_32x16b(int32_t a, uint32_t b)
{
	int32_t b16 = (int16_t)(b & 0xFFFF);
	int64_t prod = (int64_t)a * (int64_t)b16;
	return (int32_t)(prod >> 16);
}

/* computes ((a[31:0] * b[31:16]) >> 16) */
static inline int32_t signed_multiply_32x16t(int32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int32_t signed_multiply_32x16t(int32_t a, uint32_t b)
{
	int32_t b16 = (int16_t)((b >> 16) & 0xFFFF);
	int64_t prod = (int64_t)a * (int64_t)b16;
	return (int32_t)(prod >> 16);
}

/* computes (((int64_t)a * (int64_t)b) >> 32) */
static inline int32_t multiply_32x32_rshift32(int32_t a, int32_t b) __attribute__((always_inline, unused));
static inline int32_t multiply_32x32_rshift32(int32_t a, int32_t b)
{
	int64_t prod = (int64_t)a * (int64_t)b;
	/* arithmetic right shift of 64-bit signed is implementation-defined
	   but common compilers do arithmetic; use helper to be explicit. */
	return (int32_t)(prod >> 32);
}

/* computes (((int64_t)a * (int64_t)b + 0x8000000) >> 32) */
static inline int32_t multiply_32x32_rshift32_rounded(int32_t a, int32_t b) __attribute__((always_inline, unused));
static inline int32_t multiply_32x32_rshift32_rounded(int32_t a, int32_t b)
{
	int64_t prod = (int64_t)a * (int64_t)b;
	prod += (int64_t)0x08000000; /* rounding constant */
	return (int32_t)(prod >> 32);
}

/* computes sum + (((int64_t)a * (int64_t)b + 0x8000000) >> 32)
   Implemented to wrap modulo 2^32 like the ARM instruction. */
static inline int32_t multiply_accumulate_32x32_rshift32_rounded(int32_t sum, int32_t a, int32_t b) __attribute__((always_inline, unused));
static inline int32_t multiply_accumulate_32x32_rshift32_rounded(int32_t sum, int32_t a, int32_t b)
{
	int64_t prod = (int64_t)a * (int64_t)b;
	int32_t scaled = (int32_t)((prod + (int64_t)0x08000000) >> 32);
	uint32_t u = (uint32_t)sum + (uint32_t)scaled;
	return (int32_t)u;
}

/* computes sum - (((int64_t)a * (int64_t)b + 0x8000000) >> 32)
   Implemented with modulo-2^32 wrap like the ARM instruction. */
static inline int32_t multiply_subtract_32x32_rshift32_rounded(int32_t sum, int32_t a, int32_t b) __attribute__((always_inline, unused));
static inline int32_t multiply_subtract_32x32_rshift32_rounded(int32_t sum, int32_t a, int32_t b)
{
	int64_t prod = (int64_t)a * (int64_t)b;
	int32_t scaled = (int32_t)((prod + (int64_t)0x08000000) >> 32);
	uint32_t u = (uint32_t)sum - (uint32_t)scaled;
	return (int32_t)u;
}

/* computes (a[31:16] | (b[31:16] >> 16)) */
static inline uint32_t pack_16t_16t(int32_t a, int32_t b) __attribute__((always_inline, unused));
static inline uint32_t pack_16t_16t(int32_t a, int32_t b)
{
	return ((uint32_t)a & 0xFFFF0000u) | ((uint32_t)b >> 16);
}

/* computes (a[31:16] | b[15:0]) */
static inline uint32_t pack_16t_16b(int32_t a, int32_t b) __attribute__((always_inline, unused));
static inline uint32_t pack_16t_16b(int32_t a, int32_t b)
{
	return ((uint32_t)a & 0xFFFF0000u) | ((uint32_t)b & 0x0000FFFFu);
}

/* computes ((a[15:0] << 16) | b[15:0]) */
static inline uint32_t pack_16b_16b(int32_t a, int32_t b) __attribute__((always_inline, unused));
static inline uint32_t pack_16b_16b(int32_t a, int32_t b)
{
	return (((uint32_t)a & 0x0000FFFFu) << 16) | ((uint32_t)b & 0x0000FFFFu);
}

/* ---------- 16-bit pairwise saturating / halving / multiply helpers ---------- */

/* qadd16: per-half saturating add, pack back */
static inline uint32_t signed_add_16_and_16(uint32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline uint32_t signed_add_16_and_16(uint32_t a, uint32_t b)
{
	int32_t a_lo = (int16_t)(a & 0xFFFF);
	int32_t a_hi = (int16_t)((a >> 16) & 0xFFFF);
	int32_t b_lo = (int16_t)(b & 0xFFFF);
	int32_t b_hi = (int16_t)((b >> 16) & 0xFFFF);

	int32_t r_lo = clamp_i16_from_i32(a_lo + b_lo);
	int32_t r_hi = clamp_i16_from_i32(a_hi + b_hi);

	return ((uint32_t)(uint16_t)r_hi << 16) | (uint16_t)r_lo;
}

/* qsub16: per-half saturating subtract, pack back */
static inline int32_t signed_subtract_16_and_16(int32_t a, int32_t b) __attribute__((always_inline, unused));
static inline int32_t signed_subtract_16_and_16(int32_t a, int32_t b)
{
	int32_t a_lo = (int16_t)(a & 0xFFFF);
	int32_t a_hi = (int16_t)((a >> 16) & 0xFFFF);
	int32_t b_lo = (int16_t)(b & 0xFFFF);
	int32_t b_hi = (int16_t)((b >> 16) & 0xFFFF);

	int32_t r_lo = clamp_i16_from_i32(a_lo - b_lo);
	int32_t r_hi = clamp_i16_from_i32(a_hi - b_hi);

	return (int32_t)(((uint32_t)(uint16_t)r_hi << 16) | (uint16_t)r_lo);
}

/* shadd16: per-half arithmetic halving add, pack back */
static inline int32_t signed_halving_add_16_and_16(int32_t a, int32_t b) __attribute__((always_inline, unused));
static inline int32_t signed_halving_add_16_and_16(int32_t a, int32_t b)
{
	int32_t a_lo = (int16_t)(a & 0xFFFF);
	int32_t a_hi = (int16_t)((a >> 16) & 0xFFFF);
	int32_t b_lo = (int16_t)(b & 0xFFFF);
	int32_t b_hi = (int16_t)((b >> 16) & 0xFFFF);

	int32_t r_lo = (a_lo + b_lo) >> 1;
	int32_t r_hi = (a_hi + b_hi) >> 1;

	return (int32_t)(((uint32_t)(uint16_t)r_hi << 16) | (uint16_t)r_lo);
}

/* shsub16: per-half arithmetic halving subtract, pack back */
static inline int32_t signed_halving_subtract_16_and_16(int32_t a, int32_t b) __attribute__((always_inline, unused));
static inline int32_t signed_halving_subtract_16_and_16(int32_t a, int32_t b)
{
	int32_t a_lo = (int16_t)(a & 0xFFFF);
	int32_t a_hi = (int16_t)((a >> 16) & 0xFFFF);
	int32_t b_lo = (int16_t)(b & 0xFFFF);
	int32_t b_hi = (int16_t)((b >> 16) & 0xFFFF);

	int32_t r_lo = (a_lo - b_lo) >> 1;
	int32_t r_hi = (a_hi - b_hi) >> 1;

	return (int32_t)(((uint32_t)(uint16_t)r_hi << 16) | (uint16_t)r_lo);
}

/* smlawb: sum + ((a * low16(b)) >> 16), with defined 32-bit wrap like ARM */
static inline int32_t signed_multiply_accumulate_32x16b(int32_t sum, int32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int32_t signed_multiply_accumulate_32x16b(int32_t sum, int32_t a, uint32_t b)
{
	int32_t b_low = (int16_t)(b & 0xFFFF);
	int64_t prod = (int64_t)a * (int64_t)b_low;
	int32_t scaled = (int32_t)(prod >> 16);
	uint32_t u = (uint32_t)sum + (uint32_t)scaled; /* modulo 2^32 wrap like ARM */
	return (int32_t)u;
}

/* smlawt: sum + ((a * high16(b)) >> 16), with defined wrapping */
static inline int32_t signed_multiply_accumulate_32x16t(int32_t sum, int32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int32_t signed_multiply_accumulate_32x16t(int32_t sum, int32_t a, uint32_t b)
{
	int32_t b_top = (int16_t)((b >> 16) & 0xFFFF);
	int64_t prod = (int64_t)a * (int64_t)b_top;
	int32_t scaled = (int32_t)(prod >> 16);
	uint32_t u = (uint32_t)sum + (uint32_t)scaled;
	return (int32_t)u;
}

/* logical and â€” original forced a register AND instruction;
   here semantics are identical. */
static inline uint32_t logical_and(uint32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline uint32_t logical_and(uint32_t a, uint32_t b)
{
	return a & b;
}

/* pairwise 16x16 multiplies and adds (matching smuad / smuadx) */

/* ((a[15:0] * b[15:0]) + (a[31:16] * b[31:16])) */
static inline int32_t multiply_16tx16t_add_16bx16b(uint32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int32_t multiply_16tx16t_add_16bx16b(uint32_t a, uint32_t b)
{
	int32_t a_lo = (int16_t)(a & 0xFFFF);
	int32_t a_hi = (int16_t)((a >> 16) & 0xFFFF);
	int32_t b_lo = (int16_t)(b & 0xFFFF);
	int32_t b_hi = (int16_t)((b >> 16) & 0xFFFF);
	return (int32_t)((int32_t)a_lo * b_lo + (int32_t)a_hi * b_hi);
}

/* ((a[15:0] * b[31:16]) + (a[31:16] * b[15:0])) */
static inline int32_t multiply_16tx16b_add_16bx16t(uint32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int32_t multiply_16tx16b_add_16bx16t(uint32_t a, uint32_t b)
{
	int32_t a_lo = (int16_t)(a & 0xFFFF);
	int32_t a_hi = (int16_t)((a >> 16) & 0xFFFF);
	int32_t b_lo = (int16_t)(b & 0xFFFF);
	int32_t b_hi = (int16_t)((b >> 16) & 0xFFFF);
	return (int32_t)((int32_t)a_lo * b_hi + (int32_t)a_hi * b_lo);
}

/* multiply accumulate into 64-bit accumulator (smlald / smlaldx equivalents) */

/* sum += ((a_lo*b_lo) + (a_hi*b_hi)) */
static inline int64_t multiply_accumulate_16tx16t_add_16bx16b(int64_t sum, uint32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int64_t multiply_accumulate_16tx16t_add_16bx16b(int64_t sum, uint32_t a, uint32_t b)
{
	int64_t a_lo = (int16_t)(a & 0xFFFF);
	int64_t a_hi = (int16_t)((a >> 16) & 0xFFFF);
	int64_t b_lo = (int16_t)(b & 0xFFFF);
	int64_t b_hi = (int16_t)((b >> 16) & 0xFFFF);
	return sum + a_lo * b_lo + a_hi * b_hi;
}

/* sum += ((a_lo*b_hi) + (a_hi*b_lo)) */
static inline int64_t multiply_accumulate_16tx16b_add_16bx16t(int64_t sum, uint32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int64_t multiply_accumulate_16tx16b_add_16bx16t(int64_t sum, uint32_t a, uint32_t b)
{
	int64_t a_lo = (int16_t)(a & 0xFFFF);
	int64_t a_hi = (int16_t)((a >> 16) & 0xFFFF);
	int64_t b_lo = (int16_t)(b & 0xFFFF);
	int64_t b_hi = (int16_t)((b >> 16) & 0xFFFF);
	return sum + a_lo * b_hi + a_hi * b_lo;
}

/* 16x16 single lane multiplies */
static inline int32_t multiply_16bx16b(uint32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int32_t multiply_16bx16b(uint32_t a, uint32_t b)
{
	return (int32_t)((int16_t)(a & 0xFFFF) * (int16_t)(b & 0xFFFF));
}
static inline int32_t multiply_16bx16t(uint32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int32_t multiply_16bx16t(uint32_t a, uint32_t b)
{
	return (int32_t)((int16_t)(a & 0xFFFF) * (int16_t)((b >> 16) & 0xFFFF));
}
static inline int32_t multiply_16tx16b(uint32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int32_t multiply_16tx16b(uint32_t a, uint32_t b)
{
	return (int32_t)((int16_t)((a >> 16) & 0xFFFF) * (int16_t)(b & 0xFFFF));
}
static inline int32_t multiply_16tx16t(uint32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int32_t multiply_16tx16t(uint32_t a, uint32_t b)
{
	return (int32_t)((int16_t)((a >> 16) & 0xFFFF) * (int16_t)((b >> 16) & 0xFFFF));
}

/* computes (a - b), result saturated to 32 bit signed integer range (qsub 32-bit) */
static inline int32_t substract_32_saturate(uint32_t a, uint32_t b) __attribute__((always_inline, unused));
static inline int32_t substract_32_saturate(uint32_t a, uint32_t b)
{
	int64_t da = (int64_t)(int32_t)a;
	int64_t db = (int64_t)(int32_t)b;
	int64_t diff = da - db;
	return clamp_i32_from_i64(diff);
}

/* Multiply two S.31 fractional integers, and return the 32 most significant
 * bits after a shift left by the constant z (0 <= z <= 31).
 * Implemented as (int32_t)(( (int64_t)x * (int64_t)y ) >> (31 - z))
 */
static inline int32_t FRACMUL_SHL(int32_t x, int32_t y, int z) __attribute__((always_inline, unused));
static inline int32_t FRACMUL_SHL(int32_t x, int32_t y, int z)
{
	if (z < 0)
		z = 0;
	if (z > 31)
		z = 31;
	int64_t prod = (int64_t)x * (int64_t)y;
	int shift = 31 - z;
	if (shift >= 0)
		return (int32_t)(prod >> shift);
	else
		return (int32_t)(prod << (-shift));
}

/* get Q from PSR -- no PSR in WASM, return 0 */
static inline uint32_t get_q_psr(void) __attribute__((always_inline, unused));
static inline uint32_t get_q_psr(void)
{
	return 0u;
}

/* clear Q BIT in PSR -- no-op for WASM */
static inline void clr_q_psr(void) __attribute__((always_inline, unused));
static inline void clr_q_psr(void)
{
	/* no-op */
	(void)0;
}

#endif /* DSPINST_WASM_H_ */