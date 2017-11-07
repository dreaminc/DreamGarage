#ifndef ANIMATION_CURVE_H_
#define ANIMATION_CURVE_H_

#define EASING_CONSTANT 1.70158f

enum class AnimationCurveType {
	SIGMOID,
	EASE_OUT_QUAD,
	EASE_OUT_QUART,
	EASE_OUT_EXPO,
	EASE_OUT_BACK,
	EASE_IN_QUART,
	LINEAR,
	INVALID
};

class AnimationCurve
{

public:
	AnimationCurve(AnimationCurveType type = AnimationCurveType::LINEAR);
	~AnimationCurve();

public:
	float GetAnimationProgress(float progress);

private:
	AnimationCurveType m_curveType;
};


#endif // ! ANIMATION_CURVE_H_
