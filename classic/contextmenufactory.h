#pragma once

#include <Windows.h>
#include <wrl.h>
#include <wrl/implements.h>

using namespace Microsoft::WRL;

class ContextMenuFactory : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IClassFactory>
{
public:
  IFACEMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override;
  IFACEMETHODIMP LockServer(BOOL fLock) override;
};

