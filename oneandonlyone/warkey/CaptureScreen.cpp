#include "stdafx.h"
#include "CaptureScreen.h"

void CaptureScreen::MakeReady()
{
  IDXGIDevice* dxgiDevice = 0;
  IDXGIAdapter* adapter = 0;
  IDXGIOutput* dxgiOutput = 0;
  IDXGIOutput1* output1 = nullptr;

  HRESULT hr = S_OK;
  bool bOk = false;

  //setCurrentInputDesktop();
  {
    m_device->GetImmediateContext(&m_context);
    if (!SUCCEEDED(hr = m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)))
    {
      //LOG(ERROR) << "QueryInterface IDXGIDevice failed, error: " << std::hex << hr;
      goto EXIT;
    }
  }

  if (!SUCCEEDED(hr = dxgiDevice->GetAdapter(&adapter)))
  {
    //LOG(ERROR) << "Device GetAdapter failed, error: " << std::hex << hr;
    goto EXIT;
  }

  if (!SUCCEEDED(hr = adapter->EnumOutputs(0, &dxgiOutput)))
  {
    //LOG(ERROR) << "Adapter EnumOutputs failed, error: " << std::hex << hr;
    goto EXIT;
  }

  if (!SUCCEEDED(hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output1)))
  {
    //LOG(ERROR) << "QueryInterface IDXGIOutput1 failed, error: " << std::hex << hr;
    goto EXIT;
  }

  if (!SUCCEEDED(hr = output1->DuplicateOutput(m_device, &m_outputDuplication)))
  {

    goto EXIT;
  }


  bOk = true;

EXIT:
  if (output1)
    output1->Release();

  if (dxgiOutput)
    dxgiOutput->Release();

  if (adapter)
    adapter->Release();

  if (dxgiDevice)
    dxgiDevice->Release();

  //return bOk;
}

void CaptureScreen::Capture()
{
  static CaptureScreen * cs = nullptr;
  if (!cs) {
    cs = new CaptureScreen();
  }
  cs->_Capture();
  
}

bool CaptureScreen::_Capture()
{
  DXGI_OUTDUPL_FRAME_INFO frameInfo;
  IDXGIResource* tempResource;
  HRESULT hRes = m_outputDuplication->AcquireNextFrame(10, &frameInfo, &tempResource);
  if (hRes == DXGI_ERROR_ACCESS_LOST)
  {
    //LOG(ERROR) << "AcquireNextFrame DXGI_ERROR_ACCESS_LOST";
    return false;
  }
  else if (hRes == DXGI_ERROR_WAIT_TIMEOUT)
  {
    return false;
  }
  else if (FAILED(hRes))
  {

    return false;
  }

  ID3D11Texture2D* texVal;
  if (!SUCCEEDED(tempResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texVal))) {
    return false;
  }

  if (frameInfo.AccumulatedFrames >= 2) {
    
  }

  D3D11_TEXTURE2D_DESC srcDesc;
  texVal->GetDesc(&srcDesc);
  D3D11_TEXTURE2D_DESC dstDesc = srcDesc;
  dstDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  dstDesc.Usage = D3D11_USAGE_DEFAULT;
  dstDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

  if (m_lastTexture) {
    m_lastTexture->Release();
    m_lastTexture = nullptr;
  }

  m_device->CreateTexture2D(&dstDesc, NULL, &m_lastTexture);
  m_context->CopyResource(m_lastTexture, texVal);
  return true;
}

CaptureScreen::CaptureScreen()
{
  CreateDevice();
  MakeReady();
}

bool CaptureScreen::CreateDevice()
{
  HMODULE hD3D11 = ::LoadLibrary(L"D3D11.dll");
  if (!hD3D11)
  {
    return false;
  }

  PFN_D3D11_CREATE_DEVICE pfnD3D11CreateDevice = (PFN_D3D11_CREATE_DEVICE)::GetProcAddress(hD3D11, "D3D11CreateDevice");
  if (!pfnD3D11CreateDevice)
  {
    return false;
  }

  D3D_FEATURE_LEVEL featureLevels[] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1,
  };
  int nFeatureLevelsCount = ARRAYSIZE(featureLevels);
  D3D_FEATURE_LEVEL receivedLevel;

  D3D_DRIVER_TYPE driverTypes[] = {
    D3D_DRIVER_TYPE_HARDWARE,
    D3D_DRIVER_TYPE_WARP,
    D3D_DRIVER_TYPE_REFERENCE,
    D3D_DRIVER_TYPE_SOFTWARE,
    D3D_DRIVER_TYPE_UNKNOWN,
    D3D_DRIVER_TYPE_NULL
  };
  int nDriverTypesCount = ARRAYSIZE(driverTypes);
  for (int i = 0; i < nDriverTypesCount; i++)
  {
    HRESULT hr = pfnD3D11CreateDevice(NULL, driverTypes[i], NULL, 0, featureLevels,
      nFeatureLevelsCount, D3D11_SDK_VERSION, &m_device, &receivedLevel, &m_deviceContext);
    if (SUCCEEDED(hr))
    {
      IDXGIDevice* dxgiDevice = 0;
      m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
      if (dxgiDevice)
      {
        dxgiDevice->GetAdapter(&m_dxgiAdapter);
        if (m_dxgiAdapter)
          m_dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgiFactory);
        dxgiDevice->Release();
        dxgiDevice = 0;
      }
      break;
    }
  }


  if (!m_device)
  {
    return false;
  }

  return true;
}