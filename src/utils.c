#include "utils.h"

float lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

float clampf(float v, float lo, float hi) {
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}
