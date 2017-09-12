#include "material.h"
material::material() :
	m_shine(100.0f),
	m_bump(0.0f),
	m_ambient(0.03f),
	reserved3(0.0f),
	m_colorAmbient(COLOR_WHITE),
	m_colorDiffuse(COLOR_WHITE),
	m_colorSpecular(COLOR_WHITE)
{
	// empty
}

material::material(float shine, color colorAmbient, color colorDiffuse, color colorSpecular) :
	m_shine(shine),
	m_bump(0.0f),
	m_ambient(0.0f),
	reserved3(0.0f),
	m_colorAmbient(colorAmbient),
	m_colorDiffuse(colorDiffuse),
	m_colorSpecular(colorSpecular)
{
	// empty
}

material::material(float shine, float bump, color colorAmbient, color colorDiffuse, color colorSpecular, float ambient) :
	m_shine(shine),
	m_bump(bump),
	m_ambient(ambient),
	reserved3(0.0f),
	m_colorAmbient(colorAmbient),
	m_colorDiffuse(colorDiffuse),
	m_colorSpecular(colorSpecular)
{
	// empty
}

material::~material() {
	// empty
}

RESULT material::SetColors(color colorAmbient, color colorDiffuse, color colorSpecular) {
	m_colorAmbient = colorAmbient;
	m_colorDiffuse = colorDiffuse;
	m_colorSpecular = colorSpecular;
	return R_PASS;
}

RESULT material::SetAmbientColor(color colorAmbient) {
	m_colorAmbient = colorAmbient;
	return R_PASS;
}

RESULT material::SetDiffuseColor(color colorDiffuse) {
	m_colorDiffuse = colorDiffuse;
	return R_PASS;
}

RESULT material::SetSpecularColor(color colorSpecular) {
	m_colorSpecular = colorSpecular;
	return R_PASS;
}

RESULT material::SetAmbientIntensity(float ambient) {
	m_ambient = ambient;
	return R_PASS;
}

color material::GetDiffuseColor() {
	return m_colorDiffuse;
}

RESULT material::SetShininess(float shine) {
	m_shine = shine;
	return R_PASS;
}

RESULT material::SetBumpiness(float bumpiness) {
	m_bump = bumpiness;
	return R_PASS;
}