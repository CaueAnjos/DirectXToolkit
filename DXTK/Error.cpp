#include "Error.h"
#include <comdef.h> 

namespace dxtk
{
	Error::Error()
		: hErrorCode(S_OK), nLine(-1)
	{}

	Error::Error(HRESULT error, const String & refFuntion, const String & refFile, int line)
		: hErrorCode(error), sFunction(refFuntion), sFile(refFile), nLine(line)
	{
		size_t pos = refFile.find_last_of('\\');

		if(pos != String::npos)
		{
			sFile = refFile.substr(pos + 1);
		}
	}

	String Error::description() const
	{
		_com_error err(hErrorCode);

		std::stringstream text;
		text << sFunction
			<< " falhou em " << sFile
			<< ", linha " << std::to_string(nLine)
			<< ":\n" << err.ErrorMessage();

		return text.str();
	}
}
