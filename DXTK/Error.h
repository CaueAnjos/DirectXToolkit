#pragma once
#include "Types.h"

namespace dxtk
{
	class Error
	{
	public:
		Error();
		Error(HRESULT error, const String& refFuntion, const String& refFile, int line);
		String description() const;

	protected:
		HRESULT hErrorCode;
		String sFunction;
		String sFile;
		int nLine;
	};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                               \
{                                                                      \
    HRESULT hr = (x);                                                  \
    if(FAILED(hr)) { throw dxtk::Error(hr, __func__, __FILE__, __LINE__); }  \
}
#endif
}
