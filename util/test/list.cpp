#include <iostream>

#include "util/cont/list.h"

using namespace std;

template <class T>
void
print_list(LIST<T>& list) {
  typename LIST<T>::ITERATOR i(list);
  int n = 0;
  
  cout << "LIST: " << endl;
  while(!i.done()) {
    cout << "#" << n << ": " << (*i)->data() << endl;
    n++;
    i++;
  }
  cout << "END OF LIST." << endl;
}

int
main() {
  LIST<int> list;
  
  print_list(list);
  
  list.append(10);
  print_list(list);

  list.append(20);
  print_list(list);

  list.prepend(30);
  print_list(list);

  list.prepend(20);
  print_list(list);

  list.append(40);
  print_list(list);
  
  LIST<int>::ITERATOR i(list);
  
  while(!i.done()) {
    LIST<int>::NODE* node = (*i);
    i++;
    if(node->data() == 20) {      
      list.remove(node);
    }
  }
  
  print_list(list);

  i = LIST<int>::ITERATOR(list);
  
  while(!i.done()) {
    LIST<int>::NODE* node = (*i);
    i++;
    list.remove(node);
  }

  print_list(list);
  
  return 0;
}
