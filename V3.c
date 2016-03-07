#include <V3.h>
#include <math.h>

public V3 V3_Add_V3 (V3 A, V3 B){
  V3 result;
  result.x = A.x + B.x;
  result.y = A.y + B.y;
  result.z = A.z + B.z;
  return result:
}

public V3 V3_Add_Scalar (V3 A, float B){
  V3 result;
  result.x = A.x + B;
  result.y = A.y + B;
  result.z = A.z + B;
  return result:
}

public V3 V3_Sub_V3 (V3 A, V3 B){
  V3 result;
  result.x = A.x - B.x;
  result.y = A.y - B.y;
  result.z = A.z - B.z;
  return result:
}

public V3 V3_Sub_Scalar (V3 A, float B){
  V3 result;
  result.x = A.x - B;
  result.y = A.y - B;
  result.z = A.z - B;
  return result:
}

public V3 V3_Mul_V3 (V3 A, V3 B){
  V3 result;
  result.x = A.x * B.x;
  result.y = A.y * B.y;
  result.z = A.z * B.z;
  return result:
}

public V3 V3_Mul_Scalar (V3 A, float B){
  V3 result;
  result.x = A.x * B;
  result.y = A.y * B;
  result.z = A.z * B;
  return result:
}

public V3 V3_Div_V3 (V3 A, V3 B){
  V3 result;
  result.x = A.x / B.x;
  result.y = A.y / B.y;
  result.z = A.z / B.z;
  return result:
}

public V3 V3_Div_Scalar (V3 A, float B){
  V3 result;
  result.x = A.x / B;
  result.y = A.y / B;
  result.z = A.z / B;
  return result:
}

public V3 V3_CrossProduct (V3 A, V3 B){
  V3 result;
  result.x = A.y*B.z - A.z*B.y;
  result.y = A.z*B.x - A.x*B.z;
  result.z = A.x*B.y - A.y*B.x;
  return result;
}

public V3 V3calarProduct(V3 A,  V3 B){
  float result;
  result = A.x*B.x + A.y*B.y + A.z*B.zM
  return result;
}

public float V3_Length(V3 A){
  float result;
  result = sqrt(A.x^2 + A.y^2 + A.z^2);
  return result;
}
