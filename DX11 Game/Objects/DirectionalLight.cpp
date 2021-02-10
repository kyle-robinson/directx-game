#include "DirectionalLight.h"
#include "Camera.h"
#include <imgui/imgui.h>

// "Disco Ball" (https://skfb.ly/6C9ET) by mozillareality is licensed under Creative Commons Attribution (http://creativecommons.org/licenses/by/4.0/).

void DirectionalLight::SpawnControlWindow()
{
	if ( ImGui::Begin( "Directional Light", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		ImGui::SliderFloat3( "Position", &directionalLightPosition.x, -20.0f, 20.0f, "%.1f" );
		ImGui::ColorEdit3( "Colour", &directionalLightColor.x );
		ImGui::SliderFloat( "Intensity", &directionalLightStrength, 0.1f, 1.0f, "%.1f" );
	}
	ImGui::End();
}

void DirectionalLight::UpdateConstantBuffer( ConstantBuffer<CB_PS_directional>& cb_ps_directional )
{
	cb_ps_directional.data.directionalPosition = directionalLightPosition;
	cb_ps_directional.data.directionalColor = directionalLightColor;
	cb_ps_directional.data.directionalStrength = directionalLightStrength;
}