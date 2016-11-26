#include "stdafx.h"
#include "DXTextRenderer.h"
#include "Graphics\ImageBMP.h"


CDXTextRenderer::CDXTextRenderer(CDXManager * pManager, CDXBasicPainter * pPainter)
{
	m_pOwner = pManager;
	m_pPainter = pPainter;
	m_pSRVFont = nullptr;
}

CDXTextRenderer::~CDXTextRenderer()
{
}

CImageBMP::PIXEL AlphaGen(CImageBMP::PIXEL &p)
{
	CImageBMP::PIXEL r = p;
	r.a = p.r;
	return r;
}

bool CDXTextRenderer::Initialize()
{
	auto bmp = CImageBMP::CreateBitmapFromFile("..\\Assets\\font.bmp", AlphaGen);
	if (bmp)
	{
		auto text = bmp->CreateTexture(m_pOwner);
		CImageBMP::DestroyBitmap(bmp);
		m_pOwner->GetDevice()->CreateShaderResourceView(text, nullptr, &m_pSRVFont);

		D3D11_BLEND_DESC dbd;
		memset(&dbd, 0, sizeof(dbd));
		dbd.AlphaToCoverageEnable = false;
		dbd.IndependentBlendEnable = false;
		dbd.RenderTarget[0].BlendEnable = true;
		dbd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		// Color = ColorSrc * (alpha source) + colorDest * (1 - alpha source)
		dbd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		dbd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		dbd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		//Alpha = AlphaSrc * (alpha source) + AlphaDest * (1 - alpha source)
		dbd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		dbd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		// Modificar todos los canales (rgba)
		dbd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		m_pOwner->GetDevice()->CreateBlendState(&dbd, &m_pBS);
		return true;
	}
	return false;
}

void CDXTextRenderer::RenderText(MATRIX4D W, const char * pszText)
{
	//CDXBasicPainter::VERTEX Frame[4] = 
	CDXBasicPainter::VERTEX Frame[4]
	{
		{ { -1,1,0,1 }, { 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } },
		{ { 1,1,0,1  }, { 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
		{ { -1,-1,0,1}, { 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
		{ { 1,-1,0,1 }, { 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
	};

	unsigned long FrameIndex[6] { 0,1,2,2,1,3 };
	m_pPainter->m_Params.World = W;
	m_pOwner->GetContext()->PSSetShaderResources(3, 1, &m_pSRVFont);
	m_pPainter->m_Params.Flags = MAPPING_EMISSIVE;
	m_pOwner->GetContext()->OMSetBlendState(m_pBS, nullptr, -1);
	//printing text
	while (*pszText)
	{
		// 1. Calcular las coordenadas de textura correspondiente al codigo ascii *pszText
		int i, j;
		i = (unsigned char)*pszText & 0x0f;
		j = ((unsigned char)*pszText & 0xf0) >> 4;

		Frame[0].TexCoord.x = i / 16.0f;
		Frame[0].TexCoord.y = j / 16.0f;
		Frame[1].TexCoord.x = (i + 1) / 16.0f;
		Frame[1].TexCoord.y = j / 16.0f;
		Frame[2].TexCoord.x = i / 16.0f;
		Frame[2].TexCoord.y = (j + 1) / 16.0f;
		Frame[3].TexCoord.x = (i + 1) / 16.0f;
		Frame[3].TexCoord.y = (j + 1) / 16.0f;

		VECTOR4D Derecha = { 1.5, 0, 0, 0 };
		m_pPainter->DrawIndexed(Frame, 4, FrameIndex, 6);

		for (int i = 0; i < 4; i++)
		{
			Frame[i].Position = Frame[i].Position + Derecha;
		}
		pszText++;
	}
}

void CDXTextRenderer::Uninitialize()
{
	SAFE_RELEASE(m_pSRVFont);
}
