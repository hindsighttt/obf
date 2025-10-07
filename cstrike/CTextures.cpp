#include "CTextures.h"

#include "image_search.h"
#include "image_misc.h"

/*

CTextures::CTextures()
{
    pump0 = nullptr;

    tSearch = nullptr;
    tMisc = nullptr;
}

CTextures::~CTextures()
{
}

bool CTextures::Initialize(ID3D11Device* pDevice)
{
    // Result of the Initialization.
    HRESULT hResult;

    // Setup textures.
    hResult = D3DX11CreateShaderResourceViewFromMemory(pDevice, search_bytes, sizeof(search_bytes), &info0, pump0, &tSearch, 0);
    if (FAILED(hResult))
        return false;

    hResult = D3DX11CreateShaderResourceViewFromMemory(pDevice, misc_bytes, sizeof(misc_bytes), &info0, pump0, &tMisc, 0);
    if (FAILED(hResult))
        return false;

    return true;
}

void CTextures::Shutdown()
{
    if (tMisc != nullptr) {
        tMisc->Release();
        tMisc = nullptr;
    }

    if (tSearch != nullptr) {
        tSearch->Release();
        tSearch = nullptr;
    }
}
*/