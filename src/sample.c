#include "sample.h"
#include <math.h>
#include <stdint.h>

static float pcg_float(pcg32_random_t *rng) { return ldexp(pcg32_random_r(rng), -32); }

void sample_init(Sample *s, uint64_t seed_state, uint64_t seed_seq) {
    s->counter = 0;
    pcg32_srandom_r(&s->rng, seed_state, seed_seq);
    s->color[0] = pcg_float(&s->rng);
    s->color[1] = pcg_float(&s->rng);
    s->color[2] = pcg_float(&s->rng);
}

void sample_tick(Sample *s) {
    s->counter++;
    if (s->counter % SAMPLE_COLOR_CHANGE_INTERVAL == 0) {
        s->color[0] = pcg_float(&s->rng);
        s->color[1] = pcg_float(&s->rng);
        s->color[2] = pcg_float(&s->rng);
    }
}

void sample_reset(Sample *s) {
    pcg32_srandom_r(&s->rng, 42u, 54u);
    s->counter  = 0;
    s->color[0] = pcg_float(&s->rng);
    s->color[1] = pcg_float(&s->rng);
    s->color[2] = pcg_float(&s->rng);
}
