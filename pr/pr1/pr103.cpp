#include <iostream>

using namespace std;

int main(int argc, char const *argv[]) {

  int a, b;
  int *p;
  cout << "a@: " << &a << endl;
  cout << "*p@: " << &p << endl;
  p = &a;
  cout << "*p@: " << &p << endl;
  cout << "b@: " << &b << endl;

  return 0;
}
