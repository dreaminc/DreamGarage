#include "FogParams.h"

FogParams::FogParams() :
	m_startDistance(50.0f),
	m_endDistance(300.0f),
	m_density(0.05f),
	reserved0(0.0f),
	m_fogColor(COLOR_WHITE)
{
	// empty
}

FogParams::FogParams(float startDistance, float endDistance, float density, color fogColor) {
	m_startDistance = startDistance;
	m_endDistance = endDistance;
	m_density = density;
	m_fogColor = fogColor;
}

FogParams::~FogParams() {
	// empty
}