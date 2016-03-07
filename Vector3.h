/*LIbrary for Trygonometry Calculations*/

#ifndef Vector3_h
#define Vector3_h

#include <Arduino.h>

class Vector3{
public:
	Vector3();
	Vector3(float, float, float);
	~Vector3();
	float x();
	float y();
	float z();
	void x(float);
	void y(float);
	void z(float);
	void Add_V3(Vector3 B);
	void Add_Scalar(float B);
	void Sub_V3(Vector3 B);
	void Sub_Scalar(float B);
	void Mul_V3(Vector3 B);
	void Mul_Scalar(float B);
	void Div_V3(Vector3 B);
	void Div_Scalar(float B);
	void Crossproduct(Vector3 B);
	float Scalarproduct(Vector3 B);
	float Length();
private:
	float _x;
	float _y;
	float _z;
};
#endif