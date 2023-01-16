#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <dxgi1_6.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <wrl.h>
#undef min
#undef max
#undef CreateFont
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <functional>

#include "math/position.hpp"

namespace foldscape
{
	union InputFlags
	{
		struct
		{
			unsigned leftButton : 1;	// 0x0001
			unsigned rightButton : 1;	// 0x0002
			unsigned shift : 1;			// 0x0004
			unsigned ctrl : 1;			// 0x0008
			unsigned middleButton : 1;	// 0x0010
			unsigned xbutton1 : 1;		// 0x0020
			unsigned xbutton2 : 1;		// 0x0040
		};
		unsigned flags;

		InputFlags();
		InputFlags(WPARAM wparam);
	};

	enum class MouseButtonType
	{
		Left,
		Middle,
		Right,
		XButton1,
		XButton2
	};

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3DBlob> CompileShaderFromString(const char* shaderCode, const char* entry, const char* target, const D3D_SHADER_MACRO* defines = nullptr);
	ComPtr<ID3DBlob> CompileShaderFromFile(const wchar_t* filename, const char* entry, const char* target, const D3D_SHADER_MACRO* defines = nullptr);
	ComPtr<ID3DBlob> LoadBinary(const wchar_t* filename);

	void _ThrowIfFailed(HRESULT hr, const char* file, size_t line, const char* what);
#define ThrowIfFailed(hr) ::foldscape::_ThrowIfFailed(hr, __FILE__, __LINE__, #hr)
	inline void SafeDestroyWindow(HWND& hwnd)
	{
		if (hwnd)
		{
			DestroyWindow(hwnd);
			hwnd = nullptr;
		}
	}
}