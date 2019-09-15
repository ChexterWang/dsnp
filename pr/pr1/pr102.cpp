#include <iostream>
#include <typeinfo>

using namespace std;

int main()
{
  int a, b, c;

  while(cin >> a >> b)
  {
    cout << typeid(a).name() << endl;
    cout << typeid(b).name() << endl;
    cout << typeid(c).name() << endl;

    int *p = &a;
    int *q = &b;
    int *r = &c;

    int &d = *p; //if it is not referring, it wont change with a, b, c
    int &e = *q;
    int &f = *r;
    //Try to print out a,b,c’s memory addresses.
    cout << "&a = " << p << endl;
    cout << "&b = " << q << endl;
    cout << "&c = " << r << endl;
    //Print out the “content” of these addresses
    cout << "content@ &a: " << d << endl;
    cout << "content@ &b: " << e << endl;
    cout << "content@ &c: " << f << endl;
    //Operate on these variables. Print out their memory addresses and contents again.
    c = a + b;
    cout << "---------" << endl;
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << "c = " << c << endl;
    cout << "&a = " << p << endl;
    cout << "&b = " << q << endl;
    cout << "&c = " << r << endl;
    cout << "content@ &a: " << d << endl;
    cout << "content@ &b: " << e << endl;
    cout << "content@ &c: " << f << endl;
    cout << "=========" << endl;

  }

  return 0;

}
