#include "material.h"

material::material() :
	m_shine(DEFUALT_MATERIAL_SHINE),
	m_bumpiness(1.0f),
	m_ambient(DEFAULT_MATERIAL_AMBIENT_LEVEL),
	reserved0(0.0f),
	m_colorAmbient(COLOR_WHITE),
	m_colorDiffuse(COLOR_WHITE),
	m_colorSpecular(COLOR_WHITE),
	m_tilingU(1.0f),
	m_tilingV(1.0f),
	m_reflectivity(0.0f),
	m_refractivity(0.0f)
{
	// empty
}

material::material(float shine, color colorAmbient, color colorDiffuse, color colorSpecular) :
	m_shine(shine),
	m_bumpiness(1.0f),
	m_ambient(DEFAULT_MATERIAL_AMBIENT_LEVEL),
	reserved0(0.0f),
	m_colorAmbient(colorAmbient),
	m_colorDiffuse(colorDiffuse),
	m_colorSpecular(colorSpecular),
	m_tilingU(1.0f),
	m_tilingV(1.0f),
	m_reflectivity(0.0f),
	m_refractivity(0.0f)
{
	// empty
}

material::material(float shine, float bump, color colorAmbient, color colorDiffuse, color colorSpecular, float ambient) :
	m_shine(shine),
	m_bumpiness(bump),
	m_ambient(ambient),
	reserved0(1.0f),
	m_colorAmbient(colorAmbient),
	m_colorDiffuse(colorDiffuse),
	m_colorSpecular(colorSpecular),
	m_tilingU(1.0f),
	m_tilingV(1.0f),
	m_reflectivity(0.0f),
	m_refractivity(0.0f)
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

color material::GetSpecularColor() {
	return m_colorSpecular;
}

color material::GetAmbientColor() {
	return m_colorAmbient;
}

RESULT material::SetShininess(float shine) {
	m_shine = shine;
	return R_PASS;
}

RESULT material::SetBumpiness(float bumpiness) {
	m_bumpiness = bumpiness;
	return R_PASS;
}

RESULT material::SetReflectivity(float reflectivity) {
	m_reflectivity = reflectivity;
	return R_PASS;
}

RESULT material::SetRefractivity(float refractivity){
	m_refractivity = refractivity;
	return R_PASS;
}

RESULT material::SetUVTiling(float uTiling, float vTiling) {
	m_tilingU = uTiling;
	m_tilingV = vTiling;
	return R_PASS;
}