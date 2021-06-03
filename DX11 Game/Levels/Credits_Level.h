#pragma once
#ifndef CREDITS_LEVEL_H
#define CREDITS_LEVEL_H

#include "LevelContainer.h"
#include "LevelStateMachine.h"

#include<Graphics/UI_Manager.h>
#include<UI/Credits_UI.h>

class Credits_Level : public LevelContainer
{
public:
	Credits_Level( LevelStateMachine& stateMachine );
	bool OnCreate() override;
	void OnSwitch() override;
	void Render() override;
	void Update(const float dt) override;
	void CleanUp() override;
private:
	void RenderFrame() override;

	shared_ptr<Credits_UI> credits;
	LevelStateMachine& levelStateMachine;
};

#endif