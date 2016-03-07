#include <Arduino.h>
#include <Vector3.h>
#include <math.h>

Vector3::Vector3(){
	_x = 0;
	_y = 0;
	_z = 0;
}

Vector3::Vector3(float x, float y, float z){
	_x = x;
	_y = y;
	_z = z;
}

Vector3::~Vector3(){}

float Vector3::x(){
	return _x;
}

float Vector3::y(){
	return _y;
}

float Vector3::z(){
	return _z;
}

void Vector3::x(float x){
	_x = x;
}

void Vector3::y(float y){
	_y = y;
}

void Vector3::z(float z){
	_z = z;
}
void Vector3::Add_V3(Vector3 B){
	_x = _x + B.x;
	_y = _y	+ B.y;
	_z = _z + B.z;
}
void Vector3::Add_Scalar(float B){
	_x = _x + B;
	_y = _y + B;
	_z = _z + B;
}
void Vector3::Sub_V3(Vector3 B){
	_x = _x - B.x;
	_y = _y - B.y;
	_z = _z - B.z;
}
void Vector3::Sub_Scalar(float B){
	_x = _x - B;
	_y = _y - B;
	_z = _z - B;
}
void Vector3::Mul_V3(Vector3 B){
	_x = _x * B.x;
	_y = _y * B.y;
	_z = _z * B.z;
}
void Vector3::Mul_Scalar(float B){
	_x = _x * B;
	_y = _y * B;
	_z = _z * B;
}
void Vector3::Div_V3(Vector3 B){
	_x = _x / B.x;
	_y = _y / B.y;
	_z = _z / B.z;
}
void Vector3::Div_Scalar(float B){
	_x = _x / B;
	_y = _y / B;
	_z = _z / B;
}
void Vector3::Crossproduct(Vector3 B){
	_x = _y * B.z + _z * B.y;
	_y = _z * B.x + _x * B.z;
	_z = _x * B.y + _y * B.x;
}

float Vector3::Scalarproduct(Vector3 B){
	float result = _x * B.x + _y * B.y + _z * B.z;
	return result;
}

float Vector3::Length(){
	float result = sqrt(square(_x) + square(_y) + square(_z));
	return result;
}