/*
 * $Id$
 */
 
#include "class_factory.h"
#include "diff_ext.h"

extern "C" void inc_cRefThisDLL();
extern "C" void dec_cRefThisDLL();

ExtClassFactory::ExtClassFactory() {
  _ref_count = 0L;

  inc_cRefThisDLL();
}

ExtClassFactory::~ExtClassFactory() {
  dec_cRefThisDLL();
}

STDMETHODIMP 
ExtClassFactory::QueryInterface(REFIID riid, void** ppv) {
  HRESULT ret = E_NOINTERFACE;
  *ppv = 0;

  if(IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory)) {
    *ppv = static_cast<ExtClassFactory*>(this);

    AddRef();

    ret = NOERROR;
  }

  return ret;
}

STDMETHODIMP_(ULONG) 
ExtClassFactory::AddRef() {
  return InterlockedIncrement((LPLONG)&_ref_count);
}

STDMETHODIMP_(ULONG) 
ExtClassFactory::Release() {
  ULONG ret = 0L;
  
  if(InterlockedDecrement((LPLONG)&_ref_count) != 0)
    ret = _ref_count;
  else
    delete this;

  return ret;
}

STDMETHODIMP 
ExtClassFactory::CreateInstance(IUnknown* outer, REFIID refiid, void** obj) {
  HRESULT ret = CLASS_E_NOAGGREGATION;
  *obj = 0;

  // Shell extensions typically don't support aggregation (inheritance)
  if(outer == 0) {
    CShellExt* diff_ext = new CShellExt();
  
    if(diff_ext == 0)
      ret = E_OUTOFMEMORY;    
    else
      ret = diff_ext->QueryInterface(refiid, obj);
  }
  
  return ret;
}

STDMETHODIMP 
ExtClassFactory::LockServer(BOOL fLock) {
  return NOERROR;
}
