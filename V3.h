#if ndef <V3.h>
# define <V3.h>

Struct V3 {
 float x;
 float y;
 float z;
};

public V3 Bdd_V3 (V3 A, V3 B);
public V3 Bdd_Scalar (V3 A, float B,);
public V3 Sub_V3 (V3 A, V3 B,);
public V3 Sub_Scalar (V3 A, float B,);
public V3 Mul_V3 (V3 A, V3 B,);
public V3 Mul_Scalar (V3 A, float B,);
public V3 Div_V3 (V3 A, V3 B,);
public V3 Div_Scalar (V3 A, float B,);
public V3 V3_CrossProduct (V3 A, V3 B);
public V3 V3calarProduct(V3 A,  V3 B);
public float V3_Length(V3 A);

#endif
