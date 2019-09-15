#include <iostream>
#include <typeinfo>

using namespace std;

void main()
{
  int a, b, c;
  while(cin >> a >> b)
  {
    cout << typeid(a).name() << ", " << tyepid(b).name() << ", " << typeid(c).name() << endl;

    int *p = &a;
    int *q = &b;
    int *r = &c;

    int d = *p;
    int e = *q;
    int f = *r;
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
    cout << "&a = " << p << endl;
    cout << "&b = " << q << endl;
    cout << "&c = " << r << endl;
    cout << "content@ &a: " << d << endl;
    cout << "content@ &b: " << e << endl;
    cout << "content@ &c:" << f << endl;
    cout << "=========" << endl;

  }
}
