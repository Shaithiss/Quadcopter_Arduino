#include <V3.h>
#include <math.h>
float x, y, z;

public V3(float x, float y, float z){
  this.x = x;
  this.y = y;
  this.z = z;
}
public V3 Add_V3 (V3 other){
  V3 result;
  result.x = this.x + other.x;
  result.y = this.y + other.y;
  result.z = this.z + other.z;
  return result:
}

public V3 Add_Scalar (float other){
  V3 result;
  result.x = this.x + other;
  result.y = this.y + other;
  result.z = this.z + other;
  return result:
}

public V3 Sub_V3 (V3 other){
  V3 result;
  result.x = this.x - other.x;
  result.y = this.y - other.y;
  result.z = this.z - other.z;
  return result:
}

public V3 Sub_Scalar (float other){
  V3 result;
  result.x = this.x - other;
  result.y = this.y - other;
  result.z = this.z - other;
  return result:
}

public V3 Mul_V3 (V3 other){
  V3 result;
  result.x = this.x * other.x;
  result.y = this.y * other.y;
  result.z = this.z * other.z;
  return result:
}

public V3 Mul_Scalar (float other){
  V3 result;
  result.x = this.x * other;
  result.y = this.y * other;
  result.z = this.z * other;
  return result:
}

public V3 Div_V3 (V3 other){
  V3 result;
  result.x = this.x / other.x;
  result.y = this.y / other.y;
  result.z = this.z / other.z;
  return result:
}

public V3 Div_Scalar (float other){
  V3 result;
  result.x = this.x / other;
  result.y = this.y / other;
  result.z = this.z / other;
  return result:
}

public V3 CrossProduct (V3 other){
  V3 result;
  result.x = this.y*other.z - this.z*other.y;
  result.y = this.z*other.x - this.x*other.z;
  result.z = this.x*other.y - this.y*other.x;
  return result;
}

public float ScalarProduct( V3 other){
  float result;
  result = this.x*other.x + this.y*other.y + this.z*other.zM
  return result;
}

public float Length(){
  float result;
  result = sqrt(this.x^2 + this.y^2 + this.z^2);
  return result;
}
