#pragma once

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <comdef.h>
#include <wrl.h>
#include <wrl\wrappers\corewrappers.h>
#include <vector>
#include <string>


using namespace Microsoft::WRL;

class MRUEnumerator : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IEnumExplorerCommand> {
  ULONG _index = 0;
public:
  MRUEnumerator();
  IFACEMETHODIMP Next(ULONG, IExplorerCommand**, ULONG*) override;
  IFACEMETHODIMP Skip(ULONG) override;
  IFACEMETHODIMP Reset() override;
  IFACEMETHODIMP Clone(IEnumExplorerCommand**) override;
};
