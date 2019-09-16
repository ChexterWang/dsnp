#include<iostream>

using namespace std;

int main(){

	int a = 10;
	int &r = a;
	cout << "address of a: " << &a << endl;
	cout << "address of r: " << &r << endl;
	int b = 20;
	r = b;
	cout << "a, b, r: " << a << b << r << endl;

	cout << "address of a: " << &a << endl;
	cout << "address of b: " << &b << endl;
	cout << "address of r: " << &r << endl;

	return 0;

}
