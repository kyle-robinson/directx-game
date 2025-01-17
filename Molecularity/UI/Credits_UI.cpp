#include "stdafx.h"
#include "Credits_UI.h"

Credits_UI::Credits_UI()
{ }

Credits_UI::~Credits_UI()
{
	RemoveFromEvent();
}

void Credits_UI::Initialize( ID3D11Device* device, ID3D11DeviceContext* contex, ConstantBuffer<CB_VS_matrix_2D>* cb_vs_matrix_2d, std::shared_ptr<Fonts> fonts )
{
	AddtoEvent();
	UI::Initialize( device, contex, cb_vs_matrix_2d, fonts );
	FontsList->AddFont( "OpenSans_20", "OpenSans_20.spritefont" );
	Background.INITSprite( _Contex.Get(), _Device.Get(), *_cb_vs_matrix_2d );

	TextLoad();
	LoadImages();

	// Start Position
	NextY = _SizeOfScreen.y;
}

void Credits_UI::Update( float dt )
{
	_TextList.clear();
	//background 
	Background.Function( { 235,209,240 }, { _SizeOfScreen.x,_SizeOfScreen.y }, { 0,0 }, 1.0f );


	CurrentYPos = NextY;
	NextY -= 0.1f * dt;
	AddText();

	// Check if the credits have finished
	if ( CurrentYPos < 0.0f )
		EventSystem::Instance()->AddEvent( EVENTID::GameLevelChangeEvent, &LevelTo );

	if ( ( 1 << 15 ) & GetAsyncKeyState( VK_RETURN ) )
		EventSystem::Instance()->AddEvent( EVENTID::GameLevelChangeEvent, &LevelTo );

}

void Credits_UI::BeginDraw( VertexShader& vert, PixelShader& pix, XMMATRIX WorldOrthMatrix, ConstantBuffer<CB_PS_scene>* _cb_ps_scene )
{
	Background.Draw( _Contex.Get(), _Device.Get(), *_cb_ps_scene, *_cb_vs_matrix_2d, WorldOrthMatrix );

	for ( uint32_t i = 0u; i < imagecount; i++ )
		Images[i].Draw( _Contex.Get(), _Device.Get(), *_cb_ps_scene, *_cb_vs_matrix_2d, WorldOrthMatrix );

	imagecount = 0;

	for ( auto& Text : _TextList )
		FontsList->GetFont( "OpenSans_20" )->RenderString( Text._Text, Text._Position, Text._Colour );
}

void Credits_UI::TextLoad()
{
	CreditsINFO = TextLoader::Instance()->LoadText( "Credits" );
}

void Credits_UI::HandleEvent( Event* event )
{
	switch ( event->GetEventID() )
	{
	case EVENTID::WindowSizeChangeEvent:
	{
		_SizeOfScreen = *static_cast<XMFLOAT2*>( event->GetData() );
	}
	break;
	}
}

void Credits_UI::AddtoEvent()
{
	EventSystem::Instance()->AddClient( EVENTID::WindowSizeChangeEvent, this );
}

void Credits_UI::RemoveFromEvent()
{
	EventSystem::Instance()->RemoveClient(EVENTID::WindowSizeChangeEvent, this);
}

void Credits_UI::LoadImages()
{
	// Image data
	ImageData IData;
	if ( !_ImageList.empty() )
		_ImageList.clear();

	IData.Name = "Headder";
	IData.FileName = "Title_Card\\TitleCard.png";
	IData.Size = { static_cast<float>( _SizeOfScreen.x * 0.5f ),static_cast<float>( _SizeOfScreen.y * 0.12f ) };
	_ImageList.push_back( IData );

	IData.Name = "DissCube";
	IData.FileName = "Credits\\Disscube.png";
	IData.Size = { static_cast<float>( _SizeOfScreen.x * 0.20f ),static_cast<float>( _SizeOfScreen.y * 0.24f ) };
	_ImageList.push_back( IData );

	IData.Name = "Dimond";
	IData.FileName = "Settings\\Slider_Yellow.dds";
	IData.Size = { static_cast<float>( _SizeOfScreen.x * 0.05f ),static_cast<float>( _SizeOfScreen.y * 0.05f ) };
	_ImageList.push_back( IData );

	for ( uint32_t i = 0u; i < _ImageList.size(); i++ )
		Images[i].INITSprite( _Contex.Get(), _Device.Get(), *_cb_vs_matrix_2d );
}

void Credits_UI::AddImmage()
{
	if ( imagecount < _ImageList.size() )
	{
		// Insert image
		Images[imagecount].Function( _ImageList[imagecount].FileName, _ImageList[imagecount].Size, { ( _SizeOfScreen.x / 2 ) - ( _ImageList[imagecount].Size.x / 2 ),CurrentYPos } );
		CurrentYPos += _ImageList[imagecount].Size.y + 10.0f;
		imagecount++;
	}
}

void Credits_UI::AddText()
{
	TextToDraw TextForDraw;
	TextForDraw._Colour = DirectX::Colors::Black;
	for ( auto& Text : CreditsINFO )
	{
		XMVECTOR textsize = { 0.0f, 0.0f, 0.0f };
		if ( Text.Name != "" )
		{
			textsize = FontsList->GetFont( "OpenSans_20" )->GetSpriteFont()->MeasureString( Text.Name.c_str() );
			TextForDraw._Position = { ( ( _SizeOfScreen.x / 2.0f ) - ( XMVectorGetX( textsize ) * FontsList->GetFont( "OpenSans_20" )->GetScale().x ) ) - 50.0f, CurrentYPos };
			TextForDraw._Text = Text.Name;
			_TextList.push_back( TextForDraw );

			TextForDraw._Position = { ( _SizeOfScreen.x / 2.0f ) + 50.0f, CurrentYPos };
			TextForDraw._Text = Text.Text;
			_TextList.push_back( TextForDraw );
		}
		else
		{
			AddImmage();
			textsize = FontsList->GetFont( "OpenSans_20" )->GetSpriteFont()->MeasureString( Text.Text.c_str() );
			TextForDraw._Position = { ( _SizeOfScreen.x / 2.0f ) - ( ( XMVectorGetX( textsize ) / 2.0f ) * FontsList->GetFont( "OpenSans_20" )->GetScale().x ), CurrentYPos };
			TextForDraw._Text = Text.Text;
			_TextList.push_back( TextForDraw );
		}
		CurrentYPos += ( XMVectorGetY( textsize ) * FontsList->GetFont( "OpenSans_20" )->GetScale().y );
	}
}