#include "stdafx.h"
#include "Input.h"
#include "CameraMovement.h"

void Input::Initialize( RenderWindow& window, LevelStateMachine* stateMachine,
	CameraController* camera )
{
	cameras = camera;
	renderWindow = window;
	//this->level_IDs = level_IDs;
	this->levelSystem = stateMachine;
	this->level = &*stateMachine->GetCurrentLevel();

	keyboard.DisableAutoRepeatKeys();
	mousePick.Initialize(
		static_cast<int>( renderWindow.GetWidth() ),
		static_cast<int>( renderWindow.GetHeight() )
	);

	AddToEvent();
}

void Input::Update( const float dt )
{
	UpdateKeyboard( dt );
	UpdateMouse( dt );
}

void Input::AddToEvent()
{
	EventSystem::Instance()->AddClient( EVENTID::GamePauseEvent, this );
	EventSystem::Instance()->AddClient( EVENTID::UpdateSettingsEvent, this );
	EventSystem::Instance()->AddClient( EVENTID::WindowSizeChangeEvent, this );
	EventSystem::Instance()->AddClient( EVENTID::GameUnPauseEvent, this );
	EventSystem::Instance()->AddClient( EVENTID::ShowCursorEvent, this );
	EventSystem::Instance()->AddClient( EVENTID::HideCursorEvent, this );
	EventSystem::Instance()->AddClient( EVENTID::IsDissCube, this );
}

void Input::HandleEvent( Event* event )
{
	switch ( event->GetEventID() )
	{
	case EVENTID::ShowCursorEvent:
	{
		EnableCursor();
	}
	break;
	case EVENTID::HideCursorEvent:
	{
		DisableCursor();
	}
	break;
	case EVENTID::GamePauseEvent:
	{
		EnableCursor();
		isPaused = true;
	}
	break;
	case EVENTID::GameUnPauseEvent:
	{
		UiMouseData.LPress = false;
		UiMouseData.MPress = false;
		UiMouseData.RPress = false;
		DisableCursor();
		isPaused = false;
	}
	break;
	case EVENTID::WindowSizeChangeEvent:
	{
		DirectX::XMFLOAT2 _SizeOfScreen = *static_cast<DirectX::XMFLOAT2*>( event->GetData() );
		mousePick.SetWidthHight( _SizeOfScreen.x, _SizeOfScreen.y );

		UiMouseData.LPress = false;
		UiMouseData.MPress = false;
		UiMouseData.RPress = false;
	}
	break;
	case EVENTID::UpdateSettingsEvent:
	{
		// Controls 
		std::vector<JSON::SettingData> a = *static_cast<std::vector<JSON::SettingData>*>( event->GetData() );
		for ( auto& setting : a )
		{
			// Only for player not debug key changes
			if ( setting.Type == JSON::SettingType::ControlType )
			{
				// Change control

				// Control map
				std::string key = std::get<std::string>( setting.Setting ).c_str();

				// Convert to input commands
				if ( key == "SCROLL WHEEL" ) {
					MouseBinds[setting.Name + "_Up"] = Mouse::MouseEvent::EventType::WheelUp;
					MouseBinds[setting.Name + "_Down"] = Mouse::MouseEvent::EventType::WheelDown;
				}
				else if ( key == "RMB" )
				{
					MouseBinds[setting.Name] = Mouse::MouseEvent::EventType::RPress;
				}
				else if ( key == "LMB" )
				{
					MouseBinds[setting.Name] = Mouse::MouseEvent::EventType::LPress;
				}
				else if ( key == "MMB" )
				{
					MouseBinds[setting.Name] = Mouse::MouseEvent::EventType::MPress;
				}
				else
				{
					unsigned char* valChar = (unsigned char*)key.c_str();
					KeyBinds[setting.Name] = *valChar;
				}
			}

			// Mouse inputs
			MouseBinds["Change_Gun_State_Up"] = Mouse::MouseEvent::EventType::WheelUp;
			MouseBinds["Change_Gun_State_Down"] = Mouse::MouseEvent::EventType::WheelDown;
			MouseBinds["Fire_Tool"] = Mouse::MouseEvent::EventType::LPress;
			MouseBinds["Fire_Tool_Alt"] = Mouse::MouseEvent::EventType::RPress;
		}
	}
	break;
	}
}

void Input::UpdateKeyboard( const float dt )
{
	// Alt tab support
	if ( ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) && ( GetAsyncKeyState( VK_TAB ) & 0x8000 ) )
		EventSystem::Instance()->AddEvent( EVENTID::GamePauseEvent );

#pragma region KeyPress_Once
	// Handle input for single key presses
	while ( !keyboard.KeyBufferIsEmpty() )
	{
		Keyboard::KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();

		// LEVEL SELECTION
		{
			//if ( keycode == VK_NUMPAD1 ) levelSystem->SwitchTo( level_IDs[0] );
			//if ( keycode == VK_NUMPAD2 ) levelSystem->SwitchTo( level_IDs[1] );
		}

		// CAMERA INPUT
		{
			// Set camera to use
			if ( keycode == VK_F1 ) levelSystem->GetCurrentLevel()->GetCameraController()->SetIsUsingMain( true );
			if ( keycode == VK_F2 ) levelSystem->GetCurrentLevel()->GetCameraController()->SetIsUsingMain( false );

			// Set options for debug camera
			if ( cameras->GetCurrentCamera() == JSON::CameraType::Debug )
			{
				// Set cursor enabled/disabled
				if ( keycode == VK_HOME && !cursorEnabled ) EnableCursor();
				else if ( keycode == VK_END && cursorEnabled ) DisableCursor();
			}
		}

		// MULTI-TOOL INPUT
		{
			// set multi-tool type
			if ( !isPaused )
			{
				if ( keycode == KeyBinds["Gun_State_One"] ) {
					currentTool = ToolType::Convert;
					EventSystem::Instance()->AddEvent( EVENTID::ChangeToolEvent, &currentTool );
					Sound::Instance()->PlaySoundEffect( "ToolSwitchMode" );
				}
				if ( keycode == KeyBinds["Gun_State_Two"] ) {
					currentTool = ToolType::Resize;
					EventSystem::Instance()->AddEvent( EVENTID::ChangeToolEvent, &currentTool );
					Sound::Instance()->PlaySoundEffect( "ToolSwitchMode" );
				}
				if ( keycode == KeyBinds["Gun_State_Three"] ) {
					currentTool = ToolType::Bounce;
					EventSystem::Instance()->AddEvent( EVENTID::ChangeToolEvent, &currentTool );
					Sound::Instance()->PlaySoundEffect( "ToolSwitchMode" );
				}
				if ( keycode == KeyBinds["Gun_State_Four"] ) {
					currentTool = ToolType::Magnetism;
					EventSystem::Instance()->AddEvent( EVENTID::ChangeToolEvent, &currentTool );
					Sound::Instance()->PlaySoundEffect( "ToolSwitchMode" );
				}
				if ( keycode == KeyBinds["Gun_State_Five"] ) {
					currentTool = ToolType::Conductive;
					EventSystem::Instance()->AddEvent( EVENTID::ChangeToolEvent, &currentTool );
					Sound::Instance()->PlaySoundEffect( "ToolSwitchMode" );
				}
				if ( keycode == KeyBinds["Gun_State_Six"] );

				if ( kbe.IsPress() ) {
					if ( keycode == KeyBinds["Change_Gun_State_Up"] ) {
						EventSystem::Instance()->AddEvent( EVENTID::ChangeToolOptionUpEvent );
					}
					else if ( keycode == KeyBinds["Change_Gun_State_Down"] ) {
						EventSystem::Instance()->AddEvent( EVENTID::ChangeToolOptionDownEvent );
					}
				}
			}
		}

		// UI
		{
			UIChar = keycode;
			EventSystem::Instance()->AddEvent( EVENTID::UIKeyInput, &UIChar );

			if ( keycode == KeyBinds["Pause"] )
				EventSystem::Instance()->AddEvent( EVENTID::GamePauseEvent );
		}
	}
#pragma endregion

#pragma region KeyPress_Repeat
	// CAMERA INPUT
	{
		// TODO: THIS SHOULD GO INTO AN UPDATE FUNCTION IN THE CAMERA CLASS
		// Set which camera for the static camera to look at
		cameras->GetCamera( JSON::CameraType::Static )->SetLookAtPos(
			cameras->GetCamera( levelSystem->GetCurrentLevel()->GetCameraController()->GetCurrentCamera() )->GetPositionFloat3() );

		// Update mode to ignore y-movement when not in debug mode. Will be changed in the future likely when player can move around the environment with physics/collisions.
		// Will also need to be changed to the player object when player becomes its own class. Unknown how that will work currently
		bool playMode = true;
		if ( cameras->GetCurrentCamera() == JSON::CameraType::Debug )
		{
			playMode = false;
			if ( keyboard.KeyIsPressed( VK_SPACE ) ) CameraMovement::MoveUp( cameras->GetCamera( JSON::CameraType::Debug ), dt );
			if ( keyboard.KeyIsPressed( VK_CONTROL ) ) CameraMovement::MoveDown( cameras->GetCamera( JSON::CameraType::Debug ), dt );
		}
		else if (!isPaused)
		{
			static bool jumping = false;
			if (keyboard.KeyIsPressed(KeyBinds["Jump"]) || jumping)
				CameraMovement::Jump(cameras->GetCamera(JSON::CameraType::Default), jumping, dt);

			static bool crouching = false;
			if (keyboard.KeyIsPressed(KeyBinds["Crouch"]))
				crouching = true;
			else
				crouching = false;
			if (!jumping)
				CameraMovement::Crouch(cameras->GetCamera(JSON::CameraType::Default), crouching, dt);
		
		}

		if ( !isPaused )
		{
			// Normalize diagonal movement speed
			if ( keyboard.KeyIsPressed( KeyBinds["Forward"] ) && ( keyboard.KeyIsPressed( KeyBinds["Left"] ) || keyboard.KeyIsPressed( KeyBinds["Back"] ) ) )
				cameras->GetCamera( cameras->GetCurrentCamera() )->SetCameraSpeed( 0.008f );
			if ( keyboard.KeyIsPressed( KeyBinds["Back"] ) && ( keyboard.KeyIsPressed( KeyBinds["Left"] ) || keyboard.KeyIsPressed( KeyBinds["Back"] ) ) )
				cameras->GetCamera( cameras->GetCurrentCamera() )->SetCameraSpeed( 0.008f );

			// Update camera movement
			if ( keyboard.KeyIsPressed( KeyBinds["Forward"] ) ) CameraMovement::MoveForward( cameras->GetCamera( cameras->GetCurrentCamera() ), playMode, dt );
			if ( keyboard.KeyIsPressed( KeyBinds["Left"] ) ) CameraMovement::MoveLeft( cameras->GetCamera( cameras->GetCurrentCamera() ), playMode, dt );
			if ( keyboard.KeyIsPressed( KeyBinds["Back"] ) ) CameraMovement::MoveBackward( cameras->GetCamera( cameras->GetCurrentCamera() ), playMode, dt );
			if ( keyboard.KeyIsPressed( KeyBinds["Right"] ) ) CameraMovement::MoveRight( cameras->GetCamera( cameras->GetCurrentCamera() ), playMode, dt );
		}

		// Set camera speed
		cameras->GetCamera( cameras->GetCurrentCamera() )->SetCameraSpeed( 0.01f );
	}

	// MULTI-TOOL INPUT
	{
		// Set multi-tool type
		if ( !isPaused )
		{
			for ( uint32_t i = 0; i < levelSystem->GetCurrentLevel()->GetNumOfLevelCubes(); i++ )
			{
				// Ensure another cube is not already being held
				float alreadyHeld = false;
				for ( uint32_t j = 0; j < levelSystem->GetCurrentLevel()->GetNumOfLevelCubes(); j++ )
					if ( i != j && levelSystem->GetCurrentLevel()->GetCube()[j]->GetIsHolding() == true )
						alreadyHeld = true;

				// Pickup cube is in range, hovering with mouse and not already holding a cube - toggle function - was ( ( GetKeyState( KeyBindes["Action"] ) & 0x0001 ) != 0
				if ( ( keyboard.KeyIsPressed( KeyBinds["Action"] ) ) &&
					!alreadyHeld && levelSystem->GetCurrentLevel()->GetCube()[i]->GetIsInRange() && canHover && !levelSystem->GetCurrentLevel()->GetCube()[i]->GetIsDissCube() &&
					( levelSystem->GetCurrentLevel()->GetCube()[i]->GetIsHovering() || levelSystem->GetCurrentLevel()->GetCube()[i]->GetIsHolding() ) )
				{
					levelSystem->GetCurrentLevel()->GetCube()[i]->SetIsHolding( true );
					levelSystem->GetCurrentLevel()->GetCube()[i]->GetPhysicsModel()->ResetForces();

					if ( !heldLastFrame[i] )
						Sound::Instance()->PlaySoundEffect( "CubePickup" );

					// Set cube position
					static int offset = 2;
					switch ( levelSystem->GetCurrentLevel()->GetCube()[i]->GetEditableProperties()->GetBoxSize() )
					{
					case BoxSize::Small:  offset = 1; break;
					case BoxSize::Normal: offset = 2; break;
					case BoxSize::Large:  offset = 4; break;
					}
					XMVECTOR cubePosition = cameras->GetCamera( cameras->GetCurrentCamera() )->GetPositionVector();
					cubePosition += cameras->GetCamera( cameras->GetCurrentCamera() )->GetForwardVector() * offset;
					levelSystem->GetCurrentLevel()->GetCube()[i]->SetPosition( cubePosition );

					// Set cube rotation
					levelSystem->GetCurrentLevel()->GetCube()[i]->SetRotation(
						levelSystem->GetCurrentLevel()->GetCube()[i]->GetRotationFloat3().x,
						cameras->GetCamera( cameras->GetCurrentCamera() )->GetRotationFloat3().y,
						levelSystem->GetCurrentLevel()->GetCube()[i]->GetRotationFloat3().z
					);

					heldLastFrame[i] = true;
				}
				else
				{
					levelSystem->GetCurrentLevel()->GetCube()[i]->SetIsHolding( false );
					heldLastFrame[i] = false;
				}
			}

			if ( !canHover && delay < 100.0f )
				delay += 1.0f;
			else
			{
				canHover = true;
				delay = 0.0f;
			}
		}
		else
		{
			for ( int i = 0; i < levelSystem->GetCurrentLevel()->GetNumOfLevelCubes(); i++ )
				levelSystem->GetCurrentLevel()->GetCube()[i]->SetIsHolding( false );
		}
	}
#pragma endregion
}

void Input::UpdateMouse( const float dt )
{
	// Read mouse events
	if ( !isPaused && cameras->GetCurrentCamera() != JSON::CameraType::Debug )
		DisableCursor();

	while ( !mouse.EventBufferIsEmpty() )
	{
		Mouse::MouseEvent me = mouse.ReadEvent();

		// CAMERA INPUT
		{
			if ( !isPaused )
			{
				// camera orientation
				if ( mouse.IsRightDown() || !cursorEnabled )
				{
					// update raw camera movement
					if ( me.GetType() == Mouse::MouseEvent::EventType::RawMove )
					{
						cameras->GetCamera( cameras->GetCurrentCamera() )->AdjustRotation(
							XMFLOAT3(
								static_cast<float>( me.GetPosY() ) * 0.005f,
								static_cast<float>( me.GetPosX() ) * 0.005f,
								0.0f
							)
						);
					}
				}
			}
		}

		// MULTI-TOOL INPUT
		{
			if ( !isPaused )
			{
				if ( me.GetType() == MouseBinds["Change_Gun_State_Up"] )
				{
					EventSystem::Instance()->AddEvent( EVENTID::ChangeToolOptionUpEvent );
					Sound::Instance()->PlaySoundEffect( "ToolChange" );
				}
				else if ( me.GetType() == MouseBinds["Change_Gun_State_Down"] )
				{
					EventSystem::Instance()->AddEvent( EVENTID::ChangeToolOptionDownEvent );
					Sound::Instance()->PlaySoundEffect( "ToolChange" );
				}

				// Mag mode all
				if ( me.GetType() == MouseBinds["Fire_Tool"] ) {
					EventSystem::Instance()->AddEvent( EVENTID::ChangeAllCubeEvent, &levelSystem->GetCurrentLevel()->GetCube() );

				}
			}

			// Mouse picking
			mousePick.UpdateMatrices( cameras->GetCamera( cameras->GetCurrentCamera() ) );
			for ( uint32_t i = 0; i < levelSystem->GetCurrentLevel()->GetNumOfLevelCubes(); i++ )
			{
				// Cube mouse input
				{
					float alreadyHeld = false;
					for ( uint32_t j = 0; j < levelSystem->GetCurrentLevel()->GetNumOfLevelCubes(); j++ )
						if ( i != j && levelSystem->GetCurrentLevel()->GetCube()[j]->GetIsHolding() == true )
							alreadyHeld = true;

					// Cube throwing
					if ( me.GetType() == MouseBinds["Fire_Tool_Alt"] && !alreadyHeld && levelSystem->GetCurrentLevel()->GetCube()[i]->GetIsInRange() && canHover && !isPaused &&
						!levelSystem->GetCurrentLevel()->GetCube()[i]->GetIsDissCube() && levelSystem->GetCurrentLevel()->GetCube()[i]->GetIsHolding() &&
						( levelSystem->GetCurrentLevel()->GetCube()[i]->GetIsHovering() || levelSystem->GetCurrentLevel()->GetCube()[i]->GetIsHolding() ) )
					{
						canHover = false;
						XMFLOAT3 cubeForce = levelSystem->GetCurrentLevel()->GetCube()[i]->GetPhysicsModel()->Normalization(
							XMFLOAT3( sinf( levelSystem->GetCurrentLevel()->GetCube()[i]->GetRotationFloat3().y ),
								-( cameras->GetCamera( cameras->GetCurrentCamera() )->GetRotationFloat3().x + cameras->GetCamera( cameras->GetCurrentCamera() )->GetRotationFloat3().z ) / 2.0f,
								cosf( levelSystem->GetCurrentLevel()->GetCube()[i]->GetRotationFloat3().y ) )
						);

						levelSystem->GetCurrentLevel()->GetCube()[i]->GetPhysicsModel()->AddForce( cubeForce.x * 20.0f, cubeForce.y * 20.0f, cubeForce.z * 20.0f );

						Sound::Instance()->PlaySoundEffect( "CubeThrow" );
					}
				}

#pragma region Tool_Picking
				// Test intersection between crosshair and cube
				if ( mousePick.TestIntersection( levelSystem->GetCurrentLevel()->GetGraphics()->GetWidth() / 2, levelSystem->GetCurrentLevel()->GetGraphics()->GetHeight() / 2, *levelSystem->GetCurrentLevel()->GetCube()[i] ) )
					levelSystem->GetCurrentLevel()->GetCube()[i]->SetIsHovering( true );
				else
					levelSystem->GetCurrentLevel()->GetCube()[i]->SetIsHovering( false );

				// Update box texture on click while hovering
				if ( me.GetType() == MouseBinds["Fire_Tool"] && levelSystem->GetCurrentLevel()->GetCube()[i]->GetIsHovering() )
					EventSystem::Instance()->AddEvent( EVENTID::ChangeCubeEvent, levelSystem->GetCurrentLevel()->GetCube()[i]->GetEditableProperties().get() );
#pragma endregion
			}

#pragma region UI_Input
			// UI mouse input
			{
				if ( me.GetType() == Mouse::MouseEvent::EventType::Move ) {
					UiMouseData.Pos = { static_cast<float>( me.GetPosX() ),static_cast<float>( me.GetPosY() ) };
				}

				if ( me.GetType() == Mouse::MouseEvent::EventType::RPress && cursorEnabled )
					UiMouseData.RPress = true;
				else if ( me.GetType() == Mouse::MouseEvent::EventType::RRelease && cursorEnabled )
					UiMouseData.RPress = false;

				if ( me.GetType() == Mouse::MouseEvent::EventType::LPress && cursorEnabled )
					UiMouseData.LPress = true;
				else if ( me.GetType() == Mouse::MouseEvent::EventType::LRelease && cursorEnabled )
					UiMouseData.LPress = false;

				if ( me.GetType() == Mouse::MouseEvent::EventType::MPress && cursorEnabled )
					UiMouseData.MPress = true;
				else if ( me.GetType() == Mouse::MouseEvent::EventType::MRelease && cursorEnabled )
					UiMouseData.MPress = false;

				EventSystem::Instance()->AddEvent( EVENTID::UIMouseInput, &UiMouseData );
			}
#pragma endregion
		}
	}
}