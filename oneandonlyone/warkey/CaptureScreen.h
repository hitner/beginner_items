#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>

class CaptureScreen
{
public:
  CaptureScreen();
  bool CreateDevice();
  void MakeReady();
  static void Capture();
  bool _Capture();
public:
  IDXGIAdapter* m_dxgiAdapter = nullptr;
  ID3D11DeviceContext* m_deviceContext = nullptr;
  IDXGIFactory* m_dxgiFactory = nullptr;
  ID3D11Device* m_device;
  ID3D11DeviceContext* m_context;
  IDXGIOutputDuplication* m_outputDuplication;
  ID3D11Texture2D* m_lastTexture = nullptr;
};

