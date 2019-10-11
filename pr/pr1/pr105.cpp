#include <iostream>
#include "C.h"
using namespace std;

int main(){
  C x;
  C y;
  C z;
  cout << "x address: " << &x << endl;
  cout << "y address: " << &y << endl;
  cout << "z address: " << &z << endl;
  int *a1 = new int;
  int *a2 = new int;
  C   *c1 = new C;
  C   *c2 = new C;
  C   *c3 = new C;
  cout << "a1 address: " << &a1 << endl;
  cout << "a2 address: " << &a2 << endl;
  cout << "c1 address: " << &c1 << endl;
  cout << "c2 address: " << &c2 << endl;
  cout << "c3 address: " << &c3 << endl;
  cout << "a1 = " << a1 << endl;
  cout << "a2 = " << a2 << endl;
  cout << "c1 = " << c1 << endl;
  cout << "c2 = " << c2 << endl;
  cout << "c3 = " << c3 << endl;

}
