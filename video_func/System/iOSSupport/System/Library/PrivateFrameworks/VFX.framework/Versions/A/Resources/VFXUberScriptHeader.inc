// 🤖 This file is assembled from several headers to compile scripts and shaders at runtime

#define VFX_IS_UBER_HEADER

#ifdef __METAL_VERSION__
#include <metal_stdlib>
#include <metal_math>
#include <simd/simd.h>
#else
#include <math.h>
#include <float.h>
#include <simd/simd.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
using namespace simd;
#endif

#ifdef __OBJC__
#include <Foundation/Foundation.h>
@interface NSView
@end
@interface UIView
@end
#endif // __OBJC__

#if !VFX_CHECK_HEADER
#include <VFX/VFX-Script.h>
#endif

#endif // !__METAL_VERSION__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"

// MARK: - vfx_base.h


#define VFX_MATH_DEBUG_CHECK 0
#if !defined(VFX_IS_UBER_HEADER) && !defined(__METAL_VERSION__) && VFX_MATH_DEBUG_CHECK
#define VFX_MATH_ASSERT(e) (assert(e))
#else
#define VFX_MATH_ASSERT(e) ((void)0)
#endif

#ifndef __METAL_VERSION__
#else
using namespace metal;
#endif

#ifdef __cplusplus
#define VFX_EXPORT_C __attribute__((visibility("default"))) extern "C"
#define VFX_EXPORT_C_BEGIN extern "C" {
#define VFX_EXPORT_C_END }
#else
#define VFX_EXPORT_C
#define VFX_EXPORT_C_BEGIN
#define VFX_EXPORT_C_END
#endif

#ifdef DEBUG
#define VFX_ASSERT(a) assert(a)
#else
#define VFX_ASSERT(a)
#endif

#ifndef NS_ASSUME_NONNULL_BEGIN
#define NS_ASSUME_NONNULL_BEGIN _Pragma("clang assume_nonnull begin")
#endif
#ifndef NS_ASSUME_NONNULL_END
#define NS_ASSUME_NONNULL_END   _Pragma("clang assume_nonnull end")
#endif

#ifdef __METAL_VERSION__

#define VFX_EXPORT
#define VFX_OVERLOAD
#define VFX_SIMD
#define VFX_SIMD_UNPURE
#define vfx_out thread
#define vfx_device_out device

#else

#define VFX_EXPORT __attribute__((visibility("default")))
#define VFX_OVERLOAD __attribute__((overloadable))
// This is for pure functions. no side-effect allowed
#define VFX_SIMD VFX_OVERLOAD __attribute__((always_inline, __const__, __nodebug__))
#define VFX_SIMD_UNPURE VFX_OVERLOAD __attribute__((always_inline, __nodebug__))
#define vfx_out
#define vfx_device_out

#endif

// Packed types

typedef struct { float x, y; } vfx_packed_float2;
typedef struct { float x, y, z; } vfx_packed_float3;
typedef struct { float x, y, z, w; } vfx_packed_float4;
typedef struct { uint16_t x, y; } vfx_packed_half2;
typedef struct { uint16_t x, y, z; } vfx_packed_half3;
typedef struct { uint16_t x, y, z, w; } vfx_packed_half4;

// Axis Aligned Bounding Box (need to maintain [center.w = 1.f & half_size.w = 0.f])
typedef struct { simd_float4 center_w1;  simd_float4 half_size_w0; } vfx_aabb;

// use this instead of simd_float2 when you have no parallelism in operations
typedef struct { float lower_bound, upper_bound; } vfx_float_range;
typedef struct { size_t lower_bound, upper_bound; } vfx_int_range;
typedef struct { float scale, bias; } vfx_scale_bias;

// 3D Ray / Segment
typedef struct {
    simd_float4 origin; // w = 1
    simd_float4 direction; // xyz = normalized, w = 0
    simd_float4 invdir_length; // xyz = 1 / dir, w = length, INF for ray, >0 for segment
} vfx_ray;

typedef struct { simd_float3 a, b, c; } vfx_triangle;
// MARK: - vfx_math.h


#define VFX_DEBUG_INTERSECTION 0
#define VFX_INTERSECTION_ERROR_THRESHOLD 0.01

NS_ASSUME_NONNULL_BEGIN

// base math functions

#ifdef __METAL_VERSION__

// constants
#define VFX_PI M_PI_F
#define VFX_1_PI M_1_PI_F
#define VFX_PI_2 M_PI_2_F
#define VFX_2PI (2.f * M_PI_F)
#define VFX_EPSILON FLT_EPSILON
#define VFX_MIN FLT_MIN
#define VFX_INFINITY INFINITY
#define VFX_NAN NAN

// make are just ctors in C++/Metal

static inline simd_float2 VFX_SIMD vfx_make_float2(float x, float y) { return float2(x, y); }

static inline simd_float3 VFX_SIMD vfx_make_float3(float x, float y, float z) { return float3(x, y, z); }
static inline simd_float3 VFX_SIMD vfx_make_float3(simd_float2 xy, float z) { return float3(xy, z); }
static inline simd_float3 VFX_SIMD vfx_make_float3(float x, simd_float2 yz) { return float3(x, yz); }

static inline simd_float4 VFX_SIMD vfx_make_float4(float x, float y, float z, float w) { return float4(x, y, z, w); }
static inline simd_float4 VFX_SIMD vfx_make_float4(simd_float2 xy, float z, float w) { return float4(xy, z, w); }
static inline simd_float4 VFX_SIMD vfx_make_float4(simd_float3 xyz, float w) { return float4(xyz, w); }
static inline simd_float4 VFX_SIMD vfx_make_float4(simd_float3 xyz) { float4 r = 0; r.xyz = xyz; return r; }
static inline simd_float4 VFX_SIMD vfx_make_float4_undef(simd_float3 xyz) { float4 r; r.xyz = xyz; return r; }

#define vfx_bool4 bool4

#define vfx_convert_char4   char4
#define vfx_convert_uchar4  uchar4
#define vfx_convert_short4  short4
#define vfx_convert_ushort4 ushort4
#define vfx_convert_int3    int3
#define vfx_convert_int4    int4
#define vfx_convert_uint4   uint4
#define vfx_convert_float4  float4

#define vfx_any(a) any(a)
#define vfx_all(a) all(a)
#define vfx_select(a, b, c) select(a, b, c)

#define vfx_min(a, b) fmin(a, b)
#define vfx_max(a, b) fmax(a, b)

#define vfx_abs(a) fabs(a)
#define vfx_copysign(a, b) copysign((a), (b))
#define vfx_ceil(a) ceil(a)
#define vfx_floor(a) floor(a)
#define vfx_trunc(a) trunc(a)
#define vfx_sign(a) sign(a)
#define vfx_step(a,b) step(a, b)

#define vfx_fract(a) fract(a)
#define vfx_fmod(a, b) fmod(a, b)
#define vfx_recip(a) (1/(a))
#define vfx_sqrt(a) sqrt(a)
#define vfx_rsqrt(a) rsqrt(a)
#define vfx_precise_rsqrt(a) precise::rsqrt(a)
#define vfx_cbrt(a) pow(a, 1.f/3.f)

#define vfx_cos(a) cos(a)
#define vfx_sin(a) sin(a)
#define vfx_tan(a) tan(a)

#define vfx_acos(a) acos(a)
#define vfx_asin(a) asin(a)
#define vfx_atan(a) atan(a)
#define vfx_atan2(a, b) atan2(a, b)

#define vfx_acosh(a) acosh(a)
#define vfx_asinh(a) asinh(a)
#define vfx_atanh(a) atanh(a)

#define vfx_sinpi(a) sinpi(a)
#define vfx_cospi(a) cospi(a)
#define vfx_tanpi(a) tanpi(a)

#define vfx_exp(a) exp(a)
#define vfx_exp2(a) exp2(a)
#define vfx_exp10(a) exp10(a)
#define vfx_log(a) log(a)
#define vfx_pow(a,b) pow(a, b)

#define vfx_length(a) length(a)
#define vfx_length_squared(a) length_squared(a)
#define vfx_distance(a, b) distance(a, b)
#define vfx_distance_squared(a, b) distance_squared(a, b)
#define vfx_normalize(a) normalize(a)
#define vfx_cross(a, b) cross(a, b)
#define vfx_dot(a, b) dot(a, b)
#define vfx_transpose(a) transpose(a)

#define vfx_muladd fma
#define simd_bitselect select

template<typename T> static inline T VFX_SIMD vfx_reduce_min(vec<T, 2> a) { return fmin(a.x, a.y); }
template<typename T> static inline T VFX_SIMD vfx_reduce_min(vec<T, 3> a) { return fmin3(a.x, a.y, a.z); }
template<typename T> static inline T VFX_SIMD vfx_reduce_min(vec<T, 4> a) { return vfx_reduce_min(min(a.xy, a.zw)); }

template<typename T> static inline T VFX_SIMD vfx_reduce_max(vec<T, 2> a) { return fmax(a.x, a.y); }
template<typename T> static inline T VFX_SIMD vfx_reduce_max(vec<T, 3> a) { return fmax3(a.x, a.y, a.z); }
template<typename T> static inline T VFX_SIMD vfx_reduce_max(vec<T, 4> a) { return vfx_reduce_max(max(a.xy, a.zw)); }

template<typename T> static inline T VFX_SIMD vfx_reduce_add(vec<T, 2> a) { return a.x + a.y; }
template<typename T> static inline T VFX_SIMD vfx_reduce_add(vec<T, 3> a) { return a.x + a.y + a.z; }
template<typename T> static inline T VFX_SIMD vfx_reduce_add(vec<T, 4> a) { return a.x + a.y + a.z + a.w; }

#else

// constants
#define VFX_PI (float)(M_PI)
#define VFX_1_PI (float)(M_1_PI)
#define VFX_PI_2 (float)(M_PI_2)
#define VFX_2PI (2.f * (float)(M_PI))
#define VFX_EPSILON FLT_EPSILON // 1.19209290E-07F
#define VFX_MIN 1.17549435e-38f // FLT_MIN
#define VFX_INFINITY INFINITY
#define VFX_NAN NAN

// Macros are not exported correctly in swift so we have to do static inline wrapping functions

static inline simd_float2 VFX_SIMD vfx_make_float2(float x, float y) { return simd_make_float2(x, y); }

static inline simd_float3 VFX_SIMD vfx_make_float3(float x, float y, float z) { return simd_make_float3(x, y, z); }
static inline simd_float3 VFX_SIMD vfx_make_float3(simd_float2 xy, float z) { return simd_make_float3(xy, z); }
static inline simd_float3 VFX_SIMD vfx_make_float3(float x, simd_float2 yz) { return simd_make_float3(x, yz); }

static inline simd_float4 VFX_SIMD vfx_make_float4(float x, float y, float z, float w) { return simd_make_float4(x, y, z, w); }
static inline simd_float4 VFX_SIMD vfx_make_float4(simd_float3 xyz, float w) { return simd_make_float4(xyz, w); }
static inline simd_float4 VFX_SIMD vfx_make_float4(simd_float3 xyz) { return simd_make_float4(xyz); }
static inline simd_float4 VFX_SIMD vfx_make_float4(simd_float2 xy, float z, float w) { return simd_make_float4(xy, z, w); }
static inline simd_float4 VFX_SIMD vfx_make_float4_undef(simd_float3 xyz) { return simd_make_float4_undef(xyz); }

#define vfx_bool4 simd_int4

#define vfx_convert_char4   simd_char
#define vfx_convert_uchar4  simd_uchar
#define vfx_convert_short4  simd_short
#define vfx_convert_ushort4 simd_ushort
#define vfx_convert_int3    simd_int
#define vfx_convert_int4    simd_int
#define vfx_convert_uint4   simd_uint
#define vfx_convert_float4  simd_float

#define vfx_any(a) simd_any(a)
#define vfx_all(a) simd_all(a)

static inline float VFX_SIMD vfx_select(float a, float b, int c) { return (c < 0) ? b : a; }
static inline simd_float2 VFX_SIMD vfx_select(simd_float2 a, simd_float2 b, simd_int2 c) { return simd_select(a, b, c); }
static inline simd_float3 VFX_SIMD vfx_select(simd_float3 a, simd_float3 b, simd_int3 c) { return simd_select(a, b, c); }
static inline simd_float4 VFX_SIMD vfx_select(simd_float4 a, simd_float4 b, simd_int4 c) { return simd_select(a, b, c); }

static inline int32_t     VFX_SIMD vfx_min(int32_t a, int32_t b)         { return a < b ? a : b; }
static inline float       VFX_SIMD vfx_min(float a, float b)             { return a < b ? a : b; }
static inline simd_float2 VFX_SIMD vfx_min(simd_float2 a, simd_float2 b) { return __tg_fmin(a, b); }
static inline simd_float3 VFX_SIMD vfx_min(simd_float3 a, simd_float3 b) { return __tg_fmin(a, b); }
static inline simd_float4 VFX_SIMD vfx_min(simd_float4 a, simd_float4 b) { return __tg_fmin(a, b); }

static inline int32_t     VFX_SIMD vfx_max(int32_t a, int32_t b)         { return a > b ? a : b; }
static inline float       VFX_SIMD vfx_max(float a, float b)             { return a > b ? a : b; }
static inline simd_float2 VFX_SIMD vfx_max(simd_float2 a, simd_float2 b) { return __tg_fmax(a, b); }
static inline simd_float3 VFX_SIMD vfx_max(simd_float3 a, simd_float3 b) { return __tg_fmax(a, b); }
static inline simd_float4 VFX_SIMD vfx_max(simd_float4 a, simd_float4 b) { return __tg_fmax(a, b); }

static inline float       VFX_SIMD vfx_abs(float a)       { return __tg_fabs(a); }
static inline simd_float2 VFX_SIMD vfx_abs(simd_float2 a) { return __tg_fabs(a); }
static inline simd_float3 VFX_SIMD vfx_abs(simd_float3 a) { return __tg_fabs(a); }
static inline simd_float4 VFX_SIMD vfx_abs(simd_float4 a) { return __tg_fabs(a); }

static inline float       VFX_SIMD vfx_copysign(float a, float b)             { return __tg_copysign(a, b); }
static inline simd_float2 VFX_SIMD vfx_copysign(simd_float2 a, simd_float2 b) { return __tg_copysign(a, b); }
static inline simd_float3 VFX_SIMD vfx_copysign(simd_float3 a, simd_float3 b) { return __tg_copysign(a, b); }
static inline simd_float4 VFX_SIMD vfx_copysign(simd_float4 a, simd_float4 b) { return __tg_copysign(a, b); }

static inline float       VFX_SIMD vfx_ceil(float a)       { return __tg_ceil(a); }
static inline simd_float2 VFX_SIMD vfx_ceil(simd_float2 a) { return __tg_ceil(a); }
static inline simd_float3 VFX_SIMD vfx_ceil(simd_float3 a) { return __tg_ceil(a); }
static inline simd_float4 VFX_SIMD vfx_ceil(simd_float4 a) { return __tg_ceil(a); }

static inline float       VFX_SIMD vfx_floor(float a)       { return __tg_floor(a); }
static inline simd_float2 VFX_SIMD vfx_floor(simd_float2 a) { return __tg_floor(a); }
static inline simd_float3 VFX_SIMD vfx_floor(simd_float3 a) { return __tg_floor(a); }
static inline simd_float4 VFX_SIMD vfx_floor(simd_float4 a) { return __tg_floor(a); }

static inline float       VFX_SIMD vfx_trunc(float a)       { return __tg_trunc(a); }
static inline simd_float2 VFX_SIMD vfx_trunc(simd_float2 a) { return __tg_trunc(a); }
static inline simd_float3 VFX_SIMD vfx_trunc(simd_float3 a) { return __tg_trunc(a); }
static inline simd_float4 VFX_SIMD vfx_trunc(simd_float4 a) { return __tg_trunc(a); }

static inline float VFX_SIMD vfx_sign(float a) { return simd_sign(a); }
static inline simd_float2 VFX_SIMD vfx_sign(simd_float2 a) { return simd_sign(a); }
static inline simd_float3 VFX_SIMD vfx_sign(simd_float3 a) { return simd_sign(a); }
static inline simd_float4 VFX_SIMD vfx_sign(simd_float4 a) { return simd_sign(a); }

static inline float VFX_SIMD vfx_step(float a, float b) { return a >= b; }
static inline simd_float2 VFX_SIMD vfx_step(simd_float2 a, simd_float2 b) { return simd_step(a, b); }
static inline simd_float3 VFX_SIMD vfx_step(simd_float3 a, simd_float3 b) { return simd_step(a, b); }
static inline simd_float4 VFX_SIMD vfx_step(simd_float4 a, simd_float4 b) { return simd_step(a, b); }

static inline float       VFX_SIMD vfx_fract(float a)       { return simd_fract(a); }
static inline simd_float2 VFX_SIMD vfx_fract(simd_float2 a) { return simd_fract(a); }
static inline simd_float3 VFX_SIMD vfx_fract(simd_float3 a) { return simd_fract(a); }
static inline simd_float4 VFX_SIMD vfx_fract(simd_float4 a) { return simd_fract(a); }

static inline float       VFX_SIMD vfx_fmod(float a, float b)             { return __tg_fmod(a, b); }
static inline simd_float2 VFX_SIMD vfx_fmod(simd_float2 a, simd_float2 b) { return __tg_fmod(a, b); }
static inline simd_float3 VFX_SIMD vfx_fmod(simd_float3 a, simd_float3 b) { return __tg_fmod(a, b); }
static inline simd_float4 VFX_SIMD vfx_fmod(simd_float4 a, simd_float4 b) { return __tg_fmod(a, b); }

static inline float       VFX_SIMD vfx_recip(float a)       { return simd_recip(a); }
static inline simd_float2 VFX_SIMD vfx_recip(simd_float2 a) { return simd_recip(a); }
static inline simd_float3 VFX_SIMD vfx_recip(simd_float3 a) { return simd_recip(a); }
static inline simd_float4 VFX_SIMD vfx_recip(simd_float4 a) { return simd_recip(a); }

static inline float       VFX_SIMD vfx_sqrt(float a)       { return __tg_sqrt(a); }
static inline simd_float2 VFX_SIMD vfx_sqrt(simd_float2 a) { return __tg_sqrt(a); }
static inline simd_float3 VFX_SIMD vfx_sqrt(simd_float3 a) { return __tg_sqrt(a); }
static inline simd_float4 VFX_SIMD vfx_sqrt(simd_float4 a) { return __tg_sqrt(a); }

static inline float       VFX_SIMD vfx_rsqrt(float a)       { return simd_rsqrt(a); }
static inline simd_float2 VFX_SIMD vfx_rsqrt(simd_float2 a) { return simd_rsqrt(a); }
static inline simd_float3 VFX_SIMD vfx_rsqrt(simd_float3 a) { return simd_rsqrt(a); }
static inline simd_float4 VFX_SIMD vfx_rsqrt(simd_float4 a) { return simd_rsqrt(a); }

static inline float       VFX_SIMD vfx_precise_rsqrt(float a)       { return simd_precise_rsqrt(a); }
static inline simd_float2 VFX_SIMD vfx_precise_rsqrt(simd_float2 a) { return simd_precise_rsqrt(a); }
static inline simd_float3 VFX_SIMD vfx_precise_rsqrt(simd_float3 a) { return simd_precise_rsqrt(a); }
static inline simd_float4 VFX_SIMD vfx_precise_rsqrt(simd_float4 a) { return simd_precise_rsqrt(a); }

static inline float       VFX_SIMD vfx_cbrt(float a)       { return __tg_cbrt(a); }
static inline simd_float2 VFX_SIMD vfx_cbrt(simd_float2 a) { return __tg_cbrt(a); }
static inline simd_float3 VFX_SIMD vfx_cbrt(simd_float3 a) { return __tg_cbrt(a); }
static inline simd_float4 VFX_SIMD vfx_cbrt(simd_float4 a) { return __tg_cbrt(a); }

static inline float       VFX_SIMD vfx_cos(float a)       { return __tg_cos(a); }
static inline simd_float2 VFX_SIMD vfx_cos(simd_float2 a) { return __tg_cos(a); }
static inline simd_float3 VFX_SIMD vfx_cos(simd_float3 a) { return __tg_cos(a); }
static inline simd_float4 VFX_SIMD vfx_cos(simd_float4 a) { return __tg_cos(a); }

static inline float       VFX_SIMD vfx_sin(float a)       { return __tg_sin(a); }
static inline simd_float2 VFX_SIMD vfx_sin(simd_float2 a) { return __tg_sin(a); }
static inline simd_float3 VFX_SIMD vfx_sin(simd_float3 a) { return __tg_sin(a); }
static inline simd_float4 VFX_SIMD vfx_sin(simd_float4 a) { return __tg_sin(a); }

static inline float       VFX_SIMD vfx_tan(float a)       { return __tg_tan(a); }
static inline simd_float2 VFX_SIMD vfx_tan(simd_float2 a) { return __tg_tan(a); }
static inline simd_float3 VFX_SIMD vfx_tan(simd_float3 a) { return __tg_tan(a); }
static inline simd_float4 VFX_SIMD vfx_tan(simd_float4 a) { return __tg_tan(a); }

static inline float       VFX_SIMD vfx_acos(float a)       { return __tg_acos(a); }
static inline simd_float2 VFX_SIMD vfx_acos(simd_float2 a) { return __tg_acos(a); }
static inline simd_float3 VFX_SIMD vfx_acos(simd_float3 a) { return __tg_acos(a); }
static inline simd_float4 VFX_SIMD vfx_acos(simd_float4 a) { return __tg_acos(a); }

static inline float       VFX_SIMD vfx_asin(float a)       { return __tg_asin(a); }
static inline simd_float2 VFX_SIMD vfx_asin(simd_float2 a) { return __tg_asin(a); }
static inline simd_float3 VFX_SIMD vfx_asin(simd_float3 a) { return __tg_asin(a); }
static inline simd_float4 VFX_SIMD vfx_asin(simd_float4 a) { return __tg_asin(a); }

static inline float       VFX_SIMD vfx_atan(float a)       { return __tg_atan(a); }
static inline simd_float2 VFX_SIMD vfx_atan(simd_float2 a) { return __tg_atan(a); }
static inline simd_float3 VFX_SIMD vfx_atan(simd_float3 a) { return __tg_atan(a); }
static inline simd_float4 VFX_SIMD vfx_atan(simd_float4 a) { return __tg_atan(a); }

static inline float       VFX_SIMD vfx_atan2(float a, float b)             { return __tg_atan2(a, b); }
static inline simd_float2 VFX_SIMD vfx_atan2(simd_float2 a, simd_float2 b) { return __tg_atan2(a, b); }
static inline simd_float3 VFX_SIMD vfx_atan2(simd_float3 a, simd_float3 b) { return __tg_atan2(a, b); }
static inline simd_float4 VFX_SIMD vfx_atan2(simd_float4 a, simd_float4 b) { return __tg_atan2(a, b); }

static inline float       VFX_SIMD vfx_acosh(float a)       { return __tg_acosh(a); }
static inline simd_float2 VFX_SIMD vfx_acosh(simd_float2 a) { return __tg_acosh(a); }
static inline simd_float3 VFX_SIMD vfx_acosh(simd_float3 a) { return __tg_acosh(a); }
static inline simd_float4 VFX_SIMD vfx_acosh(simd_float4 a) { return __tg_acosh(a); }

static inline float       VFX_SIMD vfx_asinh(float a)       { return __tg_asinh(a); }
static inline simd_float2 VFX_SIMD vfx_asinh(simd_float2 a) { return __tg_asinh(a); }
static inline simd_float3 VFX_SIMD vfx_asinh(simd_float3 a) { return __tg_asinh(a); }
static inline simd_float4 VFX_SIMD vfx_asinh(simd_float4 a) { return __tg_asinh(a); }

static inline float       VFX_SIMD vfx_atanh(float a)       { return __tg_atanh(a); }
static inline simd_float2 VFX_SIMD vfx_atanh(simd_float2 a) { return __tg_atanh(a); }
static inline simd_float3 VFX_SIMD vfx_atanh(simd_float3 a) { return __tg_atanh(a); }
static inline simd_float4 VFX_SIMD vfx_atanh(simd_float4 a) { return __tg_atanh(a); }

static inline float VFX_SIMD vfx_sinpi(float a) { return __sinpif(a); }
static inline float VFX_SIMD vfx_cospi(float a) { return __cospif(a); }
static inline float VFX_SIMD vfx_tanpi(float a) { return __tanpif(a); }

static inline float       VFX_SIMD vfx_exp(float a)       { return __tg_exp(a); }
static inline simd_float2 VFX_SIMD vfx_exp(simd_float2 a) { return __tg_exp(a); }
static inline simd_float3 VFX_SIMD vfx_exp(simd_float3 a) { return __tg_exp(a); }
static inline simd_float4 VFX_SIMD vfx_exp(simd_float4 a) { return __tg_exp(a); }

static inline float       VFX_SIMD vfx_exp2(float a)       { return __tg_exp2(a); }
static inline simd_float2 VFX_SIMD vfx_exp2(simd_float2 a) { return __tg_exp2(a); }
static inline simd_float3 VFX_SIMD vfx_exp2(simd_float3 a) { return __tg_exp2(a); }
static inline simd_float4 VFX_SIMD vfx_exp2(simd_float4 a) { return __tg_exp2(a); }

static inline float       VFX_SIMD vfx_exp10(float a)       { return __tg_exp10(a); }
static inline simd_float2 VFX_SIMD vfx_exp10(simd_float2 a) { return __tg_exp10(a); }
static inline simd_float3 VFX_SIMD vfx_exp10(simd_float3 a) { return __tg_exp10(a); }
static inline simd_float4 VFX_SIMD vfx_exp10(simd_float4 a) { return __tg_exp10(a); }

static inline float       VFX_SIMD vfx_log(float a)       { return __tg_log(a); }
static inline simd_float2 VFX_SIMD vfx_log(simd_float2 a) { return __tg_log(a); }
static inline simd_float3 VFX_SIMD vfx_log(simd_float3 a) { return __tg_log(a); }
static inline simd_float4 VFX_SIMD vfx_log(simd_float4 a) { return __tg_log(a); }

static inline float       VFX_SIMD vfx_pow(float a, float b)             { return __tg_pow(a, b); }
static inline simd_float2 VFX_SIMD vfx_pow(simd_float2 a, simd_float2 b) { return __tg_pow(a, b); }
static inline simd_float3 VFX_SIMD vfx_pow(simd_float3 a, simd_float3 b) { return __tg_pow(a, b); }
static inline simd_float4 VFX_SIMD vfx_pow(simd_float4 a, simd_float4 b) { return __tg_pow(a, b); }

#define vfx_length(a) simd_length(a)
#define vfx_length_squared(a) simd_length_squared(a)
#define vfx_distance(a, b) simd_distance(a, b)
#define vfx_distance_squared(a, b) simd_distance_squared(a, b)
#define vfx_normalize(a) simd_normalize(a)
#define vfx_cross(a, b) simd_cross(a, b)
#define vfx_dot(a, b) simd_dot(a, b)

static inline simd_float2x2 VFX_SIMD vfx_transpose(simd_float2x2 m) { return simd_transpose(m); }
static inline simd_float3x3 VFX_SIMD vfx_transpose(simd_float3x3 m) { return simd_transpose(m); }
static inline simd_float4x4 VFX_SIMD vfx_transpose(simd_float4x4 m) { return simd_transpose(m); }

// Not yet available in metal :/
static inline simd_float2x2 VFX_SIMD vfx_inverse(simd_float2x2 m) { return simd_inverse(m); }
static inline simd_float3x3 VFX_SIMD vfx_inverse(simd_float3x3 m) { return simd_inverse(m); }
static inline simd_float4x4 VFX_SIMD vfx_inverse(simd_float4x4 m) { return simd_inverse(m); }

static inline simd_float4 VFX_SIMD vfx_muladd(simd_float4 a, simd_float4 b, simd_float4 c) { return simd_muladd(a, b, c); }
static inline simd_float3 VFX_SIMD vfx_muladd(simd_float3 a, simd_float3 b, simd_float3 c) { return simd_muladd(a, b, c); }
static inline simd_float2 VFX_SIMD vfx_muladd(simd_float2 a, simd_float2 b, simd_float2 c) { return simd_muladd(a, b, c); }
static inline float VFX_SIMD vfx_muladd(float a, float b, float c) { return simd_muladd(a, b, c); }

static inline float VFX_SIMD vfx_reduce_min(simd_float2 a) { return simd_reduce_min(a); }
static inline float VFX_SIMD vfx_reduce_min(simd_float3 a) { return simd_reduce_min(a); }
static inline float VFX_SIMD vfx_reduce_min(simd_float4 a) { return simd_reduce_min(a); }

static inline float VFX_SIMD vfx_reduce_max(simd_float2 a) { return simd_reduce_max(a); }
static inline float VFX_SIMD vfx_reduce_max(simd_float3 a) { return simd_reduce_max(a); }
static inline float VFX_SIMD vfx_reduce_max(simd_float4 a) { return simd_reduce_max(a); }

static inline float VFX_SIMD vfx_reduce_add(simd_float2 a) { return simd_reduce_add(a); }
static inline float VFX_SIMD vfx_reduce_add(simd_float3 a) { return simd_reduce_add(a); }
static inline float VFX_SIMD vfx_reduce_add(simd_float4 a) { return simd_reduce_add(a); }

#endif

static inline float       VFX_SIMD vfx_pow2(float a)       { return a * a; }
static inline simd_float2 VFX_SIMD vfx_pow2(simd_float2 a) { return a * a; }
static inline simd_float3 VFX_SIMD vfx_pow2(simd_float3 a) { return a * a; }
static inline simd_float4 VFX_SIMD vfx_pow2(simd_float4 a) { return a * a; }

static inline float       VFX_SIMD vfx_pow3(float a)       { return a * a * a; }
static inline simd_float2 VFX_SIMD vfx_pow3(simd_float2 a) { return a * a * a; }
static inline simd_float3 VFX_SIMD vfx_pow3(simd_float3 a) { return a * a * a; }
static inline simd_float4 VFX_SIMD vfx_pow3(simd_float4 a) { return a * a * a; }

static inline float       VFX_SIMD vfx_pow4(float a)       { return a * a * a * a; }
static inline simd_float2 VFX_SIMD vfx_pow4(simd_float2 a) { return a * a * a * a; }
static inline simd_float3 VFX_SIMD vfx_pow4(simd_float3 a) { return a * a * a * a; }
static inline simd_float4 VFX_SIMD vfx_pow4(simd_float4 a) { return a * a * a * a; }

static inline float       VFX_SIMD vfx_pow5(float a)       { return a * a * a * a * a; }
static inline simd_float2 VFX_SIMD vfx_pow5(simd_float2 a) { return a * a * a * a * a; }
static inline simd_float3 VFX_SIMD vfx_pow5(simd_float3 a) { return a * a * a * a * a; }
static inline simd_float4 VFX_SIMD vfx_pow5(simd_float4 a) { return a * a * a * a * a; }

static inline int VFX_SIMD vfx_max_index(simd_float2 a) { return a.x >= a.y ? 0 : 1; }
static inline int VFX_SIMD vfx_max_index(simd_float3 a) { return a.x >= a.y ? (a.x >= a.z ? 0 : 2) : (a.y >= a.z ? 1 : 2); }

static inline int VFX_SIMD vfx_min_index(simd_float2 a) { return a.x <= a.y ? 0 : 1; }
static inline int VFX_SIMD vfx_min_index(simd_float3 a) { return a.x <= a.y ? (a.x <= a.z ? 0 : 2) : (a.y <= a.z ? 1 : 2); }

static inline simd_float2 VFX_SIMD vfx_splat2(float x) { return vfx_make_float2(x, x); }
static inline simd_float3 VFX_SIMD vfx_splat3(float x) { return vfx_make_float3(x, x, x); }
static inline simd_float4 VFX_SIMD vfx_splat4(float x) { return vfx_make_float4(x, x, x, x); }

#ifdef __METAL_VERSION__

static inline simd_float2x2 VFX_SIMD vfx_diagonal(simd_float2 d) { return float2x2(float2(d.x, 0),
                                                                                   float2(0, d.y)); }
static inline simd_float3x3 VFX_SIMD vfx_diagonal(simd_float3 d) { return float3x3(float3(d.x, 0, 0),
                                                                                   float3(0, d.y, 0),
                                                                                   float3(0, 0, d.z)); }
static inline simd_float4x4 VFX_SIMD vfx_diagonal(simd_float4 d) { return float4x4(float4(d.x, 0, 0, 0),
                                                                                   float4(0, d.y, 0, 0),
                                                                                   float4(0, 0, d.z, 0),
                                                                                   float4(0, 0, 0, d.w)); }
static inline simd_float3x3 VFX_SIMD vfx_float3x3(simd_float4x4 m) { return float3x3(m[0].xyz, m[1].xyz, m[2].xyz); }

static inline simd_float2x2 VFX_SIMD vfx_float2x2(simd_float2 c0, simd_float2 c1) { return float2x2(c0, c1); }
static inline simd_float2x3 VFX_SIMD vfx_float2x3(simd_float3 c0, simd_float3 c1) { return float2x3(c0, c1); }
static inline simd_float2x4 VFX_SIMD vfx_float2x4(simd_float4 c0, simd_float4 c1) { return float2x4(c0, c1); }
static inline simd_float3x2 VFX_SIMD vfx_float3x2(simd_float2 c0, simd_float2 c1, simd_float2 c2) { return float3x2(c0, c1, c2); }
static inline simd_float3x3 VFX_SIMD vfx_float3x3(simd_float3 c0, simd_float3 c1, simd_float3 c2) { return float3x3(c0, c1, c2); }
static inline simd_float3x4 VFX_SIMD vfx_float3x4(simd_float4 c0, simd_float4 c1, simd_float4 c2) { return float3x4(c0, c1, c2); }
static inline simd_float4x2 VFX_SIMD vfx_float4x2(simd_float2 c0, simd_float2 c1, simd_float2 c2, simd_float2 c3) { return float4x2(c0, c1, c2, c3); }
static inline simd_float4x3 VFX_SIMD vfx_float4x3(simd_float3 c0, simd_float3 c1, simd_float3 c2, simd_float3 c3) { return float4x3(c0, c1, c2, c3); }
static inline simd_float4x4 VFX_SIMD vfx_float4x4(simd_float4 c0, simd_float4 c1, simd_float4 c2, simd_float4 c3) { return float4x4(c0, c1, c2, c3); }

static inline simd_float3x2 VFX_SIMD vfx_float3x2_from_rows(simd_float3 c0, simd_float3 c1) { return transpose(float2x3(c0, c1)); }
static inline simd_float3x3 VFX_SIMD vfx_float3x3_from_rows(simd_float3 c0, simd_float3 c1, simd_float3 c2) { return transpose(float3x3(c0, c1, c2)); }
static inline simd_float3x4 VFX_SIMD vfx_float3x4_from_rows(simd_float3 c0, simd_float3 c1, simd_float3 c2, simd_float3 c3) { return transpose(float4x3(c0, c1, c2, c3)); }

static inline simd_float4x2 VFX_SIMD vfx_float4x2_from_rows(simd_float4 c0, simd_float4 c1) { return transpose(float2x4(c0, c1)); }
static inline simd_float4x3 VFX_SIMD vfx_float4x3_from_rows(simd_float4 c0, simd_float4 c1, simd_float4 c2) { return transpose(float3x4(c0, c1, c2)); }
static inline simd_float4x4 VFX_SIMD vfx_float4x4_from_rows(simd_float4 c0, simd_float4 c1, simd_float4 c2, simd_float4 c3) { return transpose(float4x4(c0, c1, c2, c3)); }
#else
static inline simd_float2x2 VFX_SIMD vfx_diagonal(simd_float2 d) { return simd_diagonal_matrix(d); }
static inline simd_float3x3 VFX_SIMD vfx_diagonal(simd_float3 d) { return simd_diagonal_matrix(d); }
static inline simd_float4x4 VFX_SIMD vfx_diagonal(simd_float4 d) { return simd_diagonal_matrix(d); }
static inline simd_float3x3 VFX_SIMD vfx_float3x3(simd_float4x4 m) { return simd_matrix(m.columns[0].xyz, m.columns[1].xyz, m.columns[2].xyz); }

static inline simd_float2x2 VFX_SIMD vfx_float2x2(simd_float2 c0, simd_float2 c1) { return simd_matrix(c0, c1); }
static inline simd_float2x3 VFX_SIMD vfx_float2x3(simd_float3 c0, simd_float3 c1) { return simd_matrix(c0, c1); }
static inline simd_float2x4 VFX_SIMD vfx_float2x4(simd_float4 c0, simd_float4 c1) { return simd_matrix(c0, c1); }
static inline simd_float3x2 VFX_SIMD vfx_float3x2(simd_float2 c0, simd_float2 c1, simd_float2 c2) { return simd_matrix(c0, c1, c2); }
static inline simd_float3x3 VFX_SIMD vfx_float3x3(simd_float3 c0, simd_float3 c1, simd_float3 c2) { return simd_matrix(c0, c1, c2); }
static inline simd_float3x4 VFX_SIMD vfx_float3x4(simd_float4 c0, simd_float4 c1, simd_float4 c2) { return simd_matrix(c0, c1, c2); }
static inline simd_float4x2 VFX_SIMD vfx_float4x2(simd_float2 c0, simd_float2 c1, simd_float2 c2, simd_float2 c3) { return simd_matrix(c0, c1, c2, c3); }
static inline simd_float4x3 VFX_SIMD vfx_float4x3(simd_float3 c0, simd_float3 c1, simd_float3 c2, simd_float3 c3) { return simd_matrix(c0, c1, c2, c3); }
static inline simd_float4x4 VFX_SIMD vfx_float4x4(simd_float4 c0, simd_float4 c1, simd_float4 c2, simd_float4 c3) { return simd_matrix(c0, c1, c2, c3); }

static inline simd_float3x2 VFX_SIMD vfx_float3x2_from_rows(simd_float3 c0, simd_float3 c1) { return simd_matrix_from_rows(c0, c1); }
static inline simd_float3x3 VFX_SIMD vfx_float3x3_from_rows(simd_float3 c0, simd_float3 c1, simd_float3 c2) { return simd_matrix_from_rows(c0, c1, c2); }
static inline simd_float3x4 VFX_SIMD vfx_float3x4_from_rows(simd_float3 c0, simd_float3 c1, simd_float3 c2, simd_float3 c3) { return simd_matrix_from_rows(c0, c1, c2, c3); }

static inline simd_float4x2 VFX_SIMD vfx_float4x2_from_rows(simd_float4 c0, simd_float4 c1) { return simd_matrix_from_rows(c0, c1); }
static inline simd_float4x3 VFX_SIMD vfx_float4x3_from_rows(simd_float4 c0, simd_float4 c1, simd_float4 c2) { return simd_matrix_from_rows(c0, c1, c2); }
static inline simd_float4x4 VFX_SIMD vfx_float4x4_from_rows(simd_float4 c0, simd_float4 c1, simd_float4 c2, simd_float4 c3) { return simd_matrix_from_rows(c0, c1, c2, c3); }
#endif

static inline simd_float2 VFX_SIMD vfx_float2_zero(void) { return (simd_float2){ 0, 0 }; }
static inline simd_float2 VFX_SIMD vfx_float2_half_one(void) { return (simd_float2){ 0.5, 0.5 }; }
static inline simd_float2 VFX_SIMD vfx_float2_one(void) { return (simd_float2){ 1, 1 }; }
static inline simd_float2 VFX_SIMD vfx_float2_two(void) { return (simd_float2){ 2, 2 }; }

static inline simd_float3 VFX_SIMD vfx_float3_zero(void) { return (simd_float3){ 0, 0, 0 }; }
static inline simd_float3 VFX_SIMD vfx_float3_half_one(void) { return (simd_float3){ 0.5, 0.5, 0.5 }; }
static inline simd_float3 VFX_SIMD vfx_float3_one(void) { return (simd_float3){ 1, 1, 1 }; }
static inline simd_float3 VFX_SIMD vfx_float3_two(void) { return (simd_float3){ 2, 2, 2 }; }
static inline simd_float3 VFX_SIMD vfx_float3_unit_x(void) { return (simd_float3){ 1, 0, 0 }; }
static inline simd_float3 VFX_SIMD vfx_float3_unit_y(void) { return (simd_float3){ 0, 1, 0 }; }
static inline simd_float3 VFX_SIMD vfx_float3_unit_z(void) { return (simd_float3){ 0, 0, 1 }; }

static inline simd_float4 VFX_SIMD vfx_float4_zero(void) { return (simd_float4){ 0, 0, 0, 0 }; }
static inline simd_float4 VFX_SIMD vfx_float4_half_one(void) { return (simd_float4){ 0.5, 0.5, 0.5, 0.5 }; }
static inline simd_float4 VFX_SIMD vfx_float4_one(void) { return (simd_float4){ 1, 1, 1, 1 }; }
static inline simd_float4 VFX_SIMD vfx_float4_two(void) { return (simd_float4){ 2, 2, 2, 2 }; }
static inline simd_float4 VFX_SIMD vfx_float4_unit_x(void) { return (simd_float4){ 1, 0, 0, 0 }; }
static inline simd_float4 VFX_SIMD vfx_float4_unit_y(void) { return (simd_float4){ 0, 1, 0, 0 }; }
static inline simd_float4 VFX_SIMD vfx_float4_unit_z(void) { return (simd_float4){ 0, 0, 1, 0 }; }
static inline simd_float4 VFX_SIMD vfx_float4_unit_w(void) { return (simd_float4){ 0, 0, 0, 1 }; }

static inline vfx_packed_half4 VFX_SIMD vfx_packed_half4_one(void) { return (vfx_packed_half4){ 0x3c00, 0x3c00, 0x3c00, 0x3c00 }; }

static inline simd_float4x4 VFX_SIMD vfx_float4x4(simd_float3x3 m) { return vfx_float4x4(vfx_make_float4(m.columns[0].xyz, 0), vfx_make_float4(m.columns[1].xyz, 0), vfx_make_float4(m.columns[2].xyz, 0), vfx_float4_unit_w()); }

static inline bool VFX_SIMD vfx_is_nan(float a) { return isnan(a); }
static inline bool VFX_SIMD vfx_contains_nan(simd_float2 a) { return vfx_any(a == vfx_splat2(NAN)); }
static inline bool VFX_SIMD vfx_contains_nan(simd_float3 a) { return vfx_any(a == vfx_splat3(NAN)); }
static inline bool VFX_SIMD vfx_contains_nan(simd_float4 a) { return vfx_any(a == vfx_splat4(NAN)); }

// this is faster but breaks when feed with NaN
// return !simd_any((coord < 0.f) | (coord > 1.f));
static inline bool VFX_SIMD vfx_is_in_01(simd_float2 p) { return vfx_all(vfx_abs(p - 0.5f) <= 0.5f); }
static inline bool VFX_SIMD vfx_is_in_01(simd_float3 p) { return vfx_all(vfx_abs(p - 0.5f) <= 0.5f); }
static inline bool VFX_SIMD vfx_is_in_01(simd_float4 p) { return vfx_all(vfx_abs(p - 0.5f) <= 0.5f); }

static inline bool VFX_SIMD vfx_is_outside_01(simd_float2 p) { return vfx_any((p < 0.f) | (p > 1.f)); }
static inline bool VFX_SIMD vfx_is_outside_01(simd_float3 p) { return vfx_any((p < 0.f) | (p > 1.f)); }
static inline bool VFX_SIMD vfx_is_outside_01(simd_float4 p) { return vfx_any((p < 0.f) | (p > 1.f)); }

// a * b + c
static inline simd_float4 VFX_SIMD vfx_muladd(simd_float4 a, float b, simd_float4 c) { return vfx_muladd(a, (simd_float4)(b), c); }
static inline simd_float3 VFX_SIMD vfx_muladd(simd_float3 a, float b, simd_float3 c) { return vfx_muladd(a, (simd_float3)(b), c); }
static inline simd_float2 VFX_SIMD vfx_muladd(simd_float2 a, float b, simd_float2 c) { return vfx_muladd(a, (simd_float2)(b), c); }

static inline float VFX_SIMD vfx_get_x(simd_float2 a) { return a.x; }
static inline simd_float2 VFX_SIMD vfx_set_x(simd_float2 a, float x) { a.x = x; return a; }
static inline float VFX_SIMD vfx_get_y(simd_float2 a) { return a.y; }
static inline simd_float2 VFX_SIMD vfx_set_y(simd_float2 a, float y) { a.y = y; return a; }

static inline float VFX_SIMD vfx_get_x(simd_float3 a) { return a.x; }
static inline simd_float3 VFX_SIMD vfx_set_x(simd_float3 a, float x) { a.x = x; return a; }
static inline float VFX_SIMD vfx_get_y(simd_float3 a) { return a.y; }
static inline simd_float3 VFX_SIMD vfx_set_y(simd_float3 a, float y) { a.y = y; return a; }
static inline float VFX_SIMD vfx_get_z(simd_float3 a) { return a.z; }
static inline simd_float3 VFX_SIMD vfx_set_z(simd_float3 a, float z) { a.z = z; return a; }

static inline float VFX_SIMD vfx_get_x(simd_float4 a) { return a.x; }
static inline simd_float4 VFX_SIMD vfx_set_x(simd_float4 a, float x) { a.x = x; return a; }
static inline float VFX_SIMD vfx_get_y(simd_float4 a) { return a.y; }
static inline simd_float4 VFX_SIMD vfx_set_y(simd_float4 a, float y) { a.y = y; return a; }
static inline float VFX_SIMD vfx_get_z(simd_float4 a) { return a.z; }
static inline simd_float4 VFX_SIMD vfx_set_z(simd_float4 a, float z) { a.z = z; return a; }
static inline float VFX_SIMD vfx_get_w(simd_float4 a) { return a.w; }

static inline simd_float2 VFX_SIMD vfx_get_xy(simd_float3 a) { return a.xy; }
static inline simd_float2 VFX_SIMD vfx_get_xy(simd_float4 a) { return a.xy; }

static inline simd_float3 VFX_SIMD vfx_get_xyz(simd_float4 a) { return a.xyz; }
static inline simd_float4 VFX_SIMD vfx_set_xyz(simd_float4 a, simd_float3 xyz) { a.xyz = xyz; return a; }

static inline simd_float3 VFX_SIMD vfx_uv_to_plane_xz(simd_float2 uv) { return (simd_float3){ uv.x, 0.5f, uv.y } * 2.f - 1.f; }

static inline simd_float2 VFX_SIMD vfx_remap_n11_to_01(simd_float2 v) { return v * 0.5f + 0.5f; }
static inline simd_float3 VFX_SIMD vfx_remap_n11_to_01(simd_float3 v) { return v * 0.5f + 0.5f; }
static inline simd_float3 VFX_SIMD vfx_remap_01_to_n11(simd_float3 v) { return v * 2.f - 1.f; }

static inline bool VFX_SIMD vfx_is_almost_zero(float a) { return vfx_abs(a) <= VFX_EPSILON; }
static inline bool VFX_SIMD vfx_is_almost_zero(simd_float2 a) { return vfx_all(vfx_abs(a) <= vfx_splat2(VFX_EPSILON)); }
static inline bool VFX_SIMD vfx_is_almost_zero(simd_float3 a) { return vfx_all(vfx_abs(a) <= vfx_splat3(VFX_EPSILON)); }
static inline bool VFX_SIMD vfx_is_almost_zero(simd_float4 a) { return vfx_all(vfx_abs(a) <= vfx_splat4(VFX_EPSILON)); }

static inline bool VFX_SIMD vfx_equal(float a, float b) { return a == b; }
static inline bool VFX_SIMD vfx_equal(simd_float2 a, simd_float2 b) { return vfx_all(a == b); }
static inline bool VFX_SIMD vfx_equal(simd_float3 a, simd_float3 b) { return vfx_all(a == b); }
static inline bool VFX_SIMD vfx_equal(simd_float4 a, simd_float4 b) { return vfx_all(a == b); }

static inline bool VFX_SIMD vfx_notEqual(float a, float b) { return a != b; }
static inline bool VFX_SIMD vfx_notEqual(simd_float2 a, simd_float2 b) { return vfx_all(a != b); }
static inline bool VFX_SIMD vfx_notEqual(simd_float3 a, simd_float3 b) { return vfx_all(a != b); }
static inline bool VFX_SIMD vfx_notEqual(simd_float4 a, simd_float4 b) { return vfx_all(a != b); }

static inline bool VFX_SIMD vfx_is_almost_equal(float a, float b) { return vfx_abs(a - b) <= VFX_EPSILON; }
static inline bool VFX_SIMD vfx_is_almost_equal(simd_float2 a, simd_float2 b) { return vfx_all(vfx_abs(a - b) <= vfx_splat2(VFX_EPSILON)); }
static inline bool VFX_SIMD vfx_is_almost_equal(simd_float3 a, simd_float3 b) { return vfx_all(vfx_abs(a - b) <= vfx_splat3(VFX_EPSILON)); }
static inline bool VFX_SIMD vfx_is_almost_equal(simd_float4 a, simd_float4 b) { return vfx_all(vfx_abs(a - b) <= vfx_splat4(VFX_EPSILON)); }

static inline bool VFX_SIMD vfx_is_almost_equal(float a, float b, float eps) { return vfx_abs(a - b) <= eps; }
static inline bool VFX_SIMD vfx_is_almost_equal(simd_float2 a, simd_float2 b, float eps) { return vfx_all(vfx_abs(a - b) <= vfx_splat2(eps)); }
static inline bool VFX_SIMD vfx_is_almost_equal(simd_float3 a, simd_float3 b, float eps) { return vfx_all(vfx_abs(a - b) <= vfx_splat3(eps)); }
static inline bool VFX_SIMD vfx_is_almost_equal(simd_float4 a, simd_float4 b, float eps) { return vfx_all(vfx_abs(a - b) <= vfx_splat4(eps)); }

//static inline float VFX_SIMD vfx_max(int32_t a, int32_t b) { return a > b ? a : b; }
//static inline float VFX_SIMD vfx_min(int32_t a, int32_t b) { return a < b ? a : b; }

// retuns only -1 or 1. 0 is considered as 1
static inline float VFX_SIMD vfx_sign_fast(float a) { return a > 0 ? 1.f : -1.f; }
static inline simd_float2 VFX_SIMD vfx_sign_fast(simd_float2 a) { return vfx_select(-vfx_float2_one(), vfx_float2_one(), a >= 0.f); }
static inline simd_float3 VFX_SIMD vfx_sign_fast(simd_float3 a) { return vfx_select(-vfx_float3_one(), vfx_float3_one(), a >= 0.f); }
static inline simd_float4 VFX_SIMD vfx_sign_fast(simd_float4 a) { return vfx_select(-vfx_float4_one(), vfx_float4_one(), a >= 0.f); }


//static inline simd_float2 VFX_SIMD vfx_sincos(float a) {
//#ifdef __METAL_VERSION__
//    float2 sc; sc.x = sincos(a, sc.y); return sc;
//#else
//    struct __float2 sc = __sincosf_stret(a); return (simd_float2){ sc.__sinval, sc.__cosval };
//#endif
//}

static inline VFX_SIMD float vfx_sinc(float x) { if (x == 0.f) return 1.f; return vfx_sin(x) / x; }

static inline float VFX_SIMD vfx_mix(float a, float b, float t) { return vfx_muladd(b - a, t, a); }
static inline simd_float2 VFX_SIMD vfx_mix(simd_float2 a, simd_float2 b, float t) { return vfx_muladd(b - a, t, a); }
static inline simd_float3 VFX_SIMD vfx_mix(simd_float3 a, simd_float3 b, float t) { return vfx_muladd(b - a, t, a); }
static inline simd_float4 VFX_SIMD vfx_mix(simd_float4 a, simd_float4 b, float t) { return vfx_muladd(b - a, t, a); }
static inline simd_float2 VFX_SIMD vfx_mix(simd_float2 a, simd_float2 b, simd_float2 t) { return vfx_muladd(b - a, t, a); }
static inline simd_float3 VFX_SIMD vfx_mix(simd_float3 a, simd_float3 b, simd_float3 t) { return vfx_muladd(b - a, t, a); }
static inline simd_float4 VFX_SIMD vfx_mix(simd_float4 a, simd_float4 b, simd_float4 t) { return vfx_muladd(b - a, t, a); }

// TODO coherent order with mix/smoothstep => start, end, x
static inline float VFX_SIMD vfx_clamp(float v, float mn, float mx)                         { return vfx_min(vfx_max(v, mn), mx); }
static inline simd_float2 VFX_SIMD vfx_clamp(simd_float2 v, simd_float2 mn, simd_float2 mx) { return vfx_min(vfx_max(v, mn), mx); }
static inline simd_float3 VFX_SIMD vfx_clamp(simd_float3 v, simd_float3 mn, simd_float3 mx) { return vfx_min(vfx_max(v, mn), mx); }
static inline simd_float4 VFX_SIMD vfx_clamp(simd_float4 v, simd_float4 mn, simd_float4 mx) { return vfx_min(vfx_max(v, mn), mx); }

static inline float VFX_SIMD vfx_saturate(float v) { return vfx_clamp(v, 0, 1); }
static inline simd_float2 VFX_SIMD vfx_saturate(simd_float2 v) { return vfx_clamp(v, 0, 1); }
static inline simd_float3 VFX_SIMD vfx_saturate(simd_float3 v) { return vfx_clamp(v, 0, 1); }
static inline simd_float4 VFX_SIMD vfx_saturate(simd_float4 v) { return vfx_clamp(v, 0, 1); }

// TODO coherent order with mix/smoothstep => start, end, x
static inline float VFX_SIMD vfx_remap_01(float v, float start, float end) { return vfx_saturate( (v - start) / (end - start)); }
static inline simd_float2 VFX_SIMD vfx_remap_01(simd_float2 v, simd_float2 start, simd_float2 end) { return vfx_saturate( (v - start) / (end - start)); }
static inline simd_float3 VFX_SIMD vfx_remap_01(simd_float3 v, simd_float3 start, simd_float3 end) { return vfx_saturate( (v - start) / (end - start)); }
static inline simd_float4 VFX_SIMD vfx_remap_01(simd_float4 v, simd_float4 start, simd_float4 end) { return vfx_saturate( (v - start) / (end - start)); }

static inline float VFX_SIMD vfx_smoothstep(float a, float b, float x) { float xn = vfx_remap_01(x, a, b); return xn * xn * (3 - 2 * xn); }
static inline simd_float2 VFX_SIMD vfx_smoothstep(simd_float2 a, simd_float2 b, simd_float2 x) { simd_float2 xn = vfx_remap_01(x, a, b); return xn * xn * (3 - 2 * xn); }
static inline simd_float3 VFX_SIMD vfx_smoothstep(simd_float3 a, simd_float3 b, simd_float3 x) { simd_float3 xn = vfx_remap_01(x, a, b); return xn * xn * (3 - 2 * xn); }
static inline simd_float4 VFX_SIMD vfx_smoothstep(simd_float4 a, simd_float4 b, simd_float4 x) { simd_float4 xn = vfx_remap_01(x, a, b); return xn * xn * (3 - 2 * xn); }

static inline bool VFX_SIMD vfx_approxequal(float a, float b, float eps)             { return vfx_abs(a - b) < eps; }
static inline bool VFX_SIMD vfx_approxequal(simd_float2 a, simd_float2 b, float eps) { return vfx_all(vfx_abs(a - b) < eps); }
static inline bool VFX_SIMD vfx_approxequal(simd_float3 a, simd_float3 b, float eps) { return vfx_all(vfx_abs(a - b) < eps); }
static inline bool VFX_SIMD vfx_approxequal(simd_float4 a, simd_float4 b, float eps) { return vfx_all(vfx_abs(a - b) < eps); }

static inline float VFX_SIMD vfx_reduce_mul(simd_float2 a) { return a.x * a.y; }
static inline float VFX_SIMD vfx_reduce_mul(simd_float3 a) { return a.x * a.y * a.z; }
static inline float VFX_SIMD vfx_reduce_mul(simd_float4 a) { return a.x * a.y * a.z * a.w; }

static inline int VFX_SIMD vfx_min_index(simd_float4 v, simd_int4 ind) {
    simd_float4 min_lh = vfx_min(v, v.zwxy);
    simd_int4 min_ind_lh = simd_bitselect(ind, ind.zwxy, v > v.zwxy);
    return (min_lh.x < min_lh.y) ? min_ind_lh.x : min_ind_lh.y;
}

static inline simd_float2 VFX_SIMD vfx_robust_normalize(simd_float2 v) {
    simd_float2 length2 = vfx_length_squared(v);
    return vfx_select(v, v * vfx_rsqrt(length2), length2 != vfx_float4_zero().xy);
}

static inline simd_float4 VFX_SIMD vfx_robust_normalize(simd_float4 v) {
    simd_float4 length2 = vfx_length_squared(v);
    return vfx_select(v, v * vfx_rsqrt(length2), length2 != vfx_float4_zero());
}

static inline simd_float3 VFX_SIMD vfx_robust_normalize(simd_float3 v) {
    simd_float3 length2 = vfx_length_squared(v);
    return vfx_select(v, v * vfx_rsqrt(length2), length2 != vfx_float4_zero().xyz);
}

static inline void VFX_SIMD_UNPURE vfx_orthonormal_basis(simd_float3 n, vfx_out simd_float3 *t1, vfx_out simd_float3 *t2) {
    // from http://marc-b-reynolds.github.io/quaternions/2016/07/06/Orthonormal.html
    float sz = vfx_copysign(1.0f, n.z);
    float a  = n.y / (vfx_abs(n.z) + 1.f);
    float b  = n.y * a;
    float c  = -n.x * a;
    *t1 = (simd_float3){ n.z + sz * b,  sz * c,   -n.x       };
    *t2 = (simd_float3){ c,           1.f - b,    -sz * n.y };
}

static inline float VFX_SIMD vfx_angle(simd_float3 a, simd_float3 b) {
    float l2 = vfx_length(a) * vfx_length(b);
    return (l2 > 0.0) ? vfx_acos(vfx_dot(a, b) / l2) : 0.0;
}

//easing functions
static inline float       VFX_SIMD vfx_quadraticEaseIn(float a)     { return vfx_pow2(a); }
static inline float       VFX_SIMD vfx_quadraticEaseOut(float a)    { return -a * (a - 2.f); }
static inline float       VFX_SIMD vfx_quadraticEaseInOut(float a)  { return a < 0.5f ? (2.f * (a * a)) : (-1.f + (4.f - 2.f * a) * a); }

static inline float       VFX_SIMD vfx_cubicEaseIn(float a)         { return vfx_pow3(a); }
static inline float       VFX_SIMD vfx_cubicEaseOut(float a)        { return 1 - vfx_pow3(1 - a); }
static inline float       VFX_SIMD vfx_cubicEaseInOut(float a)      { return a < 0.5f ? (4.f * a * a * a) : ((a - 1.f) * (2.f * a - 2.f) * (2.f * a - 2.f) + 1.f); }

static inline float       VFX_SIMD vfx_quarticEaseIn(float a)       { return vfx_pow4(a); }
static inline float       VFX_SIMD vfx_quarticEaseOut(float a)      { return 1 - vfx_pow4(1-a); }
static inline float       VFX_SIMD vfx_quarticEaseInOut(float a)    { return a < 0.5f ? (8.f * a * a * a * a) : (1.f - vfx_pow4(-2.f * a + 2.f) * 0.5f); }

static inline float       VFX_SIMD vfx_quinticEaseIn(float a)       { return vfx_pow5(a); }
static inline float       VFX_SIMD vfx_quinticEaseOut(float a)      { return 1 - vfx_pow5(1 - a); }
static inline float       VFX_SIMD vfx_quinticEaseInOut(float a)    { return a < 0.5f ? (16.f * a * a * a * a * a) : (1.f - vfx_pow5(-2.f * a + 2.f) * 0.5f); }

#pragma mark - Batching

// batch dots

static inline simd_float2 VFX_SIMD vfx_dot_x2(simd_float3 v0, simd_float3 w0, simd_float3 v1, simd_float3 w1) {
    simd_float3x2 dot_mat = vfx_float3x2_from_rows(v0 * w0, v1 * w1);
    return dot_mat.columns[0] + dot_mat.columns[1] + dot_mat.columns[2];
}

static inline simd_float2 VFX_SIMD vfx_dot_x2(simd_float4 v0, simd_float4 w0, simd_float4 v1, simd_float4 w1) {
    simd_float4x2 dot_mat = vfx_float4x2_from_rows(v0 * w0, v1 * w1);
    return dot_mat.columns[0] + dot_mat.columns[1] + dot_mat.columns[2] + dot_mat.columns[3];
}

static inline simd_float2 VFX_SIMD vfx_dot3_x2(simd_float4 v0, simd_float4 w0, simd_float4 v1, simd_float4 w1) {
    simd_float4x2 dot_mat = vfx_float4x2_from_rows(v0 * w0, v1 * w1);
    return dot_mat.columns[0] + dot_mat.columns[1] + dot_mat.columns[2];
}

static inline simd_float3 VFX_SIMD vfx_dot_x3(simd_float3 v0, simd_float3 w0, simd_float3 v1, simd_float3 w1, simd_float3 v2, simd_float3 w2) {
    simd_float3x3 mat = vfx_float3x3_from_rows(v0 * w0, v1 * w1, v2 * w2);
    return mat.columns[0] + mat.columns[1] + mat.columns[2];
}

static inline simd_float3 VFX_SIMD vfx_dot_x3(simd_float4 v0, simd_float4 w0, simd_float4 v1, simd_float4 w1, simd_float4 v2, simd_float4 w2) {
    simd_float4x3 mat = vfx_float4x3_from_rows(v0 * w0, v1 * w1, v2 * w2);
    return mat.columns[0] + mat.columns[1] + mat.columns[2] + mat.columns[3];
}

static inline simd_float4 VFX_SIMD vfx_dot_x4(simd_float3 v0, simd_float3 w0, simd_float3 v1, simd_float3 w1, simd_float3 v2, simd_float3 w2, simd_float3 v3, simd_float3 w3) {
    simd_float3x4 dot_mat = vfx_float3x4_from_rows(v0 * w0, v1 * w1, v2 * w2, v3 * w3);
    return dot_mat.columns[0] + dot_mat.columns[1] + dot_mat.columns[2];
}

static inline simd_float4 VFX_SIMD vfx_dot_x4(simd_float4 v0, simd_float4 w0, simd_float4 v1, simd_float4 w1, simd_float4 v2, simd_float4 w2, simd_float4 v3, simd_float4 w3) {
    simd_float4x4 dot_mat = vfx_float4x4_from_rows(v0 * w0, v1 * w1, v2 * w2, v3 * w3);
    return dot_mat.columns[0] + dot_mat.columns[1] + dot_mat.columns[2] + dot_mat.columns[3];
}

static inline simd_float3 VFX_SIMD vfx_linear_combination(simd_float3 v0, float w0, simd_float3 v1, float w1) {
    return v0 * w0 + v1 * w1;
}


#pragma mark - Quaternion


static inline simd_float4 VFX_SIMD vfx_quat_normalize(simd_float4 q) { return vfx_robust_normalize(q); }
static inline float VFX_SIMD vfx_quat_length_(simd_float4 q) { return vfx_length(q); }
static inline simd_float4 VFX_SIMD vfx_quat_negate(simd_float4 q) { return -q; }
static inline simd_float4 VFX_SIMD vfx_quat_conjugate(simd_float4 q) { return q * (simd_float4){-1,-1,-1, 1}; }
static inline simd_float4 VFX_SIMD vfx_quat_inverse(simd_float4 q) { return vfx_quat_conjugate(q) / vfx_length_squared(q); }

static inline simd_float4 VFX_SIMD vfx_quat_identity_(void) { return vfx_float4_unit_w(); }
static inline bool VFX_SIMD vfx_quat_is_identity(simd_float4 q) { return vfx_approxequal(q, vfx_quat_identity_(), 0.000001f); }

static inline simd_float4 VFX_SIMD vfx_quat_mul(simd_float4 q1, simd_float4 q2) {
    return vfx_make_float4(q2.xyz * q1.w + q1.xyz * q2.w + vfx_cross(q1.xyz, q2.xyz), q1.w * q2.w - vfx_dot(q1.xyz, q2.xyz));
}

#ifndef __METAL_VERSION__
static inline simd_quatf VFX_SIMD _vfx_simd_quatf(simd_float4 v) { return (simd_quatf){ .vector = v }; }
static inline simd_quatf VFX_SIMD _vfx_simd_quatf(float x, float y, float z, float w) { return (simd_quatf){ .vector = vfx_make_float4(x, y, z, w) }; }

static inline simd_quatf VFX_SIMD vfx_quat_normalize(simd_quatf q) { return _vfx_simd_quatf(vfx_robust_normalize(q.vector)); }
static inline float VFX_SIMD vfx_quat_length(simd_quatf q) { return vfx_quat_length_(q.vector); }
static inline simd_quatf VFX_SIMD vfx_quat_negate(simd_quatf q) { return _vfx_simd_quatf(vfx_quat_negate(q.vector)); }
static inline simd_quatf VFX_SIMD vfx_quat_conjugate(simd_quatf q) { return _vfx_simd_quatf(vfx_quat_conjugate(q.vector)); }
static inline simd_quatf VFX_SIMD vfx_quat_inverse(simd_quatf q) { return _vfx_simd_quatf(vfx_quat_inverse(q.vector)); }
static inline simd_quatf VFX_SIMD vfx_quat_identity(void) { return _vfx_simd_quatf(vfx_quat_identity_()); }
static inline bool VFX_SIMD vfx_quat_is_identity(simd_quatf q) { return vfx_quat_is_identity(q.vector); }
static inline simd_quatf VFX_SIMD vfx_quat_mul(simd_quatf q1, simd_quatf q2) { return _vfx_simd_quatf(vfx_quat_mul(q1.vector, q2.vector)); }

#endif

static inline VFX_SIMD simd_float4 vfx_quat_(simd_float3x3 mat) {
    const simd_float3 vfx_out *m = mat.columns;
     float num8 = m[0][0] + m[1][1] + m[2][2];
     if (num8 > 0.0) {
         float num = vfx_sqrt(num8 + 1.f);
         return vfx_make_float4(m[1][2] - m[2][1], m[2][0] - m[0][2], m[0][1] - m[1][0], num * num) * (0.5f / num);
     } else if ((m[0][0] >= m[1][1]) && (m[0][0] >= m[2][2])) {
         float num7 = vfx_sqrt(((1.f + m[0][0]) - m[1][1]) - m[2][2]);
         return vfx_make_float4(num7 * num7, m[0][1] + m[1][0], m[0][2] + m[2][0], m[1][2] - m[2][1]) * (0.5f / num7);
     } else if (m[1][1] > m[2][2]) {
         float num6 = vfx_sqrt(((1.f + m[1][1]) - m[0][0]) - m[2][2]);
         return vfx_make_float4(m[1][0] + m[0][1], num6 * num6, m[2][1] + m[1][2], m[2][0] - m[0][2]) * (0.5f / num6);
     }
     float num5 = vfx_sqrt(((1.f + m[2][2]) - m[0][0]) - m[1][1]);
     return vfx_make_float4(m[2][0] + m[0][2], m[2][1] + m[1][2], num5 * num5, m[0][1] - m[1][0]) * (0.5f / num5);
}

static inline simd_float4 VFX_SIMD vfx_quat_(simd_float4x4 mat) { return vfx_quat_(vfx_float3x3(mat)); }

#ifdef __METAL_VERSION__ // custom versions

// rotation is p' = q * p * conjugate(q);
// simplification : https://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication/
static inline VFX_SIMD simd_float3 vfx_quat_rotate_vector(simd_float4 q, simd_float3 v) {
    return vfx_quat_mul(q, vfx_quat_mul(vfx_make_float4(v, 0), vfx_quat_conjugate(q))).xyz;
}

// TODO should be a matrix ctor !
static inline float3x3 vfx_quat_to_matrix3x3(simd_float4 q) {
    float3 q2 = q.xyz * q.xyz * 2.f;
    float3 wq = q.w * q.xyz;

    float xy = q.x * q.y;
    float yz = q.y * q.z;
    float xz = q.x * q.z;

    float3x3 m;
    m[0][0] = 1.0f - q2.y - q2.z;
    m[0][1] = 2.0f * (xy + wq.z);
    m[0][2] = 2.0f * (xz - wq.y);

    m[1][0] = 2.0f * (xy - wq.z);
    m[1][1] = 1.0f - q2.x - q2.z;
    m[1][2] = 2.0f * (yz + wq.x);

    m[2][0] = 2.0f * (xz + wq.y);
    m[2][1] = 2.0f * (yz - wq.x);
    m[2][2] = 1.0f - q2.x - q2.y;
    return m;
}

static inline float4x4 vfx_quat_to_matrix4x4(simd_float4 q) {
    return vfx_float4x4(vfx_quat_to_matrix3x3(q));
}

static inline simd_float4 VFX_SIMD vfx_quat_rotation_between(simd_float3 a, simd_float3 b) {
    float4 q;
    q.xyz = vfx_cross(a, b);
    q.w = vfx_sqrt(vfx_length_squared(a) * vfx_length_squared(b)) + vfx_dot(a, b);
    return vfx_quat_normalize(q);
}
static inline simd_float4 VFX_SIMD vfx_quat_rotation_between(simd_float4 a, simd_float4 b) { return vfx_quat_mul(b, vfx_quat_inverse(a)); }

#else // native SIMD version

static inline simd_float3 VFX_SIMD vfx_quat_rotate_vector(simd_float4 q, simd_float3 v) { return simd_act(_vfx_simd_quatf(q), v); }
static inline simd_float3x3 VFX_SIMD vfx_quat_to_matrix3x3(simd_float4 q) { return simd_matrix3x3(_vfx_simd_quatf(q)); }
static inline simd_float4x4 VFX_SIMD vfx_quat_to_matrix4x4(simd_float4 q) { return simd_matrix4x4(_vfx_simd_quatf(q)); }
static inline simd_float4 VFX_SIMD vfx_quat_rotation_between_(simd_float3 a, simd_float3 b) { return simd_quaternion(a, b).vector; }
static inline simd_float4 VFX_SIMD vfx_quat_rotation_between_(simd_float4 a, simd_float4 b) { return vfx_quat_mul(b, vfx_quat_inverse(a)); }

// simd_quatf versions
static inline VFX_SIMD simd_quatf vfx_quat(simd_float3x3 matrix) { return _vfx_simd_quatf(vfx_quat_(matrix)); }
static inline VFX_SIMD simd_quatf vfx_quat(simd_float4x4 matrix) { return _vfx_simd_quatf(vfx_quat_(matrix)); }
static inline simd_float3 VFX_SIMD vfx_quat_rotate_vector(simd_quatf q, simd_float3 v) { return vfx_quat_rotate_vector(q.vector, v); }
static inline simd_float3x3 VFX_SIMD vfx_quat_to_matrix3x3(simd_quatf q) { return vfx_quat_to_matrix3x3(q.vector); }
static inline simd_float4x4 VFX_SIMD vfx_quat_to_matrix4x4(simd_quatf q) { return vfx_quat_to_matrix4x4(q.vector); }
static inline simd_quatf VFX_SIMD vfx_quat_rotation_between(simd_float3 a, simd_float3 b) { return _vfx_simd_quatf(vfx_quat_rotation_between_(a, b)); }
static inline simd_quatf VFX_SIMD vfx_quat_rotation_between(simd_quatf a, simd_quatf b) { return _vfx_simd_quatf(vfx_quat_rotation_between_(a.vector, b.vector)); }

#endif


// Matrix

// Robust extraction of a rotation (3x3) matrix from a (scaled) 4x4.
// Supports for one zero axis. With more, the matrix won't be orthogonal
// but there will be no NaN
static inline simd_float3x3 VFX_SIMD vfx_extract_orthogonal(simd_float3x3 m) {
    simd_float3x3 dot_mat = vfx_float3x3_from_rows(m.columns[0] * m.columns[0],
                                                  m.columns[1] * m.columns[1],
                                                  m.columns[2] * m.columns[2]);
    simd_float3 len_sqr = dot_mat.columns[0] + dot_mat.columns[1] + dot_mat.columns[2];
#ifdef __METAL_VERSION__
    simd_bool3 len_is_zero = len_sqr <= VFX_MIN;
#else
    simd_int3 len_is_zero = len_sqr <= VFX_MIN;
#endif
    simd_float3 inv_len = vfx_select(vfx_precise_rsqrt(len_sqr), len_sqr, len_is_zero) ;
    simd_float3 vx = m.columns[0] * inv_len.x;
    simd_float3 vy = m.columns[1] * inv_len.y;
    simd_float3 vz = m.columns[2] * inv_len.z;
    return vfx_float3x3(
        len_is_zero.x ? vfx_cross(vy, vz) : vx,
        len_is_zero.y ? vfx_cross(vz, vx) : vy,
        len_is_zero.z ? vfx_cross(vx, vy) : vz
    );
}
static inline simd_float3x3 VFX_SIMD vfx_extract_orthogonal(simd_float4x4 m) {
    return vfx_extract_orthogonal(vfx_float3x3(m.columns[0].xyz, m.columns[1].xyz, m.columns[2].xyz) );
}

static inline simd_float4 VFX_SIMD vfx_quat_from_matrix3x3_(simd_float3x3 m) { return vfx_quat_(vfx_extract_orthogonal(m)); }
static inline simd_float4 VFX_SIMD vfx_quat_from_matrix4x4_(simd_float4x4 m) { return vfx_quat_(vfx_extract_orthogonal(m)); }

#ifndef __METAL_VERSION__
static inline simd_quatf VFX_SIMD vfx_quat_from_matrix3x3(simd_float3x3 m) { return _vfx_simd_quatf(vfx_quat_from_matrix3x3_(m)); }
static inline simd_quatf VFX_SIMD vfx_quat_from_matrix4x4(simd_float4x4 m) { return _vfx_simd_quatf(vfx_quat_from_matrix4x4_(m)); }
#endif

static inline simd_float4 VFX_SIMD vfx_quat_from_normal_(simd_float3 n) {
    n = vfx_robust_normalize(n);
    simd_float3 t,b;
    vfx_orthonormal_basis(n, &t, &b);
    return vfx_quat_(vfx_float3x3(t, b, n));
}
#ifndef __METAL_VERSION__
static inline simd_quatf VFX_SIMD vfx_quat_from_normal(simd_float3 n) { return _vfx_simd_quatf(vfx_quat_from_normal_(n)); }
#endif

static inline simd_float3 VFX_SIMD vfx_quat_to_normal(simd_float4 q) { return vfx_quat_to_matrix3x3(q).columns[2]; }
#ifndef __METAL_VERSION__
static inline simd_float3 VFX_SIMD vfx_quat_to_normal(simd_quatf q) { return vfx_quat_to_normal(q.vector); }
#endif

// this function will produce random sign for 180deg rotation
// use only when you know taht you deal with small rotation matrices
static inline simd_float4 VFX_SIMD vfx_quat_from_matrix4x4_describing_small_rotation_(simd_float4x4 m)
{
    // based on Branchless Matrix to Quaternion Conversion
    // from http://www.thetenthplanet.de/archives/1994
    // warning : we store real in w, not in x
    simd_float4 sign = { 1.f, -1.f, -1.f, 1.f };
    simd_float4 mx = sign.xxxx + m.columns[0][0] * sign + m.columns[1][1] * sign.yxzw + m.columns[2][2] * sign.yzxw;
    simd_float4 q = 0.5f * vfx_sqrt(vfx_max(vfx_float4_zero(), mx));
    q.xyz = vfx_copysign(q.xyz, vfx_make_float3(m.columns[1][2] - m.columns[2][1], m.columns[2][0] - m.columns[0][2], m.columns[0][1] - m.columns[1][0]));
    return q;
}
#ifndef __METAL_VERSION__
static inline simd_quatf VFX_SIMD vfx_quat_from_matrix4x4_describing_small_rotation(simd_float4x4 m) { return _vfx_simd_quatf(vfx_quat_from_matrix4x4_describing_small_rotation_(m));}
#endif

// LookAt

static inline simd_float4 VFX_SIMD vfx_quat_look_at_(simd_float3 forward, simd_float3 up) {
    forward = vfx_robust_normalize(forward);
    // deal with forward and up colinear -> use forward.yzx (rotation) as up then
    simd_float3 v = vfx_cross(up, forward);
    simd_float3 length2 = vfx_length_squared(v);
    simd_float3 right = vfx_select(forward.yzx, v * vfx_rsqrt(length2), length2 != vfx_float3_zero());
    up = vfx_normalize(vfx_cross(forward, right));
    return vfx_quat_(vfx_float3x3(right, up, forward));
}
#ifndef __METAL_VERSION__
static inline simd_quatf VFX_SIMD vfx_quat_look_at(simd_float3 forward, simd_float3 up) { return _vfx_simd_quatf(vfx_quat_look_at_(forward, up)); }
#endif

// From-To

static inline simd_float4 VFX_SIMD vfx_quat_rotation_between_fast_(simd_float3 a, simd_float3 b) {
    simd_float4 q;
    q.xyz = vfx_cross(a, b);
    q.w = vfx_sqrt(vfx_length_squared(a) * vfx_length_squared(b)) + vfx_dot(a, b);
    return vfx_quat_normalize(q);
}

#ifndef __METAL_VERSION__
static inline simd_quatf VFX_SIMD vfx_quat_rotation_between_fast(simd_float3 a, simd_float3 b) { return _vfx_simd_quatf(vfx_quat_rotation_between_fast_(a, b)); }
#endif

// Axis-Angle

static inline VFX_SIMD simd_float4 vfx_quat_from_axisangle_unit_(simd_float3 axis_unit, float angle) { return vfx_make_float4(sin(angle/2) * axis_unit, cos(angle/2)); }
static inline simd_float4 VFX_SIMD vfx_quat_from_axisangle_(simd_float3 axis, float angle) { return vfx_quat_from_axisangle_unit_(vfx_robust_normalize(axis), angle); }
static inline float VFX_SIMD vfx_quat_angle(simd_float4 q) { return 2 * vfx_atan2(vfx_length(q.xyz), q.w); }
static inline simd_float3 VFX_SIMD vfx_quat_axis(simd_float4 q) { return vfx_normalize(q.xyz); }
static inline simd_float4 VFX_SIMD vfx_quat_from_axisangle_(simd_float4 axis_angle) { return vfx_quat_from_axisangle_(axis_angle.xyz, axis_angle.w); }
static inline simd_float4 VFX_SIMD vfx_quat_to_axisangle_(simd_float4 q) { return vfx_make_float4(vfx_quat_axis(q), vfx_quat_angle(q)); }

#ifndef __METAL_VERSION__ // simd_quatf versions
static inline VFX_SIMD simd_quatf vfx_quat_from_axisangle_unit(simd_float3 axis_unit, float angle) { return _vfx_simd_quatf(vfx_quat_from_axisangle_unit_(axis_unit, angle)); }
static inline simd_quatf VFX_SIMD vfx_quat_from_axisangle(simd_float3 axis, float angle) { return _vfx_simd_quatf(vfx_quat_from_axisangle_(axis, angle)); }
static inline float VFX_SIMD vfx_quat_angle(simd_quatf q) { return vfx_quat_angle(q.vector); }
static inline simd_float3 VFX_SIMD vfx_quat_axis(simd_quatf q) { return vfx_quat_axis(q.vector); }
static inline simd_quatf VFX_SIMD vfx_quat_from_axisangle(simd_float4 axis_angle) { return _vfx_simd_quatf(vfx_quat_from_axisangle_(axis_angle)); }
static inline simd_float4 VFX_SIMD vfx_quat_to_axisangle(simd_quatf q) { return vfx_quat_to_axisangle_(q.vector); }
#endif


// Euler

static inline simd_float4 VFX_SIMD vfx_quat_from_euler_(simd_float3 angles) {
    simd_float3 angles_2 = angles / 2;
    simd_float3 cs = vfx_cos(angles_2);
    simd_float3 sn = vfx_sin(angles_2);
    float cpcy = cs.y * cs.z;
    float spsy = sn.y * sn.z;
    return vfx_make_float4(sn.x * cpcy - cs.x * spsy,
                           cs.x * sn.y * cs.z + sn.x * cs.y * sn.z,
                           cs.x * cs.y * sn.z - sn.x * sn.y * cs.z,
                           cs.x * cpcy + sn.x * spsy);
}

static inline simd_float4 VFX_SIMD vfx_quat_from_x_rotation(float angle) {
    float angle_2 = angle / 2;
    float cs = vfx_cos(angle_2);
    float sn = vfx_sin(angle_2);
    return vfx_make_float4(sn, 0.f, 0.f, cs);
}

static inline simd_float4 VFX_SIMD vfx_quat_from_y_rotation(float angle) {
    float angle_2 = angle / 2;
    float cs = vfx_cos(angle_2);
    float sn = vfx_sin(angle_2);
    return vfx_make_float4(0.f, sn, 0.f, cs);
}

static inline simd_float4 VFX_SIMD vfx_quat_from_z_rotation(float angle) {
    float angle_2 = angle / 2;
    float cs = vfx_cos(angle_2);
    float sn = vfx_sin(angle_2);
    return vfx_make_float4(0.f, 0.f, sn, cs);
}

static inline simd_float3 VFX_SIMD vfx_quat_to_euler(simd_float4 q) {
    simd_float4 sq = q * q;
    float unit = vfx_reduce_add(sq); //should be 1 if the quaternion is normalized
    if (unit == 0.f) return vfx_float4_zero().xyz;
    float test = (q.x * q.z - q.y * q.w) / unit;
    if (test >  0.499) return vfx_make_float3(  2.f * vfx_atan2(q.x, q.w),  VFX_PI_2, 0.f ); // singularity at north pole
    if (test < -0.499) return vfx_make_float3( -2.f * vfx_atan2(q.x, q.w), -VFX_PI_2, 0.f ); // singularity at south pole
    return vfx_make_float3(vfx_atan2(2.f * (q.y * q.z + q.x * q.w), (-sq.x - sq.y + sq.z + sq.w)),
                           vfx_asin(-2.f * test),
                           vfx_atan2(2.f * (q.x * q.y + q.z * q.w), ( sq.x - sq.y - sq.z + sq.w)));
}

// Math

static inline VFX_SIMD simd_float4 vfx_quat_log(simd_float4 q) {
    float real = vfx_log(vfx_length_squared(q))/2;
    if (vfx_equal(q.xyz, vfx_float3_zero())) return vfx_make_float4(vfx_float3_zero(), real);
    simd_float3 imag = vfx_acos(q.w / vfx_length(q)) * vfx_normalize(q.xyz);
    return vfx_make_float4(imag, real);
}

static inline simd_float4 VFX_SIMD vfx_quat_ln(simd_float4 q) {
    float r = vfx_length(q.xyz);
    float t = r > 10E-5 ? vfx_atan2(r, q.w) / r : 0.0f;
    return vfx_make_float4(q.xyz * t, 0.5f * vfx_log(vfx_length_squared(q.xyz)));
}

static inline simd_float4 VFX_SIMD vfx_quat_exp(simd_float4 q) {
    float r = vfx_length(q.xyz);
    float et = vfx_exp(q.w);
    float s = r >= 10E-5 ? et * vfx_sin(r) / r : 0.0f;
    return vfx_make_float4(q.xyz * s, et * vfx_cos(r));
}

static inline simd_float4 VFX_SIMD vfx_quat_pow(simd_float4 q, float p) {
    return vfx_quat_ln(p * vfx_quat_exp(q));
}

#ifndef __METAL_VERSION__ // simd_quatf versions
static inline simd_quatf VFX_SIMD vfx_quat_from_euler(simd_float3 angles) { return _vfx_simd_quatf(vfx_quat_from_euler_(angles)); }
static inline simd_float3 VFX_SIMD vfx_quat_to_euler(simd_quatf q) { return vfx_quat_to_euler(q.vector); }
static inline VFX_SIMD simd_quatf vfx_quat_log(simd_quatf q) { return _vfx_simd_quatf(vfx_quat_log(q.vector)); }
static inline VFX_SIMD simd_quatf vfx_quat_ln(simd_quatf q) { return _vfx_simd_quatf(vfx_quat_ln(q.vector)); }
static inline VFX_SIMD simd_quatf vfx_quat_exp(simd_quatf q) { return _vfx_simd_quatf(vfx_quat_exp(q.vector)); }
static inline VFX_SIMD simd_quatf vfx_quat_pow(simd_quatf q, float p) { return _vfx_simd_quatf(vfx_quat_pow(q.vector, p)); }
#endif


// Interpolations

// helpers

static inline VFX_SIMD float _vfx_quat_angle(simd_float4 p, simd_float4 q) {
    // alternative version, TO TEST
    // float d = vfx_dot(p, q);
    // return d > 1 ? 0 : acos(d < -1 ? -1 : d);
    return 2 * vfx_atan2(vfx_length(p - q), vfx_length(p + q));
}

static inline VFX_SIMD simd_float4 _vfx_quat_slerp_internal(simd_float4 q0, simd_float4 q1, float t) {
  float s = 1 - t;
  float a = _vfx_quat_angle(q0, q1);
  float r = vfx_recip(vfx_sinc(a));
  return vfx_quat_normalize(vfx_sinc(s*a)*r*s*q0 + vfx_sinc(t*a)*r*t*q1);
}

// Spherical linear interpolation along the shortest arc between quaternions `q0` and `q1`
static inline simd_float4 VFX_SIMD vfx_quat_slerp(simd_float4 from, simd_float4 to, float t) {
    if (vfx_dot(from, to) >= 0)
        return _vfx_quat_slerp_internal(from, to, t);
    return _vfx_quat_slerp_internal(from, vfx_quat_negate(to), t);
}

// Spherical linear interpolation along the longest arc between quaternions `q0` and `q1`
static inline simd_float4 VFX_SIMD vfx_quat_slerp_longest(simd_float4 from, simd_float4 to, float t) {
    if (vfx_dot(from, to) >= 0)
      return _vfx_quat_slerp_internal(from, vfx_quat_negate(to), t);
    return _vfx_quat_slerp_internal(from, to, t);
}

static inline VFX_SIMD simd_float4 _vfx_quat_squad(simd_float4 q0, simd_float4 qa, simd_float4 qb, simd_float4 q1, float t) {
    simd_float4 r0 = _vfx_quat_slerp_internal(q0, q1, t);
    simd_float4 r1 = _vfx_quat_slerp_internal(qa, qb, t);
  return _vfx_quat_slerp_internal(r0, r1, 2*t*(1 - t));
}

static inline VFX_SIMD simd_float4 _vfx_quat_squad_intermediate(simd_float4 q0, simd_float4 q1, simd_float4 q2) {
    simd_float4 p0 = vfx_quat_log(vfx_quat_mul(q0, vfx_quat_inverse(q1)));
    simd_float4 p2 = vfx_quat_log(vfx_quat_mul(q2, vfx_quat_inverse(q1)));
    return vfx_quat_normalize(vfx_quat_mul(q1, vfx_quat_exp(vfx_quat_mul(-0.25f, p0 + p2))));
}

// aka Shoemake's Quaternion curves or spline
// Interpolate between quaternions along a spherical cubic spline
// The function interpolates between q1 and q2. q0 is the left
//  endpoint of the previous interval, and q3 is the right endpoint of the next
//  interval. Use this function to smoothly interpolate between a sequence of
//  rotations
static inline simd_float4 VFX_SIMD vfx_quat_squad(simd_float4 q0, simd_float4 q1, simd_float4 q2, simd_float4 q3, float t) {
    simd_float4 qa = _vfx_quat_squad_intermediate(q0, q1, q2);
    simd_float4 qb = _vfx_quat_squad_intermediate(q1, q2, q3);
    return _vfx_quat_squad(q1, qa, qb, q2, t);
}

// Spherical cubic Bezier interpolation between quaternions
// The function treats q0 ... q3 as control points and uses slerp
//  in place of lerp in the De Castlejeau algorithm. The endpoints of
//  interpolation are thus q0 and q3, and the curve will not generally pass
//  through q1 or q2. Note that the convex hull property of "standard" Bezier
//  curve does not hold on the sphere
static inline simd_float4 VFX_SIMD vfx_quat_bezier(simd_float4 q0, simd_float4 q1, simd_float4 q2, simd_float4 q3, float t) {
    simd_float4 q01 = _vfx_quat_slerp_internal(q0, q1, t);
    simd_float4 q12 = _vfx_quat_slerp_internal(q1, q2, t);
    simd_float4 q23 = _vfx_quat_slerp_internal(q2, q3, t);
    simd_float4 q012 = _vfx_quat_slerp_internal(q01, q12, t);
    simd_float4 q123 = _vfx_quat_slerp_internal(q12, q23, t);
    return _vfx_quat_slerp_internal(q012, q123, t);
}

#ifndef __METAL_VERSION__ // simd_quatf versions
static inline VFX_SIMD float _vfx_quat_angle(simd_float4 p, simd_quatf q) { return _vfx_quat_angle(p, q.vector); }
static inline VFX_SIMD simd_quatf _vfx_quat_slerp_internal(simd_quatf q0, simd_quatf q1, float t) { return _vfx_simd_quatf(_vfx_quat_slerp_internal(q0.vector, q1.vector, t)); }
static inline simd_quatf VFX_SIMD vfx_quat_slerp(simd_quatf from, simd_quatf to, float t) { return _vfx_simd_quatf(vfx_quat_slerp(from.vector, to.vector, t)); }
static inline simd_quatf VFX_SIMD vfx_quat_slerp_longest(simd_quatf from, simd_quatf to, float t) { return _vfx_simd_quatf(vfx_quat_slerp_longest(from.vector, to.vector, t)); }
static inline VFX_SIMD simd_quatf _vfx_quat_squad(simd_quatf q0, simd_quatf qa, simd_quatf qb, simd_quatf q1, float t) { return _vfx_simd_quatf(_vfx_quat_squad(q0.vector, qa.vector, qb.vector, q1.vector, t)); }
static inline VFX_SIMD simd_quatf _vfx_quat_squad_intermediate(simd_quatf q0, simd_quatf q1, simd_quatf q2) { return _vfx_simd_quatf(_vfx_quat_squad_intermediate(q0.vector, q1.vector, q2.vector)); }
static inline simd_quatf VFX_SIMD vfx_quat_squad(simd_quatf q0, simd_quatf q1, simd_quatf q2, simd_quatf q3, float t) { return _vfx_simd_quatf(vfx_quat_squad(q0.vector, q1.vector, q2.vector, q3.vector, t)); }
static inline simd_quatf VFX_SIMD vfx_quat_bezier(simd_quatf q0, simd_quatf q1, simd_quatf q2, simd_quatf q3, float t) { return _vfx_simd_quatf(vfx_quat_bezier(q0.vector, q1.vector, q2.vector, q3.vector, t)); }
#endif


#pragma mark - Matrix3x3

#ifndef __METAL_VERSION__
static inline simd_float3x3 VFX_SIMD vfx_mul(simd_float3x3 a, simd_float3x3 b) { return simd_mul(a, b); }
#else
static inline simd_float3x3 VFX_SIMD vfx_mul(simd_float3x3 a, simd_float3x3 b) { return a * b; }
#endif


// Create a 2D transform with (a)ngle, (t)ranslation and (s)cale
static inline simd_float3x3 VFX_SIMD vfx_make_transform(float a, simd_float2 t, simd_float2 s) {
    float cs = vfx_cos(a); float sn = vfx_sin(a);
    return vfx_float3x3(
         s.x * vfx_make_float3( cs, -sn, 0 ),
         s.y * vfx_make_float3( sn,  cs, 0 ),
               vfx_make_float3(  t,   1 )
    );
}

static inline simd_float2 VFX_SIMD vfx_transform(simd_float3x3 m, simd_float2 p) {
    simd_float2 result = m.columns[2].xy;
    result += p.x * m.columns[0].xy;
    result += p.y * m.columns[1].xy;
    return result;
}

static inline simd_float3x3 VFX_SIMD vfx_translate(simd_float3x3 m, simd_float2 t) { m.columns[2].xy += t.xy; return m; }
static inline simd_float3x3 VFX_SIMD vfx_scale(simd_float3x3 m, simd_float2 s)     { m.columns[0] *= s.x; m.columns[1] *= s.y; return m; }
static inline simd_float3x3 VFX_SIMD vfx_scale(simd_float3x3 m, simd_float3 s)     { m.columns[0] *= s.x; m.columns[1] *= s.y; m.columns[2] *= s.z; return m; }

static inline simd_float2 VFX_SIMD vfx_get_translation_2D(simd_float3x3 m)      { return m.columns[2].xy; }
static inline simd_float2 VFX_SIMD vfx_get_scale_2D(simd_float3x3 m) { return vfx_make_float2(vfx_length(m.columns[0].xy), vfx_length(m.columns[1].xy)); }

static inline simd_float3 VFX_SIMD vfx_get_scale(simd_float3x3 m) { return vfx_make_float3(vfx_length(m.columns[0].xyz), vfx_length(m.columns[1].xyz), vfx_length(m.columns[2].xyz)); }

#pragma mark - Matrix4x4

#ifndef __METAL_VERSION__
static inline simd_float4x4 VFX_SIMD vfx_mul(simd_float4x4 a, simd_float4x4 b) { return simd_mul(a, b); }
#else
static inline simd_float4x4 VFX_SIMD vfx_mul(simd_float4x4 a, simd_float4x4 b) { return a * b; }
#endif

// Avoid zero scale. WARNING: this prevents negative scale
static inline simd_float3 VFX_SIMD vfx_robust_scale(simd_float3 s)      {
    VFX_MATH_ASSERT(!vfx_is_almost_zero(s));
    return vfx_max(vfx_abs(s), vfx_splat3(VFX_EPSILON));
}

static inline simd_float4x4 VFX_SIMD vfx_translate(simd_float4x4 m, simd_float4 t_w1)   { m.columns[3].xyz += t_w1.xyz; return m; }
static inline simd_float4x4 VFX_SIMD vfx_translate(simd_float4x4 m, simd_float3 t)      { m.columns[3].xyz += t; return m; }
static inline simd_float4x4 VFX_SIMD vfx_scale(simd_float4x4 m, simd_float3 s)          { m.columns[0] *= s.x; m.columns[1] *= s.y; m.columns[2] *= s.z; return m; }

static inline simd_float3 VFX_SIMD vfx_get_translation(simd_float4x4 m)      { return m.columns[3].xyz; }
static inline simd_float3 VFX_SIMD vfx_get_scale(simd_float4x4 m) { return vfx_make_float3(vfx_length(m.columns[0].xyz), vfx_length(m.columns[1].xyz), vfx_length(m.columns[2].xyz)); }
static inline simd_float3x3 VFX_SIMD vfx_get_rotation(simd_float4x4 m)      { return vfx_float3x3(vfx_robust_normalize(m.columns[0].xyz), vfx_robust_normalize(m.columns[1].xyz), vfx_robust_normalize(m.columns[2].xyz)); }

static inline simd_float4x4 VFX_SIMD vfx_make_transform(simd_float4 q, simd_float4 t_w1, simd_float3 scale) {
    scale = vfx_robust_scale(scale);
    simd_float3 q2 = q.xyz * q.xyz;
    simd_float3 wq = q.w * q.xyz;
    float xy = q.x * q.y;
    float yz = q.y * q.z;
    float xz = q.x * q.z;
    simd_float3 s = scale + scale; // faster than 2 * scale
    return (simd_float4x4){
        s.x * vfx_make_float4(0.5f - q2.y - q2.z, xy + wq.z, xz - wq.y, 0),
        s.y * vfx_make_float4(xy - wq.z, 0.5f - q2.x - q2.z, yz + wq.x, 0),
        s.z * vfx_make_float4(xz + wq.y, yz - wq.x, 0.5f - q2.x - q2.y, 0),
        t_w1
    };
}
#ifndef __METAL_VERSION__
static inline simd_float4x4 VFX_SIMD vfx_make_transform(simd_quatf q, simd_float4 t_w1, simd_float3 scale) {
    return vfx_make_transform(q.vector, t_w1, scale);
}
#endif
static inline simd_float4x4 VFX_SIMD vfx_make_transform_rotation_translation(simd_float4 q, simd_float4 t_w1) { return vfx_make_transform(q, t_w1, vfx_splat3(1.f)); }
#ifndef __METAL_VERSION__
static inline simd_float4x4 VFX_SIMD vfx_make_transform_rotation_translation(simd_quatf q, simd_float4 t_w1) { return vfx_make_transform_rotation_translation(q.vector, t_w1); }
#endif
static inline simd_float4x4 VFX_SIMD vfx_make_transform_rotation_scale(simd_float4 q, simd_float3 scale) { return vfx_make_transform(q, vfx_make_float4(0.f, 0.f, 0.f, 1.f), scale); }
#ifndef __METAL_VERSION__
static inline simd_float4x4 VFX_SIMD vfx_make_transform_rotation_scale(simd_quatf q, simd_float3 scale) { return vfx_make_transform_rotation_scale(q.vector, scale); }
#endif
static inline simd_float4x4 VFX_SIMD vfx_make_transform_translation_scale(simd_float4 t_w1, simd_float3 s) { s = vfx_robust_scale(s); return vfx_float4x4(
    vfx_make_float4( s.x, 0.f, 0.f, 0.f ),
    vfx_make_float4( 0.f, s.y, 0.f, 0.f ),
    vfx_make_float4( 0.f, 0.f, s.z, 0.f ),
    t_w1);
}
static inline simd_float4x4 VFX_SIMD vfx_make_translation(simd_float4 t_w1) { return (simd_float4x4){
    vfx_make_float4( 1.f, 0.f, 0.f, 0.f ),
    vfx_make_float4( 0.f, 1.f, 0.f, 0.f ),
    vfx_make_float4( 0.f, 0.f, 1.f, 0.f ),
    t_w1};
}
static inline simd_float4x4 VFX_SIMD vfx_make_scale(simd_float3 scale) { return vfx_diagonal(vfx_make_float4(vfx_robust_scale(scale), 1.f)); }
static inline simd_float4x4 VFX_SIMD vfx_make_rotation(simd_float4 q) { return vfx_make_transform_rotation_translation(q, vfx_float4_unit_w()); }
#ifndef __METAL_VERSION__
static inline simd_float4x4 VFX_SIMD vfx_make_rotation(simd_quatf q) { return vfx_make_rotation(q.vector); }
#endif

static inline simd_float3x3 VFX_SIMD vfx_inverse_fast(simd_float3x3 m) {
    simd_float3 a = m.columns[0];
    simd_float3 b = m.columns[1];
    simd_float3 c = m.columns[2];
    simd_float3 r0 = vfx_cross(b, c);
    simd_float3 r1 = vfx_cross(c, a);
    simd_float3 r2 = vfx_cross(a, b);
    float inv_det = 1.f / vfx_dot(r2, c);
    return vfx_float3x3_from_rows(r0 * inv_det, r1 * inv_det, r2 * inv_det);
}

// This is only valid with a transform matrix (i.e. rows.3 = 0,0,0,1)
static inline simd_float4x4 VFX_SIMD vfx_inverse_fast(simd_float4x4 m) {
    simd_float3 a = m.columns[0].xyz;
    simd_float3 b = m.columns[1].xyz;
    simd_float3 c = m.columns[2].xyz;
    simd_float3 d = m.columns[3].xyz;
    float x = m.columns[0].w;
    float y = m.columns[1].w;
    float z = m.columns[2].w;
    float w = m.columns[3].w;
    simd_float3 s = vfx_cross(a, b);
    simd_float3 t = vfx_cross(c, d);
    simd_float3 u = a * y - b * x;
    simd_float3 v = c * w - d * z;
    float inv_det = 1.f / (vfx_dot(s, v) + vfx_dot(t, u));
    s *= inv_det;
    t *= inv_det;
    u *= inv_det;
    v *= inv_det;
    simd_float3 r0 = vfx_cross(b, v) + t * y;
    simd_float3 r1 = vfx_cross(v, a) - t * x;
    simd_float3 r2 = vfx_cross(d, u) + s * w;
    simd_float3 r3 = vfx_cross(u, c) - s * z;
    return vfx_float4x4_from_rows(vfx_make_float4( r0.x, r0.y, r0.z, -vfx_dot(b, t)),
                                  vfx_make_float4( r1.x, r1.y, r1.z,  vfx_dot(a, t)),
                                  vfx_make_float4( r2.x, r2.y, r2.z, -vfx_dot(d, s)),
                                  vfx_make_float4( r3.x, r3.y, r3.z,  vfx_dot(c, s)));
}


static inline simd_float4x4 VFX_SIMD vfx_inverse_transpose(simd_float4x4 m) {
    simd_float3 scale_squared = vfx_dot_x3(m.columns[0].xyz, m.columns[0].xyz,
                                           m.columns[1].xyz, m.columns[1].xyz,
                                           m.columns[2].xyz, m.columns[2].xyz);
    // this avoid NaN but still flatten the features (e.g. a plane has no more valid normal when scaled with 0)
    simd_float3 inv_scale_squared = vfx_select(1 / scale_squared, scale_squared, scale_squared == 0);
    return vfx_scale(m, inv_scale_squared);
}

static inline simd_float2 VFX_SIMD vfx_transform_direction(simd_float3x3 m, simd_float2 d) {
    simd_float2 result = d.x * m.columns[0].xy;
    result += d.y * m.columns[1].xy;
    return result;
}
static inline simd_float3 VFX_SIMD vfx_transform_direction(simd_float4x4 m, simd_float3 d) {
    return d.x * m.columns[0].xyz + (d.y * m.columns[1].xyz + (d.z * m.columns[2].xyz));
}
static inline simd_float3 VFX_SIMD vfx_transform_normal(simd_float4x4 m, simd_float3 d) {
    return vfx_transform_direction(vfx_inverse_transpose(m), d);
}
static inline simd_float2 VFX_SIMD vfx_transform_position(simd_float3x3 m, simd_float2 p) {
    simd_float2 result = m.columns[2].xy;
    result += p.x * m.columns[0].xy;
    result += p.y * m.columns[1].xy;
    return result;
}
static inline simd_float3 VFX_SIMD vfx_transform_position(simd_float4x4 m, simd_float3 p) {
    return p.x * m.columns[0].xyz + (p.y * m.columns[1].xyz + (p.z * m.columns[2].xyz + m.columns[3].xyz));
}
static inline simd_float3 VFX_SIMD vfx_project_position(simd_float4x4 m, simd_float3 p) {
    simd_float4 result = m.columns[3];
    result += p.x * m.columns[0];
    result += p.y * m.columns[1];
    result += p.z * m.columns[2];
    if (result.w != 0) {
        result /= result.w;
    } else {
        result /= 1e-8f;
    }
    return result.xyz;
}


static inline simd_float4x4 VFX_SIMD vfx_make_rotation_around(simd_float4 q, simd_float3 center) {
    simd_float3 tr = vfx_quat_rotate_vector(q, -center);
    return vfx_make_transform_rotation_translation(q, vfx_make_float4(tr + center, 1));
}
#ifndef __METAL_VERSION__
static inline simd_float4x4 VFX_SIMD vfx_make_rotation_around(simd_quatf q, simd_float3 center) { return vfx_make_rotation_around(q.vector, center); }
#endif


#ifndef __METAL_VERSION__
static inline bool vfx_decompose_affine(simd_float4x4 m,
                                        vfx_out simd_quatf *quaternion,
                                        vfx_out simd_float3 *position,
                                        vfx_out simd_float3 *scale)
{
    if (scale) {
        *scale = vfx_get_scale(m);
    }
    
    if (quaternion) {
        simd_float3 s = scale ? *scale : vfx_get_scale(m);
        m.columns[0].xyz /= s.x;
        m.columns[1].xyz /= s.y;
        m.columns[2].xyz /= s.z;
        *quaternion = vfx_quat_from_matrix4x4(m);
    }
    
    if (position) {
        *position = vfx_get_translation(m);
    }
    
    return true;
}

static inline simd_float4x4 VFX_SIMD vfx_matrix_float4x4_make_affine_rh(simd_float3 trans, simd_quatf quat, simd_float3 scale)
{
    simd_float4x4 m = vfx_quat_to_matrix4x4(quat);
    m = vfx_scale(m, scale);
    m = vfx_translate(m, trans);
    return m;
}


static inline simd_float4x4 VFX_SIMD vfx_matrix_interpolate(simd_float4x4 m1, simd_float4x4 m2, float alpha)
{
    simd_quatf q1, q2;
    simd_float3 t1, t2, s1, s2;
    
    bool aff = vfx_decompose_affine(m1, &q1, &t1, &s1);
    aff     &= vfx_decompose_affine(m2, &q2, &t2, &s2);
    
    // if not affine can't interpolate
    if (!aff)
        return m1;
    
    return vfx_matrix_float4x4_make_affine_rh(simd_mix(t1, t2, alpha),
                                              vfx_quat_slerp(q1, q2, alpha),
                                              simd_mix(s1, s2, alpha));
}
#endif

static inline simd_float3x3 vfx_make_rotation(simd_float3 axis, float angle) {
    axis = vfx_robust_normalize(axis);
    float cs = vfx_cos(angle); float sn = vfx_sin(angle);
    simd_float3 a2 = axis * axis;
    simd_float3 d = (1.f - a2) * cs + a2;
    float ic = 1 - cs;
    float xy = axis.x * axis.y * ic;
    float xz = axis.x * axis.z * ic;
    float yz = axis.y * axis.z * ic;
    simd_float3 as = axis * sn;
    return vfx_float3x3(vfx_make_float3(d.x,  xy - as.z,   xz + as.y),
                        vfx_make_float3(xy + as.z, d.y,    yz - as.x),
                        vfx_make_float3(xz - as.y, yz + as.x,   d.z));
}

static inline simd_float3x3 VFX_SIMD vfx_make_rotation_x(float angle) {
    float cs = vfx_cos(angle); float sn = vfx_sin(angle);
    return vfx_float3x3(vfx_make_float3(1,  0,   0),
                        vfx_make_float3(0, cs, -sn),
                        vfx_make_float3(0, sn,  cs));
}

static inline simd_float3x3 VFX_SIMD vfx_make_rotation_y(float angle) {
    float cs = vfx_cos(angle); float sn = vfx_sin(angle);
    return vfx_float3x3(vfx_make_float3( cs, 0, sn),
                        vfx_make_float3(  0, 1,  0),
                        vfx_make_float3(-sn, 0, cs));
}

static inline simd_float3x3 VFX_SIMD vfx_make_rotation_z(float angle) {
    float cs = vfx_cos(angle); float sn = vfx_sin(angle);
    return vfx_float3x3(vfx_make_float3(cs, -sn, 0),
                        vfx_make_float3(sn,  cs, 0),
                        vfx_make_float3(0,   0,  1));
}

#pragma mark - Masks

#ifndef __METAL_VERSION__
// Creates a 4-bit mask from the most significant bits of the four 32 bits integer values
static inline VFX_SIMD int vfx_reduce_mask(simd_int4 inMask)
{
#if defined(__SSE__)
  return _mm_movemask_ps(inMask);
#else
  return simd_reduce_add((inMask < 0) & (simd_int4){ 0x1, 0x2, 0x4, 0x8 });
#endif
}
#endif

// Should move to vfx_random.h

static inline uint64_t VFX_SIMD vfx_hash_combine64(uint64_t hash, uint64_t v) {
    return hash ^ (v + 0x9e3779b97f4a7c15 + (hash << 6) + (hash >> 2));
}

#if SIZE_MAX == UINT64_MAX
static inline uint64_t VFX_SIMD vfx_hash_combine(uint64_t hash, uint32_t v) {
    return vfx_hash_combine64(hash, (uint64_t)v);
}
#else
static inline uint64_t VFX_SIMD vfx_hash_combine(uint64_t hash, uint64_t v) {
    return vfx_hash_combine64(hash, v);
}
#endif

static inline uint64_t VFX_SIMD vfx_hash_combine(uint64_t hash, size_t v) { return vfx_hash_combine64(hash, (uint64_t)v); } // Defined for translation to swift Int
static inline uint64_t VFX_SIMD vfx_hash_combine(uint64_t hash, int32_t v) { return vfx_hash_combine64(hash, (uint64_t)v); }
static inline uint64_t VFX_SIMD vfx_hash_combine(uint64_t hash, int64_t v) { return vfx_hash_combine64(hash, (uint64_t)v); }
static inline uint64_t VFX_SIMD vfx_hash_combine(uint64_t hash, int8_t v)  { return vfx_hash_combine64(hash, (uint64_t)v); }
static inline uint64_t VFX_SIMD vfx_hash_combine(uint64_t hash, uint8_t v) { return vfx_hash_combine64(hash, (uint64_t)v); }


// hash / random

// https://www.pcg-random.org/
static inline uint32_t VFX_SIMD vfx_hash_pcg(uint32_t v) {
    uint32_t state = v * 747796405u + 2891336453u;
    uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

static inline float vfx_random01(uint32_t index) {
    // this should start at the seed and be incremented at each generation
    // this will not take into account the number of intermediary rand() we ask
    // index could be initialized to gen_count + index * 100 and passed as &
    // to random01 which will increment it
    uint32_t u = 0x7Fu << 23 | vfx_hash_pcg(index) >> 9;
    // convert int32 to float in [0..1]
    // only generates all dyadic rationals of the form k / 2^-23 equally
#ifdef __METAL_VERSION__
    return as_type<float>(u) - 1.f;
#else
    return *((float *)(&u)) - 1.f;
#endif
}

static inline simd_float2 vfx_random01_float2(uint32_t index) {
    return vfx_make_float2(vfx_random01(index), vfx_random01(index + 1));
}
static inline simd_float3 vfx_random01_float3(uint32_t index) {
    return vfx_make_float3(vfx_random01(index), vfx_random01(index + 1), vfx_random01(index + 2));
}
static inline simd_float4 vfx_random01_float4(uint32_t index) {
    return vfx_make_float4(vfx_random01(index), vfx_random01(index + 1), vfx_random01(index + 2), vfx_random01(index + 3));
}

static inline vfx_int_range VFX_SIMD vfx_make_int_range(size_t lo, size_t hi) { return (vfx_int_range){ lo, hi }; }
static inline vfx_float_range VFX_SIMD vfx_make_float_range(float lo, float hi) { return (vfx_float_range){ lo, hi }; }

static inline bool vfx_range_is_empty(vfx_float_range range) {
    return range.lower_bound == range.upper_bound;
}

// t must be [0..1]
static inline float vfx_range_interpolate(vfx_float_range range, float t) {
    return range.lower_bound + (range.upper_bound - range.lower_bound) * t;
}

// scale bias

static inline vfx_scale_bias VFX_SIMD vfx_make_scale_bias(float start, float end) { return (vfx_scale_bias){ 1.f / (end - start), -start / (end - start) }; }
static inline vfx_scale_bias VFX_SIMD vfx_make_scale_bias(vfx_float_range range) { return vfx_make_scale_bias(range.lower_bound, range.upper_bound); }
static inline float VFX_SIMD vfx_apply(float x, vfx_scale_bias scale_bias) { return vfx_muladd(x, scale_bias.scale, scale_bias.bias); }
static inline float VFX_SIMD vfx_apply_saturate(float x, vfx_scale_bias scale_bias) { return vfx_saturate(vfx_muladd(x, scale_bias.scale, scale_bias.bias)); }

#undef simd_bitselect

#ifndef __METAL_VERSION__

// These are dummy functions only there to cope with bad Swift debug code

static inline simd_float4 VFX_SIMD vfx_add(simd_float4 a, simd_float4 b) { return a + b; }
static inline simd_float3 VFX_SIMD vfx_add(simd_float3 a, simd_float3 b) { return a + b; }
static inline simd_float2 VFX_SIMD vfx_add(simd_float2 a, simd_float2 b) { return a + b; }
static inline simd_float4 VFX_SIMD vfx_add(simd_float4 a, float b)       { return a + b; }
static inline simd_float3 VFX_SIMD vfx_add(simd_float3 a, float b)       { return a + b; }
static inline simd_float2 VFX_SIMD vfx_add(simd_float2 a, float b)       { return a + b; }
static inline simd_float4 VFX_SIMD vfx_add(float a, simd_float4 b)       { return a + b; }
static inline simd_float3 VFX_SIMD vfx_add(float a, simd_float3 b)       { return a + b; }
static inline simd_float2 VFX_SIMD vfx_add(float a, simd_float2 b)       { return a + b; }

static inline simd_float4 VFX_SIMD vfx_sub(simd_float4 a, simd_float4 b) { return a - b; }
static inline simd_float3 VFX_SIMD vfx_sub(simd_float3 a, simd_float3 b) { return a - b; }
static inline simd_float2 VFX_SIMD vfx_sub(simd_float2 a, simd_float2 b) { return a - b; }
static inline simd_float4 VFX_SIMD vfx_sub(simd_float4 a, float b)       { return a - b; }
static inline simd_float3 VFX_SIMD vfx_sub(simd_float3 a, float b)       { return a - b; }
static inline simd_float2 VFX_SIMD vfx_sub(simd_float2 a, float b)       { return a - b; }
static inline simd_float4 VFX_SIMD vfx_sub(float a, simd_float4 b)       { return a - b; }
static inline simd_float3 VFX_SIMD vfx_sub(float a, simd_float3 b)       { return a - b; }
static inline simd_float2 VFX_SIMD vfx_sub(float a, simd_float2 b)       { return a - b; }

static inline simd_float4 VFX_SIMD vfx_mul(simd_float4 a, simd_float4 b) { return a * b; }
static inline simd_float3 VFX_SIMD vfx_mul(simd_float3 a, simd_float3 b) { return a * b; }
static inline simd_float2 VFX_SIMD vfx_mul(simd_float2 a, simd_float2 b) { return a * b; }
static inline simd_float4 VFX_SIMD vfx_mul(simd_float4 a, float b)       { return a * b; }
static inline simd_float3 VFX_SIMD vfx_mul(simd_float3 a, float b)       { return a * b; }
static inline simd_float2 VFX_SIMD vfx_mul(simd_float2 a, float b)       { return a * b; }
static inline simd_float4 VFX_SIMD vfx_mul(float a, simd_float4 b)       { return a * b; }
static inline simd_float3 VFX_SIMD vfx_mul(float a, simd_float3 b)       { return a * b; }
static inline simd_float2 VFX_SIMD vfx_mul(float a, simd_float2 b)       { return a * b; }

static inline simd_float4 VFX_SIMD vfx_div(simd_float4 a, simd_float4 b) { return a / b; }
static inline simd_float3 VFX_SIMD vfx_div(simd_float3 a, simd_float3 b) { return a / b; }
static inline simd_float2 VFX_SIMD vfx_div(simd_float2 a, simd_float2 b) { return a / b; }
static inline simd_float4 VFX_SIMD vfx_div(simd_float4 a, float b)       { return a / b; }
static inline simd_float3 VFX_SIMD vfx_div(simd_float3 a, float b)       { return a / b; }
static inline simd_float2 VFX_SIMD vfx_div(simd_float2 a, float b)       { return a / b; }
static inline simd_float4 VFX_SIMD vfx_div(float a, simd_float4 b)       { return a / b; }
static inline simd_float3 VFX_SIMD vfx_div(float a, simd_float3 b)       { return a / b; }
static inline simd_float2 VFX_SIMD vfx_div(float a, simd_float2 b)       { return a / b; }

#endif

#pragma mark - Color helpers

// linear / srgb conversion

static inline float VFX_SIMD vfx_srgb_to_grayscale(simd_float4 v) {
    return 0.3f * v.x + 0.59f * v.y + 0.11f * v.z;
}

static inline simd_float3 VFX_SIMD vfx_srgb_to_linear(simd_float3 srgb) {
    return vfx_select(vfx_pow((srgb + 0.055f) / 1.055f, 2.4f), srgb / 12.92f, srgb <= 0.04045f);
}

static inline simd_float3 VFX_SIMD vfx_linear_to_srgb(simd_float3 linear) {
    return vfx_select(1.055f * vfx_pow(linear, 1.f / 2.4f) - 0.055f, linear * 12.92f, linear <= 0.0031308f);
}

#ifdef __METAL_VERSION__ // half3 metal only versions

static inline half3 VFX_SIMD vfx_srgb_to_linear(half3 srgb) {
    return vfx_select(vfx_pow((srgb + 0.055h) / 1.055h, 2.4h), srgb / 12.92h, srgb <= 0.04045h);
}

static inline half3 VFX_SIMD vfx_linear_to_srgb(half3 linear) {
    return vfx_select(1.055h * vfx_pow(linear, 1.h / 2.4h) - 0.055h, linear * 12.92h, linear <= 0.0031308h);
}

#endif

// hsb

static inline simd_float4 VFX_SIMD vfx_rgb_to_hsb(simd_float4 rgba) {
    float min = vfx_reduce_min(rgba.rgb);
    float max = vfx_reduce_max(rgba.rgb);
    float delta = max - min;
    float hue = 0;
    if (delta != 0) {
        if (rgba.r == max) {
            hue = (rgba.g - rgba.b) / delta;
        } else if (rgba.g == max) {
            hue = 2 + (rgba.b - rgba.r) / delta;
        } else { // color.b == max
            hue = 4 + (rgba.r - rgba.g) / delta;
        }
        hue = (hue < 0) ? (hue * 0.1666666666f + 1.f) : hue * 0.1666666666f;
    }
    float brightness = max;
    float saturation = max != 0 ? (max - min) / max : 0;
    return (simd_float4){ hue, saturation, brightness, rgba.a };
}

static inline simd_float3 VFX_SIMD vfx_rgb_to_hsb(simd_float3 rgb) {
    return vfx_rgb_to_hsb(vfx_make_float4_undef(rgb)).xyz;
}

static inline simd_float4 VFX_SIMD vfx_hsb_to_rgb(simd_float4 hsba) {
    simd_float3 hsb = vfx_saturate(hsba.rgb);
    float hue = ((hsb.x == 1.f) ? 0.f : hsb.x) * 6.f;
    float saturation = hsb.y;
    float brightness = hsb.z;

    if (saturation == 0) {
        return vfx_make_float4(hsb.zzz, hsba.a);
    }

    int i = (int)hue;
    float f = hue - i;
    simd_float4 bpqt = {
        brightness,
        brightness * (1 - saturation),
        brightness * (1 - saturation * f),
        brightness * (1 - saturation * (1 - f))
    };
    switch (i) {
        case 0: return vfx_make_float4(bpqt.xwy, hsba.a); break;
        case 1: return vfx_make_float4(bpqt.zxy, hsba.a); break;
        case 2: return vfx_make_float4(bpqt.yxw, hsba.a); break;
        case 3: return vfx_make_float4(bpqt.yzx, hsba.a); break;
        case 4: return vfx_make_float4(bpqt.wyx, hsba.a); break;
        default:return vfx_make_float4(bpqt.xyz, hsba.a); break;
    }
}

static inline simd_float3 VFX_SIMD vfx_hsb_to_rgb(simd_float3 rgb) {
    return vfx_hsb_to_rgb(vfx_make_float4_undef(rgb)).xyz;
}


// legacy, kept for backward compatibility

#ifndef __METAL_VERSION__
static inline simd_quatf VFX_SIMD vfx_quat_invert(simd_quatf q) { return vfx_quat_normalize(vfx_quat_conjugate(q)); }
#endif

NS_ASSUME_NONNULL_END
// MARK: - vfx_texture.h


// This should go in vfx_base.h but can't because of module issues?
#ifdef __METAL_VERSION__
#define vfx_binding_index int32_t

static simd_float2 vfx_sample_gradient_x(texture2d<half> texture, sampler smp, simd_float2 uv)
{
    const float kStep = 0.01f;
    const float2 xStep = float2(kStep, 0);
    const float2 yStep = float2(0, kStep);
    return float2(
        texture.sample(smp, uv + xStep).x - texture.sample(smp, uv - xStep).x,
        texture.sample(smp, uv + yStep).x - texture.sample(smp, uv - yStep).x
    );
}

static simd_float2 vfx_sample_gradient_x(texture2d<float> texture, sampler smp, simd_float2 uv)
{
    const float kStep = 0.01f;
    const float2 xStep = float2(kStep, 0);
    const float2 yStep = float2(0, kStep);
    return float2(
        texture.sample(smp, uv + xStep).x - texture.sample(smp, uv - xStep).x,
        texture.sample(smp, uv + yStep).x - texture.sample(smp, uv - yStep).x
    );
}

// Used in scripting and generated code
struct sampler1d {
    const texture1d<float> texture;
    const sampler sampler;
};

struct sampler2d {
    const texture2d<float> texture;
    const sampler sampler;
};

struct sampler3d {
    const texture3d<float> texture;
    const sampler sampler;
};

#else
// Swift import needs to be converted to Int for convenience of use
#define vfx_binding_index size_t

#define VFX_ENUM(type) enum __attribute__((enum_extensibility(closed))) : type

NS_ASSUME_NONNULL_BEGIN


typedef enum {
    pixel_format_r16f,
    pixel_format_r32f,
    pixel_format_rgba16f,
    pixel_format_rgba32f,
    pixel_format_r8unorm,
    pixel_format_r8unorm_srgb,
    pixel_format_rgba8unorm,
    pixel_format_rgba8unorm_srgb,
    pixel_format_bgra8unorm,
    pixel_format_bgra8unorm_srgb,
    pixel_format_count
} pixel_format;

// Also see extension in Metal+Extensions
typedef VFX_ENUM(int8_t) {
    AddressMode_clampToZero,
    AddressMode_clampToEdge,
    AddressMode_repeat,
    AddressMode_repeatMirror,
    AddressMode_count
} AddressMode;

// Also see extension in Metal+Extensions
typedef VFX_ENUM(int8_t) {
    FilterMode_nearest,
    FilterMode_linear,
    FilterMode_count
} FilterMode;

// Sampler3D

struct __vfx_sampler3d;
typedef simd_float4 (*sample_3d_func)(const struct __vfx_sampler3d* sampler, simd_float3 uvw);

typedef struct __vfx_sampler3d {
    simd_float3                 dimensions;
    simd_int3                   edges;
    simd_int3                   cellStrideBytes;
    simd_int3                   outsideRepeatStrideBytes;
    void* _Nullable             data;
    sample_3d_func _Nullable    sample_func;
} vfx_sampler3d;

VFX_EXPORT_C vfx_sampler3d vfx_sampler3d_make(void* data, simd_int3 dimensions, pixel_format format, AddressMode address, FilterMode filter);

static inline VFX_SIMD_UNPURE simd_float4 sample_3d(const vfx_sampler3d* sampler, simd_float3 uvw) {
    return sampler->sample_func(sampler, uvw);
}

static inline VFX_SIMD_UNPURE simd_float3 sample_3d_gradient_x(const vfx_sampler3d* sampler, simd_float3 uvw) {
    float kStep = 0.01f;
    simd_float3 xStep = (simd_float3){ kStep, 0, 0 };
    simd_float3 yStep = (simd_float3){ 0, kStep, 0 };
    simd_float3 zStep = (simd_float3){ 0, 0, kStep };
    return (simd_float3){
        sample_3d(sampler, uvw + xStep).x - sample_3d(sampler, uvw - xStep).x,
        sample_3d(sampler, uvw + yStep).x - sample_3d(sampler, uvw - yStep).x,
        sample_3d(sampler, uvw + zStep).x - sample_3d(sampler, uvw - zStep).x
    };
}

// vfx_sampler2d

struct __vfx_sampler2d;
typedef simd_float4 (*sample_2d_func)(const struct __vfx_sampler2d* sampler, simd_float2 uv);

typedef struct __vfx_sampler2d {
    simd_float2                 dimensions;
    simd_int2                   edges;
    simd_int2                   cellStrideBytes;
    simd_int2                   outsideRepeatStrideBytes;
    void*                       data;
    sample_2d_func _Nullable    sample_func;
} vfx_sampler2d;

VFX_EXPORT_C vfx_sampler2d vfx_sampler2d_make(void* data, simd_int2 dimensions, pixel_format format, AddressMode address, FilterMode filter);

static inline VFX_SIMD_UNPURE simd_float4 sample_2d(const vfx_sampler2d* sampler, simd_float2 uv) {
    return sampler->sample_func(sampler, uv);
}

static inline VFX_SIMD_UNPURE simd_float2 sample_2d_gradient_x(const vfx_sampler2d* sampler, simd_float2 uv) {
    float kStep = 0.01f;
    simd_float2 xStep = (simd_float2){ kStep, 0 };
    simd_float2 yStep = (simd_float2){ 0, kStep };
    return (simd_float2){
        sample_2d(sampler, uv + xStep).x - sample_2d(sampler, uv - xStep).x,
        sample_2d(sampler, uv + yStep).x - sample_2d(sampler, uv - yStep).x,
    };
}

// vfx_sampler1d

struct __vfx_sampler1d;
typedef simd_float4 (*sample_1d_func)(const struct __vfx_sampler1d* sampler, float uv);

typedef struct __vfx_sampler1d {
    float           dimensions;
    int32_t         edges;
    int32_t         cellStrideBytes;
    int32_t         outsideRepeatStrideBytes;
    void*           data;
    sample_1d_func _Nullable sample_func;
} vfx_sampler1d;

VFX_EXPORT_C vfx_sampler1d vfx_sampler1d_make(void* data, int32_t dimension, pixel_format format, AddressMode address, FilterMode filter);

static inline VFX_SIMD_UNPURE simd_float4 sample_1d(const vfx_sampler1d* sampler, float uv) {
    return sampler->sample_func(sampler, uv);
}

NS_ASSUME_NONNULL_END

#endif
// MARK: - vfx_shared.h


// This should go in vfx_base.h but can't because of module issues?
#ifdef __METAL_VERSION__
#define vfx_binding_index int32_t
#else
// Swift import needs to be converted to Int for convenience of use
#define vfx_binding_index size_t
#endif

#define VFX_ENUM(type) enum __attribute__((enum_extensibility(closed))) : type
#define VFX_BINDING_ENUM VFX_ENUM(vfx_binding_index)

typedef VFX_BINDING_ENUM
{
    // buffers
    vertex_frame_uniforms      = 0,
    vertex_material_uniforms   = 3,

    data_positions             = 4,
    data_colors                = 5,
    data_velocities            = 6,
    data_sizes                 = 7,
    data_orientations          = 8,
    data_sort_order            = 9,
    data_ages                  = 10,
    data_frames                = 11,
    data_angles                = 12,
    data_pivots                = 13,
    data_targets               = 14,
    data_masses                = 15,
    data_userData1s            = 16,
    data_userData2s            = 17,
    data_userData3s            = 18,
    data_userData4s            = 19,
    data_lifetimes             = 20,
    data_ribbon_lengths        = 25,
    data_angular_velocities    = 26,
    data_angle_velocities      = 27,
    data_linear_factors        = 28,
    data_angular_factors       = 29,
    data_particleID            = 30,
    data_neighborCounts        = 31,

    // New particle data binding index
    data_counters              = 0,
    particle_header            = 4,
    particle_datas             = 5,

    // only for compute
    particle_count             = 21,

    // because we use particle_vertex_arg in voxelization, we need to reserve an index for the special proj
    vertex_voxel_matrix        = 22,
    vertex_voxel_uniforms      = 23,
    vertex_voxel_data          = 24,

    // textures
    vertex_irradiance_tex      = 1,
    vertex_voxel_tex           = 2,

    // Fragment

    // samplers
    fragment_sampler           = 0,

    // textures
    fragment_texture           = 0,
    fragment_linearZ           = 1,
    fragment_gi_tex            = 2,
    fragment_opaque_zbuffer    = 9,
    fragment_normal_buffer     = 10,


    fragment_texture_prebaked_lighting_posXYZ    = 11,
    fragment_texture_prebaked_lighting_negXYZ    = 12,

    fragment_texture_blue_noise_dithering = 13,

    // buffers
    fragment_frame_uniforms    = 0,
    fragment_material_uniforms = 1,
    fragment_voxel_uniforms    = 2,

    fragment_voxel_data        = 3,
    fragment_lights_uniforms    = 4,
    fragment_ibl_uniforms = 5,
    fragment_clipping_uniforms = 6
} ParticleBufferIndex;

typedef VFX_ENUM(int32_t)
{
    renderer_constants_gamma_blending = 450,
    renderer_constants_inside_re = 451,
    renderer_constants_enable_clipping = 452,
    renderer_constants_enable_dither_fade = 453,
    renderer_constants_render_to_composite_layer = 454,
    renderer_constants_enable_alpha_fade = 455
} renderer_constants;

// DO NOT CHANGE THESE INDICES SINCE THEY CAN BE EXPORTED IN CUSTOM SHADERS
typedef VFX_ENUM(int32_t)
{
    // particle_constants_has_position,
    // particle_constants_has_linear_velocity,

    particle_constants_has_velocity         = 500,  // Not stating at 0: rdar://76640645 (VFX custom material compilation issue from added constant SupportsCubeArray and EnableSamplerArray)

    particle_constants_has_age              = 501,

    particle_constants_has_lifetime         = 502,
    particle_constants_uniform_lifetime     = 503,

    particle_constants_has_color            = 504,
    particle_constants_has_transient_color  = 505,

    particle_constants_has_orientation      = 506,
    particle_constants_has_angular_velocity = 507,

    // TODO merge with orientation / angular_velocity
    particle_constants_has_angle            = 508,
    particle_constants_has_angle_velocity   = 509,

    particle_constants_has_pivot            = 510,
    particle_constants_has_target           = 511,
    particle_constants_has_mass             = 512,
    particle_constants_has_texture_frame    = 513,

    particle_constants_has_linear_factor    = 514,

    particle_constants_has_angular_factor   = 515,

    particle_constants_has_ribbon_length    = 516,

    particle_constants_has_user_data1       = 517,
    particle_constants_has_user_data2       = 518,
    particle_constants_has_user_data3       = 519,
    particle_constants_has_user_data4       = 520,

    particle_constants_has_size1D           = 521,
    particle_constants_has_size2D           = 522,
    particle_constants_has_size3D           = 523,
    particle_constants_has_uniform_size     = 524,
    particle_constants_has_transient_size   = 525,

    particle_constants_is_local             = 526,

    particle_constants_has_particle_id      = 527,

    //
    // Update attribute specific

    particle_constants_size_over_life_mode, // 0: planar / 1: planar / 2: free

    particle_constants_drag_use_size,

    particle_constants_attach_use_over_life,

    particle_constants_noise_use_transform,

    particle_constants_plane_collider_infinite,

    particle_constants_enable_gravity,

    particle_constants_box_collider_inside,

    particle_constants_force_field_kind,

    //
    // Init attribute specific

    particle_constants_emitter_shape_type, // 0: point, 1: sphere, 2: box, 3: cylinder, 4: plane, 5: torus, 6: cone
    particle_constants_emitter_shape_distribution, // 0: random, 1: grid, 2: uniform
    particle_constants_emitter_shape_orientation, // -1 no orientation otherwise same as direction
    particle_constants_emitter_shape_direction, // 0: world, 1: local, 2: shape, 3: motion, 4: random
    particle_constants_emitter_shape_has_texture_2D,

    particle_constants_emitter_pointcache_inherit_position,
    particle_constants_emitter_pointcache_inherit_color,
    particle_constants_emitter_pointcache_inherit_velocity,
    particle_constants_emitter_pointcache_inherit_orientation,
    particle_constants_emitter_pointcache_inherit_size,

    particle_constants_emitter_pointcache_has_velocity,
    particle_constants_emitter_pointcache_has_transform,

    particle_constants_set_color_mode,

    // TODO Render specific, should be moved
    particle_constants_render_blending_mode = 600,
    particle_constants_render_enable_texture_2d = 601,
    particle_constants_render_enable_texture_3d = 602,
    particle_constants_render_enable_texture_2d_array = 603,
    particle_constants_render_enable_texture_cube = 604,
    particle_constants_render_enable_texture_prelighted = 605,
    particle_constants_render_texture_is_not_alphapremultiplied = 606,
    particle_constants_render_texture_channel = 607,
    particle_constants_render_local_transform_LEGACY = 608,
    particle_constants_render_has_particle_transform = 609,
    particle_constants_render_has_pbr_lighting = 610,
    particle_constants_render_has_pbr_lighting_specular = 611,
    particle_constants_render_animation_mode = 612,
    particle_constants_render_inter_frame_interpolation = 613,
    particle_constants_render_enable_scattering = 614,
    particle_constants_render_orientation_mode = 615,
    particle_constants_render_shape_mode = 616,
    particle_constants_render_enable_velocity_stretch = 617,
    particle_constants_render_enable_cutoff = 618,
    particle_constants_render_enable_sorting = 619,
    particle_constants_render_enable_thick_lines = 620,
    particle_constants_render_enable_soft_particles,
    particle_constants_render_need_opaque_zbuffer,
    particle_constants_render_need_normal_buffer,
    particle_constants_render_voxelDataChannelCount,
    particle_constants_render_voxelizeOpacity = 625,
    particle_constants_render_voxelizeColor

} particle_constants;

typedef struct {
    int32_t offset;
    int16_t type;
    int16_t stride;
} particle_data_description;

struct particle_data_header {
    particle_data_description descriptions[data_neighborCounts]; // TODO have a data_max_count
};

#ifdef __METAL_VERSION__

// consider these always there...
// constant bool has_position          [[ function_constant(particle_constants_has_position) ]];
// constant bool has_linear_velocity   [[ function_constant(particle_constants_has_linear_velocity) ]];

constant bool has_velocity              [[ function_constant(particle_constants_has_velocity) ]];

constant bool has_age                   [[ function_constant(particle_constants_has_age) ]];

constant bool has_lifetime              [[ function_constant(particle_constants_has_lifetime) ]];

constant bool has_color                 [[ function_constant(particle_constants_has_color) ]];
constant bool has_transient_color       [[ function_constant(particle_constants_has_transient_color) ]];

constant bool has_orientation           [[ function_constant(particle_constants_has_orientation) ]];
constant bool has_angular_velocity      [[ function_constant(particle_constants_has_angular_velocity) ]];

constant bool has_angle                 [[ function_constant(particle_constants_has_angle) ]];
constant bool has_angle_velocity        [[ function_constant(particle_constants_has_angle_velocity) ]];

constant bool has_texture_frame         [[ function_constant(particle_constants_has_texture_frame) ]];

constant bool has_particle_id            [[ function_constant(particle_constants_has_particle_id) ]];

constant bool has_user_data1            [[ function_constant(particle_constants_has_user_data1) ]];
constant bool has_user_data2            [[ function_constant(particle_constants_has_user_data2) ]];
constant bool has_user_data3            [[ function_constant(particle_constants_has_user_data3) ]];
constant bool has_user_data4            [[ function_constant(particle_constants_has_user_data4) ]];

constant bool has_size1D                [[ function_constant(particle_constants_has_size1D) ]];
constant bool has_size2D                [[ function_constant(particle_constants_has_size2D) ]];
constant bool has_size3D                [[ function_constant(particle_constants_has_size3D) ]];

constant bool has_transient_size        [[ function_constant(particle_constants_has_transient_size) ]];

constant bool has_linear_factor         [[ function_constant(particle_constants_has_linear_factor) ]];

constant bool has_angular_factor        [[ function_constant(particle_constants_has_angular_factor) ]];

constant bool has_ribbon_length         [[ function_constant(particle_constants_has_ribbon_length) ]];

constant bool has_pivot                 [[ function_constant(particle_constants_has_pivot) ]];

constant bool has_target                [[ function_constant(particle_constants_has_target) ]];

constant bool has_mass                  [[ function_constant(particle_constants_has_mass) ]];

constant bool is_local                  [[ function_constant(particle_constants_is_local) ]];


// ⚠️ any change here might break already compiled script metallibs as this is used by particle_script_argument_buffer
struct particle_counters {
    // Storage must match ConstantBufferOffsets
    // TODO we may don't need atomic here !!!
    atomic_int  active_count;    // 0
    atomic_int  dead_count;     // 4
    atomic_int  live_count;     // 8
    uint  added_count;          // 12
    // used for randoms. could also contain the seed?
    uint  generated_count;      // 16

    int get_active_count() device {
        return atomic_load_explicit(&active_count, memory_order_relaxed);
    }
    
    void set_active_count(int v) device {
        atomic_store_explicit(&active_count, v, memory_order_relaxed);
    }

    int get_live_count() device {
        return atomic_load_explicit(&live_count, memory_order_relaxed);
    }

    void set_live_count(int v) device {
        atomic_store_explicit(&live_count, v, memory_order_relaxed);
    }

    void set_dead_count(int v) device {
        atomic_store_explicit(&dead_count, v, memory_order_relaxed);
    }

    int get_added_count() device {
        return added_count; // Do we need atomicity?
    }
    
    void set_added_count(int v) device {
        added_count = v; // Do we need atomicity?
    }

    int get_generated_count() device {
        return generated_count; // Do we need atomicity?
    }

    void add_generated_count(int v) device {
        generated_count += v; // Do we need atomicity?
    }

    int32_t increment_live_count() device {
        return atomic_fetch_add_explicit(&live_count, 1, memory_order_relaxed);
    }

    int32_t increment_dead_count() device {
        return atomic_fetch_add_explicit(&dead_count, 1, memory_order_relaxed);
    }

    bool is_outside(uint index) device {
        uint particleCount = atomic_load_explicit(&active_count, memory_order_relaxed);
        return index >= particleCount;
    }
    
    uint index_from_added(uint index) device {
        uint particleCount = atomic_load_explicit(&active_count, memory_order_relaxed);
        // Optim store!
        uint added_start = particleCount - added_count;
        return index + added_start;
    }

    bool newly_created_is_outside(thread uint* index) device {
        if (*index >= added_count) return true;
        // offset the index to be in the added area
        uint particleCount = atomic_load_explicit(&active_count, memory_order_relaxed);
        // Optim store!
        uint added_start = particleCount - added_count;
        *index += added_start;
        return false;
    }

    // Random
    
    float random01(uint index) device {
        // this should start at the seed and be incremented at each generation
        // this will not take into account the number of intermediary rand() we ask
        // index could be initialized to gen_count + index * 100 and passed as &
        // to random01 which will increment it
        uint32_t u = vfx_hash_pcg(index + generated_count);
        // convert int32 to float in [0..1]
        // only generates all dyadic rationals of the form k / 2^-23 equally
        return as_type<float>(0x7Fu << 23 | u >> 9) - 1.;
    }
    
    float2 random_float2_01(uint index) device { return vfx_make_float2(random01(index), random01(index + 1)); }
    float3 random_float3_01(uint index) device { return vfx_make_float3(random01(index), random01(index + 1), random01(index + 2)); }

    float random_float_in_range(uint index, vfx_float_range range) device {
        if (vfx_range_is_empty(range)) {
            return range.lower_bound;
        } else {
            return vfx_range_interpolate(range, random01(index));
        }
    }

    simd_float2 random_float2_in_range(uint index, vfx_float_range xRange, vfx_float_range yRange) device {
        return (simd_float2){
            random_float_in_range(index * 2 + 0, xRange),
            random_float_in_range(index * 2 + 1, yRange)
        };
    }

    simd_float3 random_float3_in_range(uint index, vfx_float_range xRange, vfx_float_range yRange, vfx_float_range zRange) device {
        return (simd_float3){
            random_float_in_range(index * 3 + 0, xRange),
            random_float_in_range(index * 3 + 1, yRange),
            random_float_in_range(index * 3 + 2, zRange)
        };
    }

};

// Default values (also used when the data is missing)

#define DEFAULT_POSITION float3(0.f)
#define DEFAULT_VELOCITY float3(0.f)
#define DEFAULT_AGE 0.f
#define DEFAULT_LIFETIME 1.f
#define DEFAULT_COLOR float4(1.f)
#define DEFAULT_ORIENTATION vfx_float4_unit_w()
#define DEFAULT_ANGULAR_VELOCITY 0.f
#define DEFAULT_ANGLE 0.f
#define DEFAULT_ANGLE_VELOCITY 0.f
#define DEFAULT_TEXTURE_FRAME 0.f
#define DEFAULT_SIZE 1.f
#define DEFAULT_LINEAR_FACTOR 1.f
#define DEFAULT_ANGULAR_FACTOR 1.f
#define DEFAULT_PIVOT 0x80808080
#define DEFAULT_TARGET 0.f
#define DEFAULT_MASS 1.f

#if USE_PARTICLE_MONODATA

#define particle_argument_buffer particle_data_args

#else

struct particle_argument_buffer {

private:
    // Not used anymore but kept for export legacy
    device int* legacy_partition            [[ id(0) ]];

    // We could also avec one counts buffer and use offsets in it ?
    device particle_counters* counters      [[ id(1) ]];

    device simd_float3* positions           [[ id(data_positions) ]];
    device simd_float4* colors              [[ id(data_colors) ]];

    // OPTIM : the non varying versions should be constant ????
    device simd_float3* velocities          [[ id(data_velocities) ]];

    // sizes can be 1D, 2D or 3D depending on the function constant
    device float* sizes                     [[ id(data_sizes) ]];

    device simd_float4* orientations        [[ id(data_orientations) ]];
    device uint32_t* order                  [[ id(data_sort_order) ]];
    device float* ages                      [[ id(data_ages) ]];
    device float* frames                    [[ id(data_frames) ]];
    device float* angles                    [[ id(data_angles) ]];
    device uint32_t* pivots                 [[ id(data_pivots) ]];
    device simd_float3* targets             [[ id(data_targets) ]];
    device float* masses                    [[ id(data_masses) ]];
    device simd_float4* user_data1          [[ id(data_userData1s) ]];
    device simd_float4* user_data2          [[ id(data_userData2s) ]];
    device simd_float4* user_data3          [[ id(data_userData3s) ]];
    device simd_float4* user_data4          [[ id(data_userData4s) ]];
    device float* lifetimes                 [[ id(data_lifetimes) ]];

    device float* ribbon_lengths            [[ id(data_ribbon_lengths) ]];
    device simd_float4* angular_velocities  [[ id(data_angular_velocities) ]];
    device float* angle_velocities          [[ id(data_angle_velocities) ]];
    device simd_float3* linear_factors      [[ id(data_linear_factors) ]];
    device simd_float3* angular_factors     [[ id(data_angular_factors) ]];
    device uint32_t* particle_id            [[ id(data_particleID) ]];
    device float* neighborCounts            [[ id(data_neighborCounts) ]];

public:
    
    // If we don't want to duplicate all the APIs, use counters directly
    // device particle_counters* get_counters() constant { return _counters; }
    
    // Counters
    
    int get_active_count() constant { return counters->get_active_count(); }
    void set_active_count(int v) constant { counters->set_active_count(v); }
    
    int get_live_count() constant { return counters->get_live_count(); }
    void set_live_count(int v) constant { counters->set_live_count(v); }
    
    int get_added_count() constant { return counters->get_added_count(); }
    void set_added_count(int v) constant { counters->set_added_count(v); }

    void set_dead_count(int v) constant { counters->set_dead_count(v); }

    int get_generated_count() constant { return counters->get_generated_count(); }
    void add_generated_count(int v) constant { counters->add_generated_count(v); }

    int32_t increment_live_count() constant { return counters->increment_live_count(); }
    int32_t increment_dead_count() constant { return counters->increment_dead_count(); }
    
    bool is_outside(uint index) constant { return counters->is_outside(index); }
    uint index_from_added(uint index) constant { return counters->index_from_added(index); }
    bool newly_created_is_outside(thread uint* index) constant { return counters->newly_created_is_outside(index); }

    // Random
    
    float random01(uint index) constant { return counters->random01(index); }
    float2 random_float2_01(uint index) constant { return counters->random_float2_01(index); }
    float3 random_float3_01(uint index) constant { return counters->random_float3_01(index); }

    float random_float_in_range(uint index, vfx_float_range range) constant { return counters->random_float_in_range(index, range); }
    float2 random_float2_in_range(uint index, vfx_float_range xRange, vfx_float_range yRange) constant { return counters->random_float2_in_range(index, xRange, yRange); }
    float3 random_float3_in_range(uint index, vfx_float_range xRange, vfx_float_range yRange, vfx_float_range zRange) constant { return counters->random_float3_in_range(index, xRange, yRange, zRange); }

    // Data access
    
    float3 get_position(int pid) constant {
        return positions[pid];
    }

    void set_position(int pid, float3 v) constant {
        positions[pid] = v;
    }

    float3 get_velocity(int pid) constant {
        if (is_function_constant_defined(has_velocity) && !has_velocity) return DEFAULT_VELOCITY;
        return velocities[pid];
    }

    void set_velocity(int pid, float3 v) constant {
        if (is_function_constant_defined(has_velocity) && !has_velocity) return;
        velocities[pid] = v;
    }

    float4 get_color(int pid) constant {
        if (is_function_constant_defined(has_color) && !has_color) return DEFAULT_COLOR;
        return colors[pid];
    }

    void set_color(int pid, float4 v) constant {
        if (is_function_constant_defined(has_color) && !has_color) return;
        colors[pid] = v;
    }

    float3 get_size(uint pid) constant {
        if (has_size3D) return ((device simd_float3 *)sizes)[pid];
        if (has_size2D) return float3(((device simd_float2 *)sizes)[pid], 1.f);
        if (has_size1D) return float3(sizes[pid]);
        return DEFAULT_SIZE;
    }

    void set_size(uint pid, float3 size) constant {
        if (has_size3D) ((device simd_float3 *)sizes)[pid] = size;
        if (has_size2D) ((device simd_float2 *)sizes)[pid] = size.xy;
        if (has_size1D) sizes[pid] = size.x;
    }

    float2 get_size2D(uint pid) constant {
        return get_size(pid).xy;
    }

    void set_size2D(uint pid, float2 size) constant {
        set_size(pid, float3(size, 1.f));
    }

    float get_size1D(uint pid) constant {
        return get_size(pid).x;
    }
    
    void set_size1D(uint pid, float size)  constant {
        set_size(pid, float3(size));
    }

    float4 get_orientation(int pid) constant {
        if (is_function_constant_defined(has_orientation) && !has_orientation) return DEFAULT_ORIENTATION;
        return orientations[pid];
    }

    void set_orientation(int pid, float4 v) constant {
        if (is_function_constant_defined(has_orientation) && !has_orientation) return;
        orientations[pid] = v;
    }

    float4 get_angular_velocity(int pid) constant {
        if (is_function_constant_defined(has_angular_velocity) && !has_angular_velocity) return DEFAULT_ANGULAR_VELOCITY;
        return angular_velocities[pid];
    }

    void set_angular_velocity(int pid, float4 v) constant {
        if (is_function_constant_defined(has_angular_velocity) && !has_angular_velocity) return;
        angular_velocities[pid] = v;
    }

    float get_angle(int pid) constant {
        if (is_function_constant_defined(has_angle) && !has_angle) return DEFAULT_ANGLE;
        return angles[pid];
    }

    void set_angle(int pid, float v) constant {
        if (is_function_constant_defined(has_angle) && !has_angle) return;
        angles[pid] = v;
    }

    float get_angle_velocity(int pid) constant {
        if (is_function_constant_defined(has_angle_velocity) && !has_angle_velocity) return DEFAULT_ANGLE_VELOCITY;
        return angle_velocities[pid];
    }

    void set_angle_velocity(int pid, float v) constant {
        if (is_function_constant_defined(has_angle_velocity) && !has_angle_velocity) return;
        angle_velocities[pid] = v;
    }

    // age = normalized age
    
    float get_age(int pid) constant {
        if (is_function_constant_defined(has_age) && !has_age) return DEFAULT_AGE;
        return ages[pid];
    }

    void set_age(int pid, float v) constant {
        if (is_function_constant_defined(has_age) && !has_age) return;
        ages[pid] = v;
    }

    float get_lifetime(int pid) constant {
        if (is_function_constant_defined(has_lifetime) && !has_lifetime) return DEFAULT_LIFETIME;
        return lifetimes[pid];
    }

    void set_lifetime(int pid, float v) constant {
        if (is_function_constant_defined(has_lifetime) && !has_lifetime) return;
        lifetimes[pid] = v;
    }

    float get_texture_frame(int pid) constant {
        if (is_function_constant_defined(has_texture_frame) && !has_texture_frame) return DEFAULT_TEXTURE_FRAME;
        return frames[pid];
    }

    void set_texture_frame(int pid, float v) constant {
        if (is_function_constant_defined(has_texture_frame) && !has_texture_frame) return;
        frames[pid] = v;
    }

    float3 get_linear_factor(int pid) constant {
        if (is_function_constant_defined(has_linear_factor) && !has_linear_factor) return DEFAULT_LINEAR_FACTOR;
        return linear_factors[pid];
    }

    void set_linear_factor(int pid, float3 v) constant {
        if (is_function_constant_defined(has_linear_factor) && !has_linear_factor) return;
        linear_factors[pid] = v;
    }

    float3 get_angular_factor(int pid) constant {
        if (is_function_constant_defined(has_angular_factor) && !has_angular_factor) return DEFAULT_ANGULAR_FACTOR;
        return angular_factors[pid];
    }

    void set_angular_factor(int pid, float3 v) constant {
        if (is_function_constant_defined(has_angular_factor) && !has_angular_factor) return;
        angular_factors[pid] = v;
    }

    uint32_t get_pivot(int pid) constant {
        if (is_function_constant_defined(has_pivot) && !has_pivot) return DEFAULT_PIVOT;
        return pivots[pid];
    }

    void set_pivot(int pid, uint32_t v) constant {
        if (is_function_constant_defined(has_pivot) && !has_pivot) return;
        pivots[pid] = v;
    }

    float3 get_target(int pid) constant {
        if (is_function_constant_defined(has_target) && !has_target) return DEFAULT_TARGET;
        return targets[pid];
    }

    void set_target(int pid, float3 v) constant {
        if (is_function_constant_defined(has_target) && !has_target) return;
        targets[pid] = v;
    }
    
    // mass = 1 / mass
    
    float get_mass(int pid) constant {
        if (is_function_constant_defined(has_mass) && !has_mass) return DEFAULT_MASS;
        return masses[pid];
    }

    void set_mass(int pid, float v) constant {
        if (is_function_constant_defined(has_mass) && !has_mass) return;
        masses[pid] = v;
    }

    uint32_t get_particle_id(int pid) constant {
        if (is_function_constant_defined(has_particle_id) && !has_particle_id) return 0;
        return particle_id[pid];
    }
    
    void set_particle_id(int pid, uint32_t v) constant {
        if (is_function_constant_defined(has_particle_id) && !has_particle_id) return;
        particle_id[pid] = v;
    }
    
    float4 get_user_data1(int pid) constant {
        if (is_function_constant_defined(has_user_data1) && !has_user_data1) return 0.f;
        return user_data1[pid];
    }

    void set_user_data1(int pid, float4 v) constant {
        if (is_function_constant_defined(has_user_data1) && !has_user_data1) return;
        user_data1[pid] = v;
    }
    
    float4 get_user_data2(int pid) constant {
        if (is_function_constant_defined(has_user_data2) && !has_user_data2) return 0.f;
        return user_data2[pid];
    }

    void set_user_data2(int pid, float4 v) constant {
        if (is_function_constant_defined(has_user_data2) && !has_user_data2) return;
        user_data2[pid] = v;
    }
    
    float4 get_user_data3(int pid) constant {
        if (is_function_constant_defined(has_user_data3) && !has_user_data3) return 0.f;
        return user_data3[pid];
    }

    void set_user_data3(int pid, float4 v) constant {
        if (is_function_constant_defined(has_user_data3) && !has_user_data3) return;
        user_data3[pid] = v;
    }
    
    float4 get_user_data4(int pid) constant {
        if (is_function_constant_defined(has_user_data4) && !has_user_data4) return 0.f;
        return user_data4[pid];
    }

    void set_user_data4(int pid, float4 v) constant {
        if (is_function_constant_defined(has_user_data4) && !has_user_data4) return;
        user_data4[pid] = v;
    }
    
};

#endif

/// Particle buffer for scripts. Be careful when changing this struct as we embed metallibs in .vfx that are
/// then used with newer version of the framework.
struct particle_script_argument_buffer {

    // Not used anymore but kept for export legacy
    device int* legacy_partition            [[ id(0) ]];

    // We could also avec one counts buffer and use offsets in it ?
    device particle_counters* counters      [[ id(1) ]];

    device simd_float3* positions           [[ id(data_positions) ]];
    device simd_float4* colors              [[ id(data_colors) ]];

    // OPTIM : the non varying versions should be constant ????
    device simd_float3* velocities          [[ id(data_velocities) ]];

    // sizes can be 1D, 2D or 3D depending on the function constant
    device float* sizes                     [[ id(data_sizes) ]];

    device simd_float4* orientations        [[ id(data_orientations) ]];
    device uint32_t* order                  [[ id(data_sort_order) ]];
    device float* ages                      [[ id(data_ages) ]];
    device float* frames                    [[ id(data_frames) ]];
    device float* angles                    [[ id(data_angles) ]];
    device uint32_t* pivots                 [[ id(data_pivots) ]];
    device simd_float3* targets             [[ id(data_targets) ]];
    device float* masses                    [[ id(data_masses) ]];
    
    device simd_float4* user_data1          [[ id(data_userData1s) ]];
    device simd_float4* user_data2          [[ id(data_userData2s) ]];
    device simd_float4* user_data3          [[ id(data_userData3s) ]];
    device simd_float4* user_data4          [[ id(data_userData4s) ]];

    device float* lifetimes                 [[ id(data_lifetimes) ]];
    device float* ribbon_lengths            [[ id(data_ribbon_lengths) ]];
    device simd_float4* angular_velocities  [[ id(data_angular_velocities) ]];
    device float* angle_velocities          [[ id(data_angle_velocities) ]];
    device simd_float3* linear_factors      [[ id(data_linear_factors) ]];
    device simd_float3* angular_factors     [[ id(data_angular_factors) ]];

    device uint32_t* particle_id            [[ id(data_particleID) ]];
    
    device float* neighborCounts            [[ id(data_neighborCounts) ]];

    device float3* sizes3() constant { return (device float3*)sizes; }
    device float2* sizes2() constant { return (device float2*)sizes; }
    device float*  sizes1() constant { return (device float*)sizes; }

    bool is_outside(uint index) constant {
        return counters->is_outside(index);
    }

    bool newly_created_is_outside(thread uint* index) constant {
        return counters->newly_created_is_outside(index);
    }
};

struct particle_data_args {

private:
    constant particle_data_header& particle_header    [[ id(particle_header) ]];
    device const uint8_t* particle_data               [[ id(particle_datas) ]];

    device particle_counters* counters() constant {
        particle_data_description desc = particle_header.descriptions[ data_counters ];
        return (device particle_counters *)(particle_data + desc.offset);
    }

public:
    
    particle_data_args() = default;
    
    // Counters
    
    int get_active_count() constant { return counters()->get_active_count(); }
    void set_active_count(int v) constant { counters()->set_active_count(v); }
    
    int get_live_count() constant { return counters()->get_live_count(); }
    void set_live_count(int v) constant { counters()->set_live_count(v); }
    
    int get_added_count() constant { return counters()->get_added_count(); }
    void set_added_count(int v) constant { counters()->set_added_count(v); }

    void set_dead_count(int v) constant { counters()->set_dead_count(v); }

    int get_generated_count() constant { return counters()->get_generated_count(); }
    void add_generated_count(int v) constant { counters()->add_generated_count(v); }

    int32_t increment_live_count() constant { return counters()->increment_live_count(); }
    int32_t increment_dead_count() constant { return counters()->increment_dead_count(); }
    
    bool is_outside(uint index) constant { return counters()->is_outside(index); }
    uint index_from_added(uint index) constant { return counters()->index_from_added(index); }
    bool newly_created_is_outside(thread uint* index) constant { return counters()->newly_created_is_outside(index); }

    // Random
    
    float random01(uint index) constant { return counters()->random01(index); }
    float2 random_float2_01(uint index) constant { return counters()->random_float2_01(index); }
    float3 random_float3_01(uint index) constant { return counters()->random_float3_01(index); }

    float random_float_in_range(uint index, vfx_float_range range) constant { return counters()->random_float_in_range(index, range); }
    float2 random_float2_in_range(uint index, vfx_float_range xRange, vfx_float_range yRange) constant { return counters()->random_float2_in_range(index, xRange, yRange); }
    float3 random_float3_in_range(uint index, vfx_float_range xRange, vfx_float_range yRange, vfx_float_range zRange) constant { return counters()->random_float3_in_range(index, xRange, yRange, zRange); }

    // Generic Data

    device uint32_t* get_uint32(int data_index) constant {
        particle_data_description desc = particle_header.descriptions[ data_index ];
        return (device uint32_t *)(particle_data + desc.offset);
    }

    device float* get_float(int data_index) constant {
        particle_data_description desc = particle_header.descriptions[ data_index ];
        return (device float *)(particle_data + desc.offset);
    }

    device float2* get_float2(int data_index) constant {
        particle_data_description desc = particle_header.descriptions[ data_index ];
        return (device float2 *)(particle_data + desc.offset);
    }

    device float3* get_float3(int data_index) constant {
        particle_data_description desc = particle_header.descriptions[ data_index ];
        return (device float3 *)(particle_data + desc.offset);
    }

    device float4* get_float4(int data_index) constant {
        particle_data_description desc = particle_header.descriptions[ data_index ];
        return (device float4 *)(particle_data + desc.offset);
    }

    // Data
    
    float3 get_position(int pid) constant {
        return get_float3(data_positions)[pid];
    }

    void set_position(int pid, float3 v) constant {
        get_float3(data_positions)[pid] = v;
    }

    float3 get_velocity(int pid) constant {
        if (is_function_constant_defined(has_velocity) && !has_velocity) return DEFAULT_VELOCITY;
        return get_float3(data_velocities)[pid];
    }

    void set_velocity(int pid, float3 v) constant {
        if (is_function_constant_defined(has_velocity) && !has_velocity) return;
        get_float3(data_velocities)[pid] = v;
    }

    float4 get_color(int pid) constant {
        if (is_function_constant_defined(has_color) && !has_color) return DEFAULT_COLOR;
        // alpha needs saturation. Is this the right place to do this ???
        // color.a = saturate(color.a);
        return get_float4(data_colors)[pid];
    }

    void set_color(int pid, float4 v) constant {
        if (is_function_constant_defined(has_color) && !has_color) return;
        get_float4(data_colors)[pid] = v;
    }

    float get_ribbon_length(int pid) constant {
        if (is_function_constant_defined(has_ribbon_length) && !has_ribbon_length) return 0.f;
        return get_float(data_ribbon_lengths)[pid];
    }

    void set_ribbon_length(int pid, float v) constant {
        if (is_function_constant_defined(has_ribbon_length) && !has_ribbon_length) return;
        get_float(data_ribbon_lengths)[pid] = v;
    }

    float3 get_size(uint pid) constant {
        if (has_size3D) {
            return get_float3(data_sizes)[ pid ];
        } else if (has_size2D) {
            return float3(get_float2(data_sizes)[ pid ], 1);
        } else if (has_size1D) {
            return float3(get_float(data_sizes)[ pid ]);
        }
        return DEFAULT_SIZE;
    }

    void set_size(int pid, float3 v) constant {
        if (has_size3D) {
            get_float3(data_sizes)[ pid ] = v;
        } else if (has_size2D) {
            get_float2(data_sizes)[ pid ] = v.xy;
        } else if (has_size1D) {
            get_float(data_sizes)[ pid ] = v.x;
        }
    }
    
    float2 get_size2D(uint pid) constant {
        return get_size(pid).xy;
    }

    void set_size2D(uint pid, float2 size) constant {
        set_size(pid, float3(size, 1.f));
    }

    float get_size1D(uint pid) constant {
        return get_size(pid).x;
    }
    
    void set_size1D(uint pid, float size)  constant {
        set_size(pid, float3(size));
    }

    float4 get_orientation(int pid) constant {
        if (is_function_constant_defined(has_orientation) && !has_orientation) return DEFAULT_ORIENTATION;
        return get_float4(data_orientations)[pid];
    }

    void set_orientation(int pid, float4 v) constant {
        if (is_function_constant_defined(has_orientation) && !has_orientation) return;
        get_float4(data_orientations)[pid] = v;
    }

    float4 get_angular_velocity(int pid) constant {
        if (is_function_constant_defined(has_angular_velocity) && !has_angular_velocity) return DEFAULT_ANGULAR_VELOCITY;
        return get_float4(data_angular_velocities)[pid];
    }

    void set_angular_velocity(int pid, float4 v) constant {
        if (is_function_constant_defined(has_angular_velocity) && !has_angular_velocity) return;
        get_float4(data_angular_velocities)[pid] = v;
    }

    float get_angle(int pid) constant {
        if (is_function_constant_defined(has_angle) && !has_angle) return DEFAULT_ANGLE;
        return get_float(data_angles)[pid];
    }

    void set_angle(int pid, float v) constant {
        if (is_function_constant_defined(has_angle) && !has_angle) return;
        get_float(data_angles)[pid] = v;
    }

    float get_angle_velocity(int pid) constant {
        if (is_function_constant_defined(has_angle_velocity) && !has_angle_velocity) return DEFAULT_ANGLE_VELOCITY;
        return get_float(data_angle_velocities)[pid];
    }

    void set_angle_velocity(int pid, float v) constant {
        if (is_function_constant_defined(has_angle_velocity) && !has_angle_velocity) return;
        get_float(data_angle_velocities)[pid] = v;
    }

    // age = normalized age
    
    float get_age(int pid) constant {
        if (is_function_constant_defined(has_age) && !has_age) return DEFAULT_AGE;
        return get_float(data_ages)[pid];
    }

    void set_age(int pid, float v) constant {
        if (is_function_constant_defined(has_age) && !has_age) return;
        get_float(data_ages)[pid] = v;
    }

    float get_lifetime(int pid) constant {
        if (is_function_constant_defined(has_lifetime) && !has_lifetime) return DEFAULT_LIFETIME;
        return get_float(data_lifetimes)[pid];
    }

    void set_lifetime(int pid, float v) constant {
        if (is_function_constant_defined(has_lifetime) && !has_lifetime) return;
        get_float(data_lifetimes)[pid] = v;
    }

    float get_texture_frame(int pid) constant {
        if (is_function_constant_defined(has_texture_frame) && !has_texture_frame) return DEFAULT_TEXTURE_FRAME;
        return get_float(data_frames)[ pid ];
    }

    void set_texture_frame(int pid, float v) constant {
        if (is_function_constant_defined(has_texture_frame) && !has_texture_frame) return;
        get_float(data_frames)[ pid ] = v;
    }

    float3 get_linear_factor(int pid) constant {
        if (is_function_constant_defined(has_linear_factor) && !has_linear_factor) return DEFAULT_LINEAR_FACTOR;
        return get_float3(data_linear_factors)[pid];
    }

    void set_linear_factor(int pid, float3 v) constant {
        if (is_function_constant_defined(has_linear_factor) && !has_linear_factor) return;
        get_float3(data_linear_factors)[pid] = v;
    }

    float3 get_angular_factor(int pid) constant {
        if (is_function_constant_defined(has_angular_factor) && !has_angular_factor) return DEFAULT_ANGULAR_FACTOR;
        return get_float3(data_angular_factors)[pid];
    }

    void set_angular_factor(int pid, float3 v) constant {
        if (is_function_constant_defined(has_angular_factor) && !has_angular_factor) return;
        get_float3(data_angular_factors)[pid] = v;
    }

    uint32_t get_pivot(int pid) constant {
        if (is_function_constant_defined(has_pivot) && !has_pivot) return DEFAULT_PIVOT;
        return get_uint32(data_pivots)[pid];
    }

    void set_pivot(int pid, uint32_t v) constant {
        if (is_function_constant_defined(has_pivot) && !has_pivot) return;
        get_uint32(data_pivots)[pid] = v;
    }

    float3 get_target(int pid) constant {
        if (is_function_constant_defined(has_target) && !has_target) return DEFAULT_TARGET;
        return get_float3(data_targets)[pid];
    }

    void set_target(int pid, float3 v) constant {
        if (is_function_constant_defined(has_target) && !has_target) return;
        get_float3(data_targets)[pid] = v;
    }
    
    // mass = 1 / mass
    
    float get_mass(int pid) constant {
        if (is_function_constant_defined(has_mass) && !has_mass) return DEFAULT_MASS;
        return get_float(data_masses)[pid];
    }

    void set_mass(int pid, float v) constant {
        if (is_function_constant_defined(has_mass) && !has_mass) return;
        get_float(data_masses)[pid] = v;
    }

    uint32_t get_particle_id(int pid) constant {
        if (is_function_constant_defined(has_particle_id) && !has_particle_id) return 0;
        return get_uint32(data_particleID)[pid];
    }

    void set_particle_id(int pid, uint32_t v) constant {
        if (is_function_constant_defined(has_particle_id) && !has_particle_id) return;
        get_uint32(data_particleID)[pid] = v;
    }
    
    float4 get_user_data1(int pid) constant {
        if (is_function_constant_defined(has_user_data1) && !has_user_data1) return 0.f;
        return get_float4(data_userData1s)[pid];
    }

    void set_user_data1(int pid, float4 v) constant {
        if (is_function_constant_defined(has_user_data1) && !has_user_data1) return;
        get_float4(data_userData1s)[pid] = v;
    }
    
    float4 get_user_data2(int pid) constant {
        if (is_function_constant_defined(has_user_data2) && !has_user_data2) return 0.f;
        return get_float4(data_userData2s)[pid];
    }

    void set_user_data2(int pid, float4 v) constant {
        if (is_function_constant_defined(has_user_data2) && !has_user_data2) return;
        get_float4(data_userData2s)[pid] = v;
    }
    
    float4 get_user_data3(int pid) constant {
        if (is_function_constant_defined(has_user_data3) && !has_user_data3) return 0.f;
        return get_float4(data_userData3s)[pid];
    }

    void set_user_data3(int pid, float4 v) constant {
        if (is_function_constant_defined(has_user_data3) && !has_user_data3) return;
        get_float4(data_userData3s)[pid] = v;
    }
    
    float4 get_user_data4(int pid) constant {
        if (is_function_constant_defined(has_user_data4) && !has_user_data4) return 0.f;
        return get_float4(data_userData4s)[pid];
    }

    void set_user_data4(int pid, float4 v) constant {
        if (is_function_constant_defined(has_user_data4) && !has_user_data4) return;
        get_float4(data_userData4s)[pid] = v;
    }

    // pack/unpack
    
    float3 get_pivot_as_float3(int pid) constant {
        return unpack_unorm4x8_to_float(get_pivot(pid)).xyz * 2 - 1; // TODO pack to 1010102
    }

};

#define USE_PARTICLE_MONODATA 0

#endif
// MARK: - vfx_render.h


typedef struct
{
    float           aperture;
    float           focalLength;
    float           focusDistance;
    float           exposure;
} CameraSettings;

typedef struct {
    matrix_float4x4 viewMatrix; // view_from_world
    matrix_float4x4 inverseViewMatrix; // world_from_view
    matrix_float4x4 projectionMatrix;
    matrix_float4x4 viewProjectionMatrix;
    matrix_float4x4 modelViewMatrix; // view_from_model
    matrix_float4x4 inverseViewProjectionMatrix;
    // crWorld_from_world -> translation -camPos
    // view = camera_from_world
    // viewProj = clip_from_camera * camera_from_world
    // crProj = clip_from_cameraCenterZero * cameraCenterZero_from_world
    // cameraCenterZero_from_world = inverse(world_from_cameraCenterZero) = inverse(camera.transformNoTranslation)
    // crInverseViewProjectionMatrix = projection * inverse(camera.transformNoTranslation)
    matrix_float4x4 crInverseViewProjectionMatrix;

    // TODO remove: this is the same thing than inverseViewMatrix
    matrix_float4x4 cameraMatrix;
    simd_float3     cameraPos;

    // DoF parameters
    CameraSettings  cameraSettings;

    // contains xy: -drawableSize*2 zw: 1
    simd_float4     ndcFromScreen; // madd -> pt * xy + zw
    simd_float4     drawableSize; // .zw = inverse
    simd_float2     nearFar;

    // TODO: remove
    float     dt;
    float     time;
} ViewDependantConstants;

typedef struct
{
    unsigned short offset;
    unsigned short count;
} ClippingIndexSlice;

// RE version of ViewDependantConstants
typedef struct
{
    // TODO: Romain -> will need to add static assert to check similar value in RE/EngineConstants.h; likely in VFXREPlayer
//    static uint32_t kArraySize = 2;

    // instance constants
    matrix_float4x4 objectToCrWorld;
    unsigned int    viewportCount;

    // view constants
    matrix_float4x4 crWorldToViewArray[2];
    matrix_float4x4 crWorldToProjArray[2];
    matrix_float4x4 viewToProjArray[2];
    simd_float3     crwsCameraPositionArray[2];    // Position of the camera-view in camera-relative space, usually (0, 0, 0).

    simd_float4     renderTargetSize;
    unsigned int    useVertexAmplification;

    // global constants
    simd_float3     crwsReferencePosition; // crwsReferencePosition is the reference position of camera-relative world space in world-space.
                                           // It can be the position of the main camera, or other appropriate position based on current scene.

    float time;
    float dt;

    float fadeOpacity;

    ClippingIndexSlice clippingIndexSlice;
    simd_int2 renderTargetDepthStencilFormatSampleCount;
} REUniforms;

// Stereo info

struct particle_view_info {
    uint16_t cameraIndexToUse;
    int32_t cInstanceId;
};

// Used to keep semantic with corresponding value between swift and shader
// size_t for swift Int compat
typedef VFX_ENUM(size_t)
{
    blend_mode_constants_opaque = 0,
    blend_mode_constants_alpha = 1,
    blend_mode_constants_additive = 2,
    blend_mode_constants_mask = 3
} blend_mode_constants;

// TODO move to render_internal?
// can we have only one material uniforms ? seems unlikely...
// is this the generic base material??? what's in there
typedef struct
{
    // this are only valid if local_transform = true (EmitterDescription.isLocal = true)
    simd_float4x4 world_from_emitter;

    // Renderer has scale/orientation/position, to affect the individual particle
    simd_float4x4 particle_transform;

    float cutoff;
    float softDistance;
    float stretchFactor; // quad
    float curvature; // quad

    simd_float4 uvScaleBias; // quad
    simd_float3 upAxis;

    // only valid if local_transform = true (EmitterDescription.isLocal = true)
    // only uniform scale is supported
    float world_from_emitter_scale;

    float scale;     // aoSphere/aoBox
    float intensity; // aoSphere/aoBox
    float noiseScale; // volumetric
    float ribbonStride; // != 1 when ribbonCount is not nil => interleaving

    simd_float2 invFrameSize;

    float thickness; // line & quad

    int16_t frameCount; // animated texture / atlas
    int8_t primitiveShape;
    bool multiplyThicknessByAlpha; //quad
} particle_material_uniforms;

// Mesh/Vertex

// Keep these function constants index separated from particle_constants since
// they can be used in "generic" rendering context
typedef VFX_ENUM(int32_t)
{
    mesh_constants_has_vertex_position  = 400,
    mesh_constants_has_vertex_normal    = 401,
    mesh_constants_has_vertex_uv0       = 402,
    mesh_constants_has_vertex_uv1       = 403,
    mesh_constants_has_vertex_uv2       = 404,
    mesh_constants_has_vertex_uv3       = 405,
    mesh_constants_has_vertex_color     = 406,
    mesh_constants_has_vertex_tangent   = 407

} mesh_constants;

typedef VFX_BINDING_ENUM
{
    VertexAttributePosition     = 0,
    VertexAttributeNormal       = 1,
    VertexAttributeUV0          = 2,
    VertexAttributeUV1          = 3,
    VertexAttributeColor        = 4,
    VertexAttributeTangent      = 5,
    VertexAttributeUV2          = 6,
    VertexAttributeUV3          = 7
} VertexAttribute;

typedef VFX_BINDING_ENUM
{
    deferred_attachments_emissive               = 0,
    deferred_attachments_normal                 = 1,
    deferred_attachments_roughness_metalness    = 2,
    deferred_attachments_albedo                 = 3
} deferred_attachments;

#ifdef __METAL_VERSION__

constant bool has_vertex_position       [[ function_constant(mesh_constants_has_vertex_position) ]];
constant bool has_vertex_normal         [[ function_constant(mesh_constants_has_vertex_normal) ]];
constant bool has_vertex_uv0            [[ function_constant(mesh_constants_has_vertex_uv0) ]];
constant bool has_vertex_uv1            [[ function_constant(mesh_constants_has_vertex_uv1) ]];
constant bool has_vertex_uv2            [[ function_constant(mesh_constants_has_vertex_uv2) ]];
constant bool has_vertex_uv3            [[ function_constant(mesh_constants_has_vertex_uv3) ]];
constant bool has_vertex_color          [[ function_constant(mesh_constants_has_vertex_color) ]];
constant bool has_vertex_tangent        [[ function_constant(mesh_constants_has_vertex_tangent) ]];

typedef struct
{
    float3 position [[ attribute(VertexAttributePosition), function_constant(has_vertex_position) ]];
    float3 normal   [[ attribute(VertexAttributeNormal), function_constant(has_vertex_normal) ]];
    float2 uv0      [[ attribute(VertexAttributeUV0), function_constant(has_vertex_uv0) ]];
    float2 uv1      [[ attribute(VertexAttributeUV1), function_constant(has_vertex_uv1) ]];
    float2 uv2      [[ attribute(VertexAttributeUV2), function_constant(has_vertex_uv2) ]];
    float2 uv3      [[ attribute(VertexAttributeUV3), function_constant(has_vertex_uv3) ]];
    float4 color    [[ attribute(VertexAttributeColor), function_constant(has_vertex_color) ]];
    float4 tangent  [[ attribute(VertexAttributeTangent), function_constant(has_vertex_tangent) ]];

    float3 get_position() { return has_vertex_position ? position : float3(0); }
    float3 get_normal() { return has_vertex_normal ? normal : float3(0, 0, 1); }
    float2 get_uv0() { return has_vertex_uv0 ? uv0 : float2(0); }
    float2 get_uv1() { return has_vertex_uv1 ? uv1 : float2(0); }
    float2 get_uv2() { return has_vertex_uv2 ? uv2 : float2(0); }
    float2 get_uv3() { return has_vertex_uv3 ? uv3 : float2(0); }
    float4 get_color() { return has_vertex_color ? color : float4(1); }
    float4 get_tangent() { return has_vertex_tangent ? tangent : float4(0, 0, 0, 1); }

} Vertex;

// unproject a point given
// TODO move to vfx_math
static inline float3 unproject_cr(float2 fragmentPos, float zb, ViewDependantConstants uniforms)
{
    float4 ssp = float4( fragmentPos.x * uniforms.drawableSize.z, 1.f - fragmentPos.y * uniforms.drawableSize.w, zb, 1.f);
    ssp.xy = ssp.xy * 2.f - 1.f;
    float4 wp = uniforms.crInverseViewProjectionMatrix * ssp;
    return wp.xyz / wp.w;
}

static inline float4x4 getREViewMatrix(constant REUniforms& uniforms, unsigned short camIndex) {
    float4x4 t = float4x4(1.0f);
    t.columns[3].xyz = -uniforms.crwsReferencePosition;

    const auto cameraWorldToView = uniforms.crWorldToViewArray[camIndex];

    float4x4 viewFromWorld = cameraWorldToView * t;
    return viewFromWorld;
}

static inline float4x4 getREInverseViewMatrix(constant REUniforms& uniforms, unsigned short camIndex) {
    float4x4 viewFromWorld = getREViewMatrix(uniforms, camIndex);

    float3x3 worldFromViewOrientation = transpose(
        float3x3(viewFromWorld.columns[0].xyz,
                 viewFromWorld.columns[1].xyz,
                 viewFromWorld.columns[2].xyz));

    float4x4 worldFromView = float4x4(
        float4(worldFromViewOrientation.columns[0], 0),
        float4(worldFromViewOrientation.columns[1], 0),
        float4(worldFromViewOrientation.columns[2], 0),
        float4(- (worldFromViewOrientation * viewFromWorld.columns[3].xyz), 1) );

    return worldFromView;
}

static inline float3 box_corner_from_vid(uint16_t vert_id) {
    uint32_t vertex_bit = 1 << vert_id;
    uint32_t x = (0x287a & vertex_bit) != 0;
    uint32_t y = (0x02af & vertex_bit) != 0;
    uint32_t z = (0x31e3 & vertex_bit) != 0;
    return float3(x, y, z) * 2.f - 1.f; // 153 inst
}

constant int  blending_mode              [[ function_constant(particle_constants_render_blending_mode) ]];
constant bool enable_texture_2d          [[ function_constant(particle_constants_render_enable_texture_2d) ]];
constant bool enable_texture_3d          [[ function_constant(particle_constants_render_enable_texture_3d) ]];
constant bool enable_texture_2d_array    [[ function_constant(particle_constants_render_enable_texture_2d_array) ]];
constant bool enable_texture_cube        [[ function_constant(particle_constants_render_enable_texture_cube) ]];
constant bool enable_texture_prelighted  [[ function_constant(particle_constants_render_enable_texture_prelighted) ]];
constant bool has_particle_transform     [[ function_constant(particle_constants_render_has_particle_transform) ]];
constant bool texture_is_not_alphapremultiplied [[ function_constant(particle_constants_render_texture_is_not_alphapremultiplied) ]];
constant bool has_pbr_lighting           [[ function_constant(particle_constants_render_has_pbr_lighting) ]];
constant bool has_pbr_lighting_specular  [[ function_constant(particle_constants_render_has_pbr_lighting_specular) ]];
constant int animation_mode              [[ function_constant(particle_constants_render_animation_mode) ]];
constant bool inter_frame_interpolation  [[ function_constant(particle_constants_render_inter_frame_interpolation) ]];
constant bool enable_velocity_stretch    [[ function_constant(particle_constants_render_enable_velocity_stretch) ]];
constant bool enable_cutoff              [[ function_constant(particle_constants_render_enable_cutoff) ]];
constant bool enable_sorting             [[ function_constant(particle_constants_render_enable_sorting) ]];
constant bool enable_thick_lines         [[ function_constant(particle_constants_render_enable_thick_lines) ]];
constant bool enable_soft_particles      [[ function_constant(particle_constants_render_enable_soft_particles) ]];
constant bool need_opaque_zbuffer        [[ function_constant(particle_constants_render_need_opaque_zbuffer) ]];
constant bool need_normal_buffer         [[ function_constant(particle_constants_render_need_normal_buffer) ]];

//constant bool hasVoxelBox                [[ function_constant(42) ]];
constant bool enable_scattering            [[ function_constant(particle_constants_render_enable_scattering) ]];
//constant bool enablePrecomputeGI         [[ function_constant(47) ]];
//constant bool enableAO                   [[ function_constant(43) ]];
//constant bool enableGI                   [[ function_constant(44) ]];
//constant bool enableIBL                  [[ function_constant(45) ]];
//constant bool voxelOpacityOnly           [[ function_constant(46) ]];

constant bool enable_clipping            [[ function_constant(renderer_constants_enable_clipping) ]];
constant bool enable_alpha_fade          [[ function_constant(renderer_constants_enable_alpha_fade) ]];
constant bool enable_dither_fade         [[ function_constant(renderer_constants_enable_dither_fade) ]];
constant bool has_blue_noise_texture = enable_dither_fade;
constant bool enable_sample_mask_read  = enable_clipping && enable_dither_fade;
constant bool enable_sample_mask_write = enable_clipping && enable_dither_fade;

constant bool render_to_composite_layer  [[ function_constant(renderer_constants_render_to_composite_layer) ]];
constant int render_layer = (is_function_constant_defined(render_to_composite_layer) && render_to_composite_layer) ? 1 : 0;

constant bool is_multi_frame = animation_mode != -1;
constant bool need_inter_frame_interpolation = is_multi_frame && inter_frame_interpolation;
constant bool need_uv2 = need_inter_frame_interpolation && enable_texture_2d; // in case of texture3d & texture2d_array, uv == uv2

constant bool is_opaque = (blending_mode == blend_mode_constants_opaque) || (blending_mode == blend_mode_constants_mask);
constant bool need_frame = enable_texture_3d || need_inter_frame_interpolation;
constant bool need_worldPos = enable_scattering || has_pbr_lighting || enable_clipping;
constant bool need_worldNrm = has_pbr_lighting || enable_texture_cube || is_opaque;
constant bool need_point_size = enable_texture_2d || enable_texture_3d || has_pbr_lighting;
constant bool need_box_normal = enable_texture_2d || enable_texture_3d;

// re_uniforms is true when rendering from RE
constant bool gamma_blending             [[ function_constant(renderer_constants_gamma_blending) ]];
constant bool re_uniforms                [[ function_constant(renderer_constants_inside_re) ]];
constant bool vfx_uniforms = !re_uniforms;

constant bool enable_texture_prelighted_array = enable_texture_prelighted && enable_texture_2d_array;
constant bool enable_texture_prelighted_2d = enable_texture_prelighted && !enable_texture_2d_array;

constant bool need_sampler = enable_texture_2d || enable_texture_3d || enable_texture_2d_array;
constant bool uniform_sphere = has_size1D && !has_particle_transform;
constant bool non_uniform_sphere = !uniform_sphere;

#if TARGET_OS_REALITY
constant bool enable_depth_as_color = re_uniforms;
#else
constant bool enable_depth_as_color = false;
#endif

// Fragment Output

struct forward_frag_out {
    half4 color             [[ color(render_layer) ]];
    float depth_as_color    [[ color(4), function_constant(enable_depth_as_color) ]];
    uint sample_mask        [[ sample_mask, function_constant(enable_sample_mask_write) ]];
};

struct deferred_frag_out {
    half3 emissive [[ color(deferred_attachments_emissive) ]];
    half4 normal [[ color(deferred_attachments_normal) ]];
    half2 roughness_metalness [[ color(deferred_attachments_roughness_metalness) ]];
    half4 albedo [[ color(deferred_attachments_albedo) ]];
};

static inline half3 unpremultiply(half4 c) {
    return c.rgb / max(c.a, 1.h / MAXHALF);
}

//used for gamma blending
static inline half4 applyGammaIfNeeded(half4 color){
    if (gamma_blending) {
        //color was previously premultiplied in linear, we need to unpremultiply and redo it after conversion to gamma space
        color.rgb = vfx_linear_to_srgb(unpremultiply(color));
        color.rgb *= color.a;
    }
    return color;
}

static inline forward_frag_out forward_return(half4 color, float depth,
                                              uint sampleMask [[ function_constant(enable_sample_mask_read) ]]) {
    forward_frag_out out;
    out.color = applyGammaIfNeeded(color);
    if (enable_depth_as_color) {
        out.depth_as_color = depth;
    }
    out.sample_mask = sampleMask;
    return out;
}

struct view_args {
    constant ViewDependantConstants & uniforms  [[ buffer(vertex_frame_uniforms), function_constant(vfx_uniforms) ]];
    constant REUniforms & reuniforms            [[ buffer(vertex_frame_uniforms), function_constant(re_uniforms) ]];
    
    particle_view_info particleViewInfo(uint32_t inst_id, uint16_t amp_id) {
        particle_view_info info;
        info.cameraIndexToUse = 0;
        info.cInstanceId = inst_id;

        if (re_uniforms) {
            if (reuniforms.useVertexAmplification) {
                info.cInstanceId = inst_id;
                info.cameraIndexToUse = amp_id;
            } else {
                // this deals with viewportCount of 1 or 2
                info.cInstanceId = inst_id >> reuniforms.viewportCount;
                info.cameraIndexToUse = inst_id & (reuniforms.viewportCount - 1);
            }
        }
        return info;
    }

    float3 viewDir(unsigned short cameraIndex) {
        if (vfx_uniforms) {
            return -uniforms.cameraMatrix[2].xyz;
        } else {
            const auto cameraWorldToView = reuniforms.crWorldToViewArray[cameraIndex];
            return -float3(cameraWorldToView.columns[0].z, cameraWorldToView.columns[1].z, cameraWorldToView.columns[2].z);
        }
    }
    float4x4 viewMatrix(unsigned short cameraIndex) {
        return vfx_uniforms ? uniforms.viewMatrix : getREViewMatrix(reuniforms, cameraIndex);
    }
    float4x4 projMatrix(unsigned short cameraIndex) {
        return vfx_uniforms ? uniforms.projectionMatrix : reuniforms.viewToProjArray[cameraIndex];
    }
    float4x4 inverseViewMatrix(unsigned short cameraIndex) {
        return vfx_uniforms ? uniforms.inverseViewMatrix : getREInverseViewMatrix(reuniforms, cameraIndex);
    }
    float4x4 viewProjectionMatrix(unsigned short cameraIndex) {
        if (vfx_uniforms) {
            return uniforms.viewProjectionMatrix;
        } else {
            const auto viewToProjMtx = reuniforms.viewToProjArray[cameraIndex];
            return viewToProjMtx * getREViewMatrix(reuniforms, cameraIndex);
        }
    }
    float3 cameraPos() {
        return vfx_uniforms ? uniforms.cameraPos : reuniforms.crwsReferencePosition;
    }
    float dt() {
        return vfx_uniforms ? uniforms.dt : reuniforms.dt;
    }
    float2 drawableSize() {
        return vfx_uniforms ? uniforms.drawableSize.xy : reuniforms.renderTargetSize.xy;
    }
    float2 invDrawableSize() {
        return vfx_uniforms ? uniforms.drawableSize.zw : reuniforms.renderTargetSize.zw;
    }
};

struct particle_vertex_args {

    constant ViewDependantConstants & uniforms            [[ buffer(vertex_frame_uniforms), function_constant(vfx_uniforms) ]];
    constant REUniforms & reuniforms        [[ buffer(vertex_frame_uniforms), function_constant(re_uniforms) ]];

    constant particle_material_uniforms& matUniforms [[ buffer(vertex_material_uniforms) ]]; // TODO have a function constant to know if we need it?

#if USE_PARTICLE_MONODATA
    particle_data_args particle_data;
#else
    device const simd_float3* positions     [[ buffer(data_positions) ]];
    device const simd_float4* colors        [[ buffer(data_colors) ]];

    // OPTIM : the non varying versions should be constant ????
    device const simd_float3* velocities    [[ buffer(data_velocities), function_constant(has_velocity) ]];

    device const float* sizes1D             [[ buffer(data_sizes), function_constant(has_size1D) ]];
    device const simd_float2* sizes2D       [[ buffer(data_sizes), function_constant(has_size2D) ]];
    device const simd_float3* sizes3D       [[ buffer(data_sizes), function_constant(has_size3D) ]];

    device const float* ages                [[ buffer(data_ages), function_constant(has_age) ]];
    device const simd_float4* orientations  [[ buffer(data_orientations), function_constant(has_orientation) ]];
    device const float* frames              [[ buffer(data_frames), function_constant(has_texture_frame) ]];
    device const float* angles              [[ buffer(data_angles), function_constant(has_angle) ]];
    device const uint32_t* pivots           [[ buffer(data_pivots), function_constant(has_pivot) ]];
    device const simd_float3* targets       [[ buffer(data_targets), function_constant(has_target) ]];
    device const float* ribbon_lengths      [[ buffer(data_ribbon_lengths), function_constant(has_ribbon_length) ]];

    // TODO integrate it in the material_uniforms -> particle_uniforms
    // or use baseVertex? baseInstance?
    constant const int32_t& particle_count  [[ buffer(particle_count) ]];
#endif

    device const uint32_t* order            [[ buffer(data_sort_order), function_constant(enable_sorting) ]];

    particle_material_uniforms material() {
        return matUniforms;
    }
    
    particle_view_info particleViewInfo(uint32_t inst_id, uint16_t amp_id) {
        particle_view_info info;
        info.cameraIndexToUse = 0;
        info.cInstanceId = inst_id;

        if (re_uniforms) {
            if (reuniforms.useVertexAmplification) {
                info.cInstanceId = inst_id;
                info.cameraIndexToUse = amp_id;
            } else {
                info.cInstanceId = inst_id / reuniforms.viewportCount;
                info.cameraIndexToUse = inst_id % reuniforms.viewportCount;
            }
        }
        return info;
    }

    float3 viewDir(unsigned short cameraIndex) {
        if (vfx_uniforms) {
            return -uniforms.cameraMatrix[2].xyz;
        } else {
            const auto cameraWorldToView = reuniforms.crWorldToViewArray[cameraIndex];
            return -float3(cameraWorldToView.columns[0].z, cameraWorldToView.columns[1].z, cameraWorldToView.columns[2].z);
        }
    }
    float4x4 viewMatrix(unsigned short cameraIndex) {
        return vfx_uniforms ? uniforms.viewMatrix : getREViewMatrix(reuniforms, cameraIndex);
    }
    float4x4 projMatrix(unsigned short cameraIndex) {
        return vfx_uniforms ? uniforms.projectionMatrix : reuniforms.viewToProjArray[cameraIndex];
    }
    float4x4 inverseViewMatrix(unsigned short cameraIndex) {
        return vfx_uniforms ? uniforms.inverseViewMatrix : getREInverseViewMatrix(reuniforms, cameraIndex);
    }
    float4x4 viewProjectionMatrix(unsigned short cameraIndex) {
        if (vfx_uniforms) {
            return uniforms.viewProjectionMatrix;
        } else {
            const auto viewToProjMtx = reuniforms.viewToProjArray[cameraIndex];
            return viewToProjMtx * getREViewMatrix(reuniforms, cameraIndex);
        }
    }
    float3 cameraPos() {
        return vfx_uniforms ? uniforms.cameraPos : reuniforms.crwsReferencePosition;
    }
    float dt() {
        return vfx_uniforms ? uniforms.dt : reuniforms.dt;
    }

    float3 cameraView(unsigned short cameraIndex) {
        return vfx_uniforms ? float3(0.f) : reuniforms.crwsCameraPositionArray[cameraIndex];
    }

    float2 drawableSize() {
        return vfx_uniforms ? uniforms.drawableSize.xy : reuniforms.renderTargetSize.xy;
    }
    float2 invDrawableSize() {
        return vfx_uniforms ? uniforms.drawableSize.zw : reuniforms.renderTargetSize.zw;
    }

#if USE_PARTICLE_MONODATA

    float3 sample_pivot(uint32_t pid)       { return particle_data.get_pivot(pid); }
    float3 sample_position(uint pid)        { return particle_data.get_position(pid); }
    float3 sample_velocity(uint pid)        { return particle_data.get_velocity(pid); }
    float3 sample_size3D(uint pid)          { return particle_data.get_size3D(pid); }
    float2 sample_size2D(uint pid)          { return particle_data.get_size2D(pid); }
    float  sample_size1D(uint pid)          { return particle_data.get_size1D(pid); }
    float4 sample_orientation(uint pid)     { return particle_data.get_orientation(pid); }
    float3 sample_target(uint pid)          { return particle_data.get_target(pid); }
    half4 sample_color(uint pid)            { return half4(particle_data.get_color(pid)); }
    float4 sample_color_as_float(uint pid)  { return particle_data.get_color(pid); }
    float sample_angle(uint pid)            { return particle_data.get_angle(pid); }
    float sample_texture_frame(uint pid)    { return particle_data.get_texture_frame(pid); }
    float sample_ribbon_length(uint pid)    { return particle_data.get_ribbon_length(pid); }
    int active_count()                      { return particle_data.get_active_count(); }

#else

    int active_count()                    { return particle_count; }

    float3 sample_position(uint pid) {
        return positions[ pid ];
    }

    float3 sample_velocity(uint pid) {
        return velocities[ pid ];
    }

    float3 sample_pivot(uint32_t pid) {
        // TODO pack to 1010102
        return has_pivot
        ? (unpack_unorm4x8_to_float( pivots[ pid ] ).xyz * 2 - 1)
        : float3(0.f);
    }

    float3 sample_size3D(uint pid) {
        if (has_size3D) return sizes3D[ pid ];
        if (has_size2D) return float3(sizes2D[ pid ], 1);
        if (has_size1D) return float3(sizes1D[ pid ]);
        return float3(1.f);
    }

    float2 sample_size2D(uint pid) {
        if (has_size3D) return sizes3D[ pid ].xy;
        if (has_size2D) return sizes2D[ pid ];
        if (has_size1D) return float2(sizes1D[ pid ]);
        return float2(1.f);
    }

    float sample_size1D(uint pid) {
        if (has_size3D) return sizes3D[ pid ].x;
        if (has_size2D) return sizes2D[ pid ].x;
        if (has_size1D) return sizes1D[ pid ];
        return 1.f;
    }

    float4 sample_orientation(uint pid) {
        return has_orientation ? orientations[ pid ] : float4(0, 0, 0, 1);
    }

    float3 sample_target(uint pid) {
        return has_target ? targets[ pid ] : 0.f;
    }

    float sample_ribbon_length(uint pid) {
         return has_ribbon_length ? ribbon_lengths[ pid ] : 0.f;
     }

    half4 sample_color(uint pid) {
        if (has_color) {
            half4 color = half4(colors[ pid ]);
            // alpha needs saturation
            color.a = saturate(color.a);
            return color;
        } else {
            return 1.h;
        }
    }

    float4 sample_color_as_float(uint pid) {
        if (has_color) {
            float4 color = colors[ pid ];
            // alpha needs saturation
            color.a = saturate(color.a);
            return color;
        } else {
            return 1.f;
        }
    }

    float sample_angle(uint pid) {
        return has_angle ? angles[ pid ] : 0.f;
    }

    float sample_texture_frame(uint pid) {
        return has_texture_frame ? frames[ pid ] : 0.f;
    }
#endif

    float3 box_corner(uint32_t pid, uint16_t vert_id) {
        float3 pivot = sample_pivot(pid);
        // pivot = float3(0, 0, 0);
        return box_corner_from_vid(vert_id) - pivot;
    }

    float3 sample_position_world(uint pid) {
        float3 pos = sample_position(pid);
        if (is_local) {
            pos = vfx_transform_position(material().world_from_emitter, pos);
        }
        return pos;
    }

    float3 sample_velocity_world(uint pid) {
        float3 vel = sample_velocity(pid);
        if (is_local) {
            vel = vfx_transform_direction(material().world_from_emitter, vel);
        }
        return vel;
    }

    float sample_size_world(uint pid) {
        float size = sample_size1D(pid);
        if (is_local) {
            size *= emitter_scale();
        }
        return size;
    }

    float emitter_scale() {
        return is_local ? material().world_from_emitter_scale : 1.0f;
    }

    float4x4 sample_world_from_local(uint pid) {
        float3 pos = sample_position(pid);
        float4 ori = sample_orientation(pid);
        float3 scl = sample_size3D(pid);
        float4x4 emitter_from_particle = vfx_make_transform(ori, float4(pos, 1), scl);
        if (is_local) {
            return material().world_from_emitter * emitter_from_particle;
        } else {
            return emitter_from_particle; // emitter = world
        }
    }

    float4x4 sample_world_from_local_no_scale(uint pid) {
        float3 pos = sample_position(pid);
        float4 ori = sample_orientation(pid);
        float4x4 emitter_from_particle = vfx_make_transform_rotation_translation(ori, float4(pos, 1));
        if (is_local) {
            return material().world_from_emitter * emitter_from_particle;
        } else {
            return emitter_from_particle; // emitter = world
        }
    }

    float4x4 sample_world_from_local_with_pivot(uint pid) {
        float3 pos = sample_position(pid);
        float4 ori = sample_orientation(pid);
        float3 scl = sample_size3D(pid);
        if (has_pivot) {
            float3 pvt = sample_pivot(pid) * 2 - 1;
            pos -= pvt * scl;
        }
        float4x4 emitter_from_particle = vfx_make_transform(ori, float4(pos, 1), scl);
        if (is_local) {
            return material().world_from_emitter * emitter_from_particle;
        } else {
            return emitter_from_particle; // emitter = world
        }
    }

    float3x3 sample_rotation(uint pid) {
        float4 ori = sample_orientation(pid);
        return vfx_quat_to_matrix3x3(ori);
    }

};

#endif

// Custom Shader Part

typedef VFX_ENUM(int32_t)
{
    vertex_buffer_binding_view_uniforms         = 0,

    // Should be replaced by particle data if we want to support more
    // -> need to move to ArgumentBuffer then
    vertex_buffer_binding_instance_transforms   = 4,
    vertex_buffer_binding_instance_colors       = 5,

    vertex_buffer_binding_user_first_index      = 10,
    vertex_buffer_binding_user_count            = 4

    // mesh attachemnts begin at 16, so can't be past that...

} vertex_buffer_binding;

typedef VFX_ENUM(int32_t)
{
    vertex_texture_binding_user_first_index      = 10,
    vertex_texture_binding_user_count            = 4

} vertex_texture_binding;

typedef VFX_ENUM(int32_t)
{
    fragment_buffer_binding_view_uniforms         = 0,

    fragment_buffer_binding_user_first_index      = 10,
    fragment_buffer_binding_user_count            = 4

} fragment_buffer_binding;

typedef VFX_ENUM(int32_t)
{
    fragment_texture_binding_user_first_index      = 10,
    fragment_texture_binding_user_count            = 4

} fragment_texture_binding;

#define vfx_view_uniforms buffer(vertex_buffer_binding_view_uniforms)

#define vfx_instance_transforms buffer(vertex_buffer_binding_instance_transforms)
#define vfx_instance_colors buffer(vertex_buffer_binding_instance_colors)

#define vfx_user_vertex_buffer0 buffer(vertex_buffer_binding_user_first_index + 0)
#define vfx_user_vertex_buffer1 buffer(vertex_buffer_binding_user_first_index + 1)
#define vfx_user_vertex_buffer2 buffer(vertex_buffer_binding_user_first_index + 2)
#define vfx_user_vertex_buffer3 buffer(vertex_buffer_binding_user_first_index + 3)

#define vfx_user_vertex_texture0 texture(vertex_texture_binding_user_first_index + 0)
#define vfx_user_vertex_texture1 texture(vertex_texture_binding_user_first_index + 1)
#define vfx_user_vertex_texture2 texture(vertex_texture_binding_user_first_index + 2)
#define vfx_user_vertex_texture3 texture(vertex_texture_binding_user_first_index + 3)

#define vfx_user_fragment_buffer0 buffer(fragment_buffer_binding_user_first_index + 0)
#define vfx_user_fragment_buffer1 buffer(fragment_buffer_binding_user_first_index + 1)
#define vfx_user_fragment_buffer2 buffer(fragment_buffer_binding_user_first_index + 2)
#define vfx_user_fragment_buffer3 buffer(fragment_buffer_binding_user_first_index + 3)

#define vfx_user_fragment_texture0 texture(fragment_texture_binding_user_first_index + 0)
#define vfx_user_fragment_texture1 texture(fragment_texture_binding_user_first_index + 1)
#define vfx_user_fragment_texture2 texture(fragment_texture_binding_user_first_index + 2)
#define vfx_user_fragment_texture3 texture(fragment_texture_binding_user_first_index + 3)
// MARK: - vfx_compute.h


// Included by client compute metal files (script + textureShader)

typedef struct {
    float     dt;
    float     time;
    bool      isFirstFrame;
    uint32_t  simulationIndex;
} FrameConstants;
#pragma GCC diagnostic pop
