#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <wrl.h>

using namespace Microsoft::WRL;

template <typename T>
class ClassFactory : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IClassFactory> {
  public:
    IFACEMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override {
      if(pUnkOuter) return CLASS_E_NOAGGREGATION;

      ComPtr<T> obj = Make<T>();
      return obj.CopyTo(riid, ppvObject);
    }

    IFACEMETHODIMP LockServer(BOOL lock) override {
      OutputDebugStringW((L"DiffExt: DLL_PROCESS_DETACH: " + std::to_wstring(lock)).c_str());

      return S_OK;
    }
};
