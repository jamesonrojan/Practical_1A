/*
 * Task 1: The Golden Measure on the PC
 *
 * Integer square root of a 32-bit unsigned input x: the largest integer
 * whose square does not exceed x.  Golden version uses double precision
 * arithmetic and the standard library square root.
 *
 * Prediction (written before running):
 *   On a ~3 GHz x86-64 CPU, sqrt + floor + conversion is roughly
 *   20..40 instructions.  Estimate: 40 instructions * 0.33 ns/instr
 *   ~= 13 ns per call.  One call alone cannot be timed reliably because
 *   clock_gettime resolution is ~10..50 ns and scheduling noise is larger
 *   than the call itself, so we loop and divide.
 *
 * Build:   gcc -O2 -o golden_measure golden_measure.c -lm
 * Run:     ./golden_measure
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

static const uint32_t inputs[10] = {
    0, 1, 15, 16, 4095, 65535,
    123456789, 987654321, 4294836225u, 4294967295u
};

static uint32_t golden_isqrt(uint32_t x)
{
    return (uint32_t)floor(sqrt((double)x));
}

static double timestamp_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000000.0 + (double)ts.tv_nsec / 1000.0;
}

/* Hand check: r^2 <= x < (r+1)^2, written out in full. */
static int hand_check(uint32_t x, uint32_t r)
{
    uint64_t low = (uint64_t)r*(uint64_t)r;
    uint64_t high = ((uint64_t)r+1)*((uint64_t)r+1);
    
    return((low <= (uint64_t)x) && (high > (uint64_t)x));
}

static double time_n_calls(long reps)
{

    volatile uint32_t result;
    
    double t0 = timestamp_us();

    for(uint32_t i = 0; i < reps; i++){
        result = golden_isqrt(inputs[i % 10]);
    }

    double t1 = timestamp_us();

    return (t1 - t0) * 1000.0 / (double)reps;
}

int main(void)
{
    printf("Predicted time per call: 13.6 ns\n\n");

    printf("Output table:\n");
    printf("%-14s %-12s %-6s\n", "input", "isqrt(x)", "check");
    for (int i = 0; i < 10; i++) {
        uint32_t x = inputs[i];
        uint32_t r = golden_isqrt(x);
        int ok = hand_check(x, r);
        printf("%-14" PRIu32 " %-12" PRIu32 " %s\n",
               x, r, ok ? "PASS" : "FAIL");
    }

    
    // Hand checks
    uint32_t x = 987654321u;
    uint32_t r = golden_isqrt(x);
    uint64_t rr = (uint64_t)r;
    printf("\nHand check for x = %" PRIu32 ":\n", x);
    printf("  r      = %" PRIu32 "\n", r);
    printf("  r^2    = %" PRIu64 "\n", rr * rr);
    printf("  (r+1)^2 = %" PRIu64 "\n", (rr + 1) * (rr + 1));
    printf("  x       = %" PRIu32 "\n", x);
    printf("  Result: %s\n", hand_check(x, r) ? "PASS" : "FAIL");

    // Two different repetition counts, to confirm the time per call is stable
    long reps1 = 1000000L;
    long reps2 = 10000000L;

    double ns1 = time_n_calls(reps1);
    double ns2 = time_n_calls(reps2);
    double mean = (ns1 + ns2) / 2.0;
    double spread = fabs(ns1 - ns2);

    printf("\nTiming results:\n");
    printf("  Run 1: %ld reps -> %.3f ns/call\n", reps1, ns1);
    printf("  Run 2: %ld reps -> %.3f ns/call\n", reps2, ns2);
    printf("  Mean:  %.3f ns/call\n", mean);
    printf("  Spread: %.3f ns\n", spread);

    return 0;
}