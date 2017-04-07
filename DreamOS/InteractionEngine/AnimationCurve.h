#ifndef ANIMATION_CURVE_H_
#define ANIMATION_CURVE_H_

class AnimationCurve
{
	
enum class CurveType {
	EASE_OUT_QUAD,
	EASE_OUT_QUART,
	EASE_OUT_EXPO,
	EASE_OUT_BACK,
	LINEAR,
	INVALID
} CURVE_TYPE;

public:
	AnimationCurve(AnimationCurve::CurveType type = AnimationCurve::CurveType::LINEAR);
	~AnimationCurve();

public:
	float GetAnimationProgress(float progress);

private:
	CurveType m_curveType;
};


#endif // ! ANIMATION_CURVE_H_
