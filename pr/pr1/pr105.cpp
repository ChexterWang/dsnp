#include <iostream>
#include "C.h"
using namespace std;

int main(){
  C x;
  C y;
  C z;
  cout << sizeof(x) << endl;
  cout << "x address: " << &x << endl;
  cout << "y address: " << &y << endl;
  cout << "z address: " << &z << endl;
}
