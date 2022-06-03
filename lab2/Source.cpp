#include<iostream>
#include<windows.h>
using namespace std;

struct Args {
	double* koef;
	int deg;
	double x;
	double y;
};

void polynom(Args& args)
{
	int y = 0;
	for (int i = 0; i <= args.deg; i++) {
		y += args.koef[i] * pow(args.x, i);
		Sleep(15);
	}

	cout << "p(" << args.x << ")= " << y << endl;
	args.y = y;
}

int main() {

	cout << "degChisl=";
	int degChisl;
	cin >> degChisl;

	double* koefChisl = new double[degChisl + 1];
	cout << "koefChisl:" << endl;
	for (int i = 0; i <= degChisl; i++)
		cin >> koefChisl[i];

	cout << "degZnam=";
	int degZnam;
	cin >> degZnam;
	double* koefZnam = new double[degZnam + 1];
	cout << "koefZnam:" << endl;
	for (int i = 0; i <= degZnam; i++)
		cin >> koefZnam[i];

	cout << "x=";
	double x;
	cin >> x;
	Args args1;
	args1.koef = koefChisl;
	args1.deg = degChisl;
	args1.x = x;
	HANDLE hPolynom1;
	DWORD IDhPolynom1;
	hPolynom1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)polynom, &args1, 0, &IDhPolynom1);
	if (hPolynom1 == NULL)
		return GetLastError();

	Args args2;
	args2.koef = koefZnam;
	args2.deg = degZnam;
	args2.x = x;
	HANDLE hPolynom2;
	DWORD IDhPolynom2;
	hPolynom2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)polynom, &args2, 0, &IDhPolynom2);
	if (hPolynom2 == NULL)
		return GetLastError();
	HANDLE handles[2] = { hPolynom1, hPolynom2 };
	WaitForMultipleObjects(2, handles, TRUE, INFINITE);
	CloseHandle(hPolynom1);
	CloseHandle(hPolynom2);

	double result = args1.y / args2.y;

	cout << "p(" << x << ")= " << result << endl;

	return 0;
}