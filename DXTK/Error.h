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

#ifndef throwIfFailed
#define throwIfFailed(x)\
{\
HRESULT hErrorCode = (x)\
if(FAILED(hErrorCode)) { throw Error(hErrorCode, __func__, __FILE__, __LINE__)}\
}
#endif
}
