#pragma once
#include "UI.h"
using namespace std;


struct ImageData
{
	string Name;
	string FileName;
	XMFLOAT2 Size;
};


class Credits_UI :
    public UI
{
public:
	Credits_UI();
	~Credits_UI();


	void Inizalize(ID3D11Device* device, ID3D11DeviceContext* contex, ConstantBuffer<CB_VS_matrix_2D>* cb_vs_matrix_2d, std::shared_ptr<Fonts> fonts);
	void Update(float dt);
	void BeginDraw(VertexShader& vert, PixelShader& pix, XMMATRIX WorldOrthMatrix, ConstantBuffer<CB_PS_scene>* _cb_ps_scene);
	void TextLoad();
	void HandleEvent(Event* event);
private:

	void AddtoEvent();
	void RemoveFromEvent();
	void LoadImages();

	void AddImmage();
	void AddText();
private:

	Image_Widget Images[10];
	ColourBlock Background;
	vector<TextToDraw> _TextList;
	vector<ImageData> _ImageList;

	
	vector < JSON::TextData > CreditsINFO;
	UINT32 LevelTo = 2;
	float NextY = 0;
	float CurrentYPos;
	int imagecount = 0;
};
