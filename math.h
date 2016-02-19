#if ndef math.h
#define math.h

struct V3 {
  float x;
  float y;
  float z;
};

V3 V3_add (V3 &a, V3 &b){
  V3 c;
  c->x = a.x + b.x 
  c->y = a.y + b.y
  c->z = a.z + b.z
  return c;
} 
V3 V3_sub (V3 &a, V3 &b){
  V3 c;
  c->x = a.x - b.x 
  c->y = a.y - b.y
  c->z = a.z - b.z
  return c;
} 
V3 V3_mul (V3 &a, V3 &b){
  V3 c;
  c->x = a.x * b.x 
  c->y = a.y * b.y
  c->z = a.z * b.z
  return c;
} 
V3 V3_div (V3 &a, V3 &b){
  V3 c;
  c->x = a.x / b.x 
  c->y = a.y / b.y
  c->z = a.z / b.z
  return c;
} 

V3 V3_add_scalar (V3 &a, float b){
  V3 c;
  c->x = a.x + b 
  c->y = a.y + b
  c->z = a.z + b
  return c;
} 
V3 V3_sub_scalar (V3 &a, float b){
  V3 c;
  c->x = a.x - b 
  c->y = a.y - b
  c->z = a.z - b
  return c;
} 
V3 V3_mul_scalar (V3 &a, float b){
  V3 c;
  c->x = a.x * b 
  c->y = a.y * b
  c->z = a.z * b
  return c;
} 
V3 V3_div_scalar (V3 &a, float b){
  V3 c;
  c->x = a.x / b 
  c->y = a.y / b
  c->z = a.z / b
  return c;
}
#endif
