#if ndef <V3.h>
# define <V3.h>

Struct V3 {
 float x;
 float y;
 float z;
};

V3 Add_V3 (V3 other);
V3 Add_Scalar (float other);
V3 Sub_V3 (V3 other);
V3 Sub_Scalar (float other);
V3 Mul_V3 (V3 other);
V3 Mul_Scalar (float other);
V3 Div_V3 (V3 other);
V3 Div_Scalar (float other);

V3 CrossProduct (V3 other);
float SkalarProduct (V3 other);
#endif
