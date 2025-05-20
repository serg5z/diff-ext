#include "settings.h"
#include "util.h"
#include "comparewithmrucommand.h"
#include "clearmrucommand.h"
#include "mruenumerator.h"


MRUEnumerator::MRUEnumerator() {  
}

IFACEMETHODIMP 
MRUEnumerator::Next(ULONG celt, IExplorerCommand** rgelt, ULONG* pceltFetched) {
  ULONG fetched = 0;
  const auto& mru = getMRU();
  
  while(fetched < celt && _index < mru.size()) {
    auto cmd = Make<CompareWithMRUCommand>(get_shortened_display_path(mru[_index]), _index);

    if(!cmd) 
      return E_OUTOFMEMORY;

    rgelt[fetched++] = cmd.Detach();
    ++_index;
  }
  
  if(fetched < celt && _index == mru.size()) {
    auto cmd = Make<ClearMRUCommand>();

    if(!cmd)
      return E_OUTOFMEMORY;

    rgelt[fetched++] = cmd.Detach();
    ++_index;
  }

  if(pceltFetched) 
    *pceltFetched = fetched;

  return (fetched == celt) ? S_OK : S_FALSE;
}

IFACEMETHODIMP 
MRUEnumerator::Skip(ULONG celt) { 
  _index += celt; 
  
  return (_index >= getMRU().size()) ? S_FALSE : S_OK; 
}

IFACEMETHODIMP 
MRUEnumerator::Reset() { 
  _index = 0; 
  
  return S_OK; 
}

IFACEMETHODIMP 
MRUEnumerator::Clone(IEnumExplorerCommand** ppEnum) {
  auto clone = Make<MRUEnumerator>();
  
  if (!clone) return E_OUTOFMEMORY;

  clone->_index = _index;
  *ppEnum = clone.Detach();

  return S_OK;
}
