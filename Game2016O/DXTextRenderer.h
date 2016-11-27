#pragma once

#include "Graphics\DXManager.h"
#include "Graphics\DXBasicPainter.h"

class CDXTextRenderer
{
protected:
	CDXManager *m_pOwner;
	CDXBasicPainter *m_pPainter;
	ID3D11ShaderResourceView *m_pSRVFont;
	ID3D11BlendState *m_pBS; // Blend state
public:
	CDXTextRenderer(CDXManager *pManager, CDXBasicPainter *pPainter);
	~CDXTextRenderer();
	bool Initialize();
	void RenderText(MATRIX4D W, const char * pszText, VECTOR4D color); //la transformacion es sobre la primer letra izquierda
	void Uninitialize();
};

