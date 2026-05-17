#ifndef SRC_SAMPLE_H_
#define SRC_SAMPLE_H_

#include <pcg_variants.h>

#define SAMPLE_COLOR_CHANGE_INTERVAL 60

typedef struct {
	int            counter;
	float          color[3];
	pcg32_random_t rng;
} Sample;

void sample_init(Sample *s, uint64_t seed_state, uint64_t seed_seq);
void sample_tick(Sample *s);
void sample_reset(Sample *s);

#endif
