#include "common.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dwrite.lib")

namespace foldscape
{
	InputFlags::InputFlags()
	{
		flags = 0;
	}

	InputFlags::InputFlags(WPARAM wparam)
	{
		flags = GET_KEYSTATE_WPARAM(wparam);
	}

	ComPtr<ID3DBlob> CompileShaderFromString(const char* shaderCode, const char* entry, const char* target, const D3D_SHADER_MACRO* defines)
	{
#ifdef _DEBUG
		ComPtr<ID3DBlob> byteCode, errors;
		HRESULT shaderCompileResult = D3DCompile(
			shaderCode, std::strlen(shaderCode),
			nullptr, defines, nullptr, entry, target,
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
			&byteCode, &errors);
		if (errors)
			OutputDebugStringA(static_cast<LPCSTR>(errors->GetBufferPointer()));

		ThrowIfFailed(shaderCompileResult);

		return byteCode;
#else
		ComPtr<ID3DBlob> byteCode;
		ThrowIfFailed(D3DCompile(
			shaderCode, std::strlen(shaderCode),
			nullptr, defines, nullptr, entry, target,
			D3DCOMPILE_OPTIMIZATION_LEVEL3, 0,
			&byteCode, nullptr));

		return byteCode;
#endif
	}

	ComPtr<ID3DBlob> CompileShaderFromFile(const wchar_t* filename, const char* entry, const char* target, const D3D_SHADER_MACRO* defines)
	{
#ifdef _DEBUG
		ComPtr<ID3DBlob> byteCode, errors;
		HRESULT shaderCompileResult = D3DCompileFromFile(
			filename, defines, nullptr, entry, target,
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
			&byteCode, &errors);
		if (errors)
			OutputDebugStringA(static_cast<LPCSTR>(errors->GetBufferPointer()));

		ThrowIfFailed(shaderCompileResult);

		return byteCode;
#else
		ComPtr<ID3DBlob> byteCode;
		ThrowIfFailed(D3DCompileFromFile(
			filename, defines, nullptr, entry, target,
			D3DCOMPILE_OPTIMIZATION_LEVEL3, 0,
			&byteCode, nullptr));

		return byteCode;
#endif

	}

	ComPtr<ID3DBlob> LoadBinary(const wchar_t* filename)
	{
		std::ifstream fin(filename, std::ios::binary);
		ThrowIfFailed(fin.is_open() ? S_OK : E_FAIL);
		fin.seekg(0, std::ios_base::end);
		std::ifstream::pos_type size = fin.tellg();
		fin.seekg(0, std::ios_base::beg);

		ComPtr<ID3DBlob> blob;
		ThrowIfFailed(D3DCreateBlob(size, &blob));

		fin.read(reinterpret_cast<char*>(blob->GetBufferPointer()), size);
		fin.close();

		return blob;
	}

	void _ThrowIfFailed(HRESULT hr, const char* file, size_t line, const char* what)
	{
		if (FAILED(hr))
		{
			std::stringstream ss;
			ss << "Error in file: \"" << file << "\" at line " << line <<
				". Error code: " << std::system_category().message(hr) <<
				" (0x" << std::hex << hr << ")\n" << what;
			throw std::exception(ss.str().c_str());
		}
	}
}