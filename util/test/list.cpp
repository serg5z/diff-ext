#include <iostream>

#include "util/cont/list.h"

using namespace std;

template <class T>
void
print_list(LIST<T>& list) {
  typename LIST<T>::NODE* current = list.begin();
  int i = 0;
  
  cout << "LIST: " << endl;
  while(!list.is_last(current)) {
    cout << "#" << i << ": " << current->data() << endl;
    i++;
    current = current->next();
  }
  cout << "END OF LIST." << endl;
}

int
main() {
  LIST<int> list;
  
  print_list(list);
  
  list.add(10);
  print_list(list);

  list.add(20);
  print_list(list);

  list.add(30);
  print_list(list);

  list.add(20);
  print_list(list);

  list.add(40);
  print_list(list);
  
  LIST<int>::NODE* current = list.begin();
  
  while(!list.is_last(current)) {
    if(current->data() == 20) {
      list.remove(current);
    }
    
    current = current->next();
  }
  
  print_list(list);

  current = list.begin();
  
  while(!list.is_last(current)) {
    list.remove(current);
    
    current = current->next();
  }

  print_list(list);
  
  return 0;
}
