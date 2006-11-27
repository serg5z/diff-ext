#include <util/resource_string.h>

void
load_resource_string(HINSTANCE  resource, UINT string_id, TCHAR* string, int length, TCHAR* default_value) {
  int resource_string_length;
  
  resource_string_length = LoadString(resource, string_id, string, length);
  
  if(resource_string_length == 0) {
    resource_string_length = LoadString(GetModuleHandle(0), string_id, string, length);
    
    if(resource_string_length == 0) {
      lstrcpy(string, default_value);
    }
  }
}

