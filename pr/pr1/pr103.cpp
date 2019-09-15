#include <iostream>

using namespace std;

int main(int argc, char const *argv[]) {

  int a, b;
  int *p;
  //Print out a and p’s memory addresses.
  cout << "a@: " << &a << endl;
  cout << "*p@: " << &p << endl;
  p = &a;
  //Print out a and p’s memory addresses.
  cout << "a@: " << &a << endl;
  cout << "*p@: " << &p << endl;
  //Print out the contents of these memory addresses.
  cout << "a = " << a << endl;
  cout << "p = " << p << endl;

  return 0;
}
