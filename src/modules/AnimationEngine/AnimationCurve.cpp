#include "AnimationCurve.h"
#include <math.h>

AnimationCurve::AnimationCurve(AnimationCurveType type) {
	m_curveType = type;
}

AnimationCurve::~AnimationCurve() {
	// empty
}

float AnimationCurve::GetAnimationProgress(float progress) {
	switch (m_curveType) {
	case AnimationCurveType::SIGMOID:
		return (tanh(6.0f*(progress - 0.5f)) + 1.0f) / 2.0f;
	case AnimationCurveType::EASE_OUT_QUART:
		return -1.0f * (pow((progress - 1), 4) - 1); break;
	case AnimationCurveType::EASE_IN_QUART:
		return 1.0f - 1.0f * (pow(((1 - progress) - 1), 4) - 1); break;
	case AnimationCurveType::EASE_OUT_QUAD:
		return -1.0f * progress * (progress - 2.0f); break;
	case AnimationCurveType::EASE_OUT_EXPO:
		return -pow(2, -10 * progress) + 1; break;
	case AnimationCurveType::EASE_OUT_BACK:
		return (progress - 1) * (progress - 1) * ((EASING_CONSTANT + 1) * (progress - 1) + EASING_CONSTANT) + 1; break;
	case AnimationCurveType::LINEAR:
		return progress; break;
	default:
		return 0.0f; break;
	}
}