#pragma once

// The renderer is shared by both mobile ports. Android's NDK defines
// ANDROID; keep that legacy branch active locally on iOS as well, while using
// Apple's OpenGLES headers and without leaking the macro to other headers.
#if defined(__IOS__)
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
    #include <stdarg.h>
    #include <stdio.h>
    #define LUPUS2_GRP_OPENGL_IOS_ANDROID_ALIAS 1
    #define ANDROID 1
    #define ANDROID_LOG_ERROR 6
    static inline int Lupus2IOSOpenGLLog(int, const char*, const char* format, ...)
    {
        va_list arguments;
        va_start(arguments, format);
        const int result = vfprintf(stderr, format, arguments);
        fputc('\n', stderr);
        va_end(arguments);
        return result;
    }
    #define __android_log_print Lupus2IOSOpenGLLog
#elif defined(ANDROID)
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <android/log.h>
    #include <IL/il.h>
#else
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

#include <math.h>
#include <string.h>
#include <vector>

// Direct3D 8 compatibility declarations required by the legacy engine.  The
// original shared archive declared several of these after their first use,
// which no conforming Android compiler accepts.
#define LUPUS2_D3D_CORE_TYPES 1
typedef int D3DFORMAT;
typedef int D3DPOOL;
typedef int D3DDEVTYPE;
typedef int D3DMULTISAMPLE_TYPE;
typedef int D3DBACKBUFFER_TYPE;
typedef int D3DPRIMITIVETYPE;
typedef int D3DTEXTURESTAGESTATETYPE;

#ifndef D3DFMT_A8R8G8B8
#define D3DFMT_A8R8G8B8 21
#endif

struct D3DVIEWPORT8 {
    DWORD X, Y, Width, Height;
    float MinZ, MaxZ;
};

struct D3DADAPTER_IDENTIFIER8 {
    char Driver[512];
    char Description[512];
    union {
        struct { DWORD LowPart; LONG HighPart; };
        int64_t QuadPart;
    } DriverVersion;
};

struct D3DDISPLAYMODE {
    UINT Width, Height, RefreshRate;
    D3DFORMAT Format;
};

struct D3DCAPS8 {
    DWORD Caps2, DevCaps, PrimitiveMiscCaps, TextureCaps;
    DWORD TextureAddressCaps, VertexProcessingCaps;
    DWORD MaxTextureWidth, MaxTextureHeight;
    DWORD VertexShaderVersion;
    DWORD MaxAnisotropy;
    DWORD TextureFilterCaps;
};

struct D3DPRESENT_PARAMETERS {
    UINT BackBufferWidth, BackBufferHeight;
    D3DFORMAT BackBufferFormat;
    UINT BackBufferCount;
    int MultiSampleType;
    int SwapEffect;
    HWND hDeviceWindow;
    int Windowed;
    BOOL EnableAutoDepthStencil;
    D3DFORMAT AutoDepthStencilFormat;
    DWORD Flags;
    UINT FullScreen_RefreshRateInHz;
    UINT FullScreen_PresentationInterval;
};

struct D3DLOCKED_RECT {
    int Pitch;
    void* pBits;
};
struct D3DSURFACE_DESC { UINT Width; UINT Height; D3DFORMAT Format; };

struct IDirect3DDevice8;
struct IDirect3DTexture8;
struct IDirect3DSurface8;
struct IDirect3DBaseTexture8;
struct IDirect3DVertexBuffer8;
struct IDirect3DIndexBuffer8;
struct D3DXIMAGE_INFO;
typedef IDirect3DDevice8* LPDIRECT3DDEVICE8;
typedef IDirect3DTexture8* LPDIRECT3DTEXTURE8;
typedef IDirect3DTexture8* LPDIRECT3DBASETEXTURE8;
typedef IDirect3DSurface8* LPDIRECT3DSURFACE8;
typedef IDirect3DTexture8* LPDIRECT3DBaseTexture8;
typedef IDirect3DVertexBuffer8* LPDIRECT3DVERTEXBUFFER8;
typedef IDirect3DIndexBuffer8* LPDIRECT3DINDEXBUFFER8;

#ifdef ANDROID
IDirect3DDevice8* Lupus2CreateGLESDevice();
IDirect3DTexture8* Lupus2CreateGLESTexture(unsigned int width, unsigned int height);
IDirect3DTexture8* Lupus2CreateGLESTextureFromMemory(const void* data, unsigned int size,
                                                      D3DXIMAGE_INFO* imageInfo);
IDirect3DVertexBuffer8* Lupus2CreateGLESVertexBuffer(unsigned int length, bool dynamic);
IDirect3DIndexBuffer8* Lupus2CreateGLESIndexBuffer(unsigned int length);
GLuint Lupus2GLESVertexBufferHandle(IDirect3DVertexBuffer8* buffer);
GLuint Lupus2GLESIndexBufferHandle(IDirect3DIndexBuffer8* buffer);
const void* Lupus2GLESVertexBufferData(IDirect3DVertexBuffer8* buffer);
const void* Lupus2GLESIndexBufferData(IDirect3DIndexBuffer8* buffer);
unsigned int Lupus2GLESVertexBufferLength(IDirect3DVertexBuffer8* buffer);
unsigned int Lupus2GLESIndexBufferLength(IDirect3DIndexBuffer8* buffer);
GLuint Lupus2GLESTextureHandle(IDirect3DTexture8* texture);
bool Lupus2GLESTextureHasMipmaps(IDirect3DTexture8* texture);
int AndroidGetVirtualWidth();
int AndroidGetVirtualHeight();
int AndroidGetSurfaceWidth();
int AndroidGetSurfaceHeight();
#endif

#ifndef HRESULT
typedef long HRESULT;
#endif
#ifndef S_OK
#define S_OK ((HRESULT)0L)
#endif
#ifndef FAILED
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif
#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

#ifdef ANDROID
typedef const void* LPCVOID;
typedef void* LPVOID;
typedef struct { BYTE peRed; BYTE peGreen; BYTE peBlue; BYTE peFlags; } PALETTEENTRY;
#endif

typedef unsigned int UINT;
typedef uint32_t DWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef int BOOL;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

struct D3DXIMAGE_INFO {
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    int Format;
    int ResourceType;
    int ImageFileFormat;
};

// Simple D3DX Math simulation for OpenGL
struct GLMATRIX {
    union {
        float m[4][4];
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
    };
    
    GLMATRIX() {
        memset(m, 0, sizeof(m));
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
    }
    GLMATRIX& operator*=(const GLMATRIX& rhs) {
        GLMATRIX lhs = *this;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = lhs.m[i][0] * rhs.m[0][j] + lhs.m[i][1] * rhs.m[1][j] +
                          lhs.m[i][2] * rhs.m[2][j] + lhs.m[i][3] * rhs.m[3][j];
        return *this;
    }
};

typedef GLMATRIX D3DMATRIX;

struct GLVECTOR3 {
    float x, y, z;
    
    GLVECTOR3() : x(0), y(0), z(0) {}
    GLVECTOR3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    GLVECTOR3 operator+(const GLVECTOR3& rhs) const { return GLVECTOR3(x+rhs.x, y+rhs.y, z+rhs.z); }
    GLVECTOR3 operator-(const GLVECTOR3& rhs) const { return GLVECTOR3(x-rhs.x, y-rhs.y, z-rhs.z); }
    GLVECTOR3 operator-() const { return GLVECTOR3(-x, -y, -z); }
    GLVECTOR3 operator*(float s) const { return GLVECTOR3(x*s, y*s, z*s); }
    GLVECTOR3 operator/(float s) const { return GLVECTOR3(x/s, y/s, z/s); }
    bool operator==(const GLVECTOR3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    bool operator!=(const GLVECTOR3& rhs) const { return !(*this == rhs); }
    GLVECTOR3& operator+=(const GLVECTOR3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    GLVECTOR3& operator-=(const GLVECTOR3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
    GLVECTOR3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    GLVECTOR3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }
};

inline GLVECTOR3 operator*(float s, const GLVECTOR3& v) {
    return GLVECTOR3(v.x * s, v.y * s, v.z * s);
}

inline GLVECTOR3* D3DXVec3Normalize(GLVECTOR3* pOut, const GLVECTOR3* pV) {
    float len = sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
    if (len > 0.0f) {
        pOut->x = pV->x / len;
        pOut->y = pV->y / len;
        pOut->z = pV->z / len;
    } else {
        pOut->x = pOut->y = pOut->z = 0.0f;
    }
    return pOut;
}
inline GLVECTOR3* D3DXVec3Scale(GLVECTOR3* pOut, const GLVECTOR3* pV, float scale) {
    pOut->x = pV->x * scale;
    pOut->y = pV->y * scale;
    pOut->z = pV->z * scale;
    return pOut;
}
inline GLVECTOR3* D3DXVec3Add(GLVECTOR3* pOut, const GLVECTOR3* a, const GLVECTOR3* b) {
    pOut->x = a->x + b->x;
    pOut->y = a->y + b->y;
    pOut->z = a->z + b->z;
    return pOut;
}

struct GLVECTOR2 {
    float x, y;
    GLVECTOR2() : x(0), y(0) {}
    GLVECTOR2(float _x, float _y) : x(_x), y(_y) {}
};

struct GLCOLOR {
    float r, g, b, a;
    GLCOLOR() : r(0), g(0), b(0), a(0) {}
    GLCOLOR(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
    GLCOLOR(DWORD argb)
        : r(((argb >> 16) & 0xff) / 255.0f),
          g(((argb >> 8) & 0xff) / 255.0f),
          b((argb & 0xff) / 255.0f),
          a(((argb >> 24) & 0xff) / 255.0f) {}
    GLCOLOR& operator=(DWORD argb) {
        r = ((argb >> 16) & 0xff) / 255.0f;
        g = ((argb >> 8) & 0xff) / 255.0f;
        b = (argb & 0xff) / 255.0f;
        a = ((argb >> 24) & 0xff) / 255.0f;
        return *this;
    }
    operator DWORD() const {
        const DWORD aa = (DWORD)(a <= 0 ? 0 : a >= 1 ? 255 : a * 255.0f);
        const DWORD rr = (DWORD)(r <= 0 ? 0 : r >= 1 ? 255 : r * 255.0f);
        const DWORD gg = (DWORD)(g <= 0 ? 0 : g >= 1 ? 255 : g * 255.0f);
        const DWORD bb = (DWORD)(b <= 0 ? 0 : b >= 1 ? 255 : b * 255.0f);
        return (aa << 24) | (rr << 16) | (gg << 8) | bb;
    }
};

inline GLCOLOR* D3DXColorModulate(GLCOLOR* out, const GLCOLOR* left, const GLCOLOR* right) {
    if (!out || !left || !right) return out;
    out->r = left->r * right->r;
    out->g = left->g * right->g;
    out->b = left->b * right->b;
    out->a = left->a * right->a;
    return out;
}

struct GLVECTOR4 {
    float x, y, z, w;
    GLVECTOR4() : x(0), y(0), z(0), w(0) {}
    GLVECTOR4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};
struct GLQUATERNION {
    float x, y, z, w;
    GLQUATERNION() : x(0), y(0), z(0), w(1) {}
    GLQUATERNION(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};
struct GLPLANE {
    float a, b, c, d;
    GLPLANE() : a(0), b(0), c(0), d(0) {}
    GLPLANE(float _a, float _b, float _c, float _d) : a(_a), b(_b), c(_c), d(_d) {}
};

inline float D3DXVec3Dot(const GLVECTOR3* pV1, const GLVECTOR3* pV2) {
    return pV1->x*pV2->x + pV1->y*pV2->y + pV1->z*pV2->z;
}
inline GLVECTOR3* D3DXVec3Cross(GLVECTOR3* pOut, const GLVECTOR3* pV1, const GLVECTOR3* pV2) {
    pOut->x = pV1->y * pV2->z - pV1->z * pV2->y;
    pOut->y = pV1->z * pV2->x - pV1->x * pV2->z;
    pOut->z = pV1->x * pV2->y - pV1->y * pV2->x;
    return pOut;
}
inline GLVECTOR3* D3DXVec3Lerp(GLVECTOR3* pOut, const GLVECTOR3* a, const GLVECTOR3* b, float t) {
    pOut->x = a->x + (b->x - a->x) * t;
    pOut->y = a->y + (b->y - a->y) * t;
    pOut->z = a->z + (b->z - a->z) * t;
    return pOut;
}
inline float D3DXVec3LengthSq(const GLVECTOR3* pV) {
    return pV->x*pV->x + pV->y*pV->y + pV->z*pV->z;
}
inline float D3DXVec3Length(const GLVECTOR3* pV) {
    return sqrtf(D3DXVec3LengthSq(pV));
}
inline float D3DXVec2Length(const GLVECTOR2* pV) {
    return sqrtf(pV->x*pV->x + pV->y*pV->y);
}
inline float D3DXVec2Dot(const GLVECTOR2* pV1, const GLVECTOR2* pV2) {
    return pV1->x*pV2->x + pV1->y*pV2->y;
}
inline GLVECTOR2* D3DXVec2Normalize(GLVECTOR2* pOut, const GLVECTOR2* pV) {
    float len = D3DXVec2Length(pV);
    if (len > 0) { pOut->x = pV->x/len; pOut->y = pV->y/len; }
    else { pOut->x = pOut->y = 0; }
    return pOut;
}
inline GLVECTOR3* D3DXVec3TransformCoord(GLVECTOR3* pOut, const GLVECTOR3* pV, const GLMATRIX* pM) {
    const float x = pV->x*pM->_11 + pV->y*pM->_21 + pV->z*pM->_31 + pM->_41;
    const float y = pV->x*pM->_12 + pV->y*pM->_22 + pV->z*pM->_32 + pM->_42;
    const float z = pV->x*pM->_13 + pV->y*pM->_23 + pV->z*pM->_33 + pM->_43;
    const float w = pV->x*pM->_14 + pV->y*pM->_24 + pV->z*pM->_34 + pM->_44;
    const float inverseW = (fabsf(w) > 1.0e-8f) ? 1.0f / w : 1.0f;
    pOut->x = x * inverseW;
    pOut->y = y * inverseW;
    pOut->z = z * inverseW;
    return pOut;
}
inline GLVECTOR3* D3DXVec3TransformNormal(GLVECTOR3* pOut, const GLVECTOR3* pV, const GLMATRIX* pM) {
    GLVECTOR3 result(
        pV->x*pM->_11 + pV->y*pM->_21 + pV->z*pM->_31,
        pV->x*pM->_12 + pV->y*pM->_22 + pV->z*pM->_32,
        pV->x*pM->_13 + pV->y*pM->_23 + pV->z*pM->_33);
    *pOut = result;
    return pOut;
}
inline GLVECTOR4* D3DXVec4Transform(GLVECTOR4* pOut, const GLVECTOR4* v, const GLMATRIX* m) {
    GLVECTOR4 result(
        v->x*m->_11 + v->y*m->_21 + v->z*m->_31 + v->w*m->_41,
        v->x*m->_12 + v->y*m->_22 + v->z*m->_32 + v->w*m->_42,
        v->x*m->_13 + v->y*m->_23 + v->z*m->_33 + v->w*m->_43,
        v->x*m->_14 + v->y*m->_24 + v->z*m->_34 + v->w*m->_44);
    *pOut = result;
    return pOut;
}
inline GLVECTOR4* D3DXVec3Transform(GLVECTOR4* out, const GLVECTOR3* v, const GLMATRIX* m) {
    out->x = v->x*m->_11 + v->y*m->_21 + v->z*m->_31 + m->_41;
    out->y = v->x*m->_12 + v->y*m->_22 + v->z*m->_32 + m->_42;
    out->z = v->x*m->_13 + v->y*m->_23 + v->z*m->_33 + m->_43;
    out->w = v->x*m->_14 + v->y*m->_24 + v->z*m->_34 + m->_44;
    return out;
}
inline GLMATRIX* D3DXMatrixIdentity(GLMATRIX* pOut) {
    memset(pOut->m, 0, sizeof(pOut->m));
    pOut->m[0][0] = pOut->m[1][1] = pOut->m[2][2] = pOut->m[3][3] = 1.0f;
    return pOut;
}
inline GLMATRIX* D3DXMatrixInverse(GLMATRIX* pOut, float* pDeterminant, const GLMATRIX* pM) {
    const float* m = &pM->m[0][0];
    float inv[16];
    inv[0]=m[5]*m[10]*m[15]-m[5]*m[11]*m[14]-m[9]*m[6]*m[15]+m[9]*m[7]*m[14]+m[13]*m[6]*m[11]-m[13]*m[7]*m[10];
    inv[4]=-m[4]*m[10]*m[15]+m[4]*m[11]*m[14]+m[8]*m[6]*m[15]-m[8]*m[7]*m[14]-m[12]*m[6]*m[11]+m[12]*m[7]*m[10];
    inv[8]=m[4]*m[9]*m[15]-m[4]*m[11]*m[13]-m[8]*m[5]*m[15]+m[8]*m[7]*m[13]+m[12]*m[5]*m[11]-m[12]*m[7]*m[9];
    inv[12]=-m[4]*m[9]*m[14]+m[4]*m[10]*m[13]+m[8]*m[5]*m[14]-m[8]*m[6]*m[13]-m[12]*m[5]*m[10]+m[12]*m[6]*m[9];
    inv[1]=-m[1]*m[10]*m[15]+m[1]*m[11]*m[14]+m[9]*m[2]*m[15]-m[9]*m[3]*m[14]-m[13]*m[2]*m[11]+m[13]*m[3]*m[10];
    inv[5]=m[0]*m[10]*m[15]-m[0]*m[11]*m[14]-m[8]*m[2]*m[15]+m[8]*m[3]*m[14]+m[12]*m[2]*m[11]-m[12]*m[3]*m[10];
    inv[9]=-m[0]*m[9]*m[15]+m[0]*m[11]*m[13]+m[8]*m[1]*m[15]-m[8]*m[3]*m[13]-m[12]*m[1]*m[11]+m[12]*m[3]*m[9];
    inv[13]=m[0]*m[9]*m[14]-m[0]*m[10]*m[13]-m[8]*m[1]*m[14]+m[8]*m[2]*m[13]+m[12]*m[1]*m[10]-m[12]*m[2]*m[9];
    inv[2]=m[1]*m[6]*m[15]-m[1]*m[7]*m[14]-m[5]*m[2]*m[15]+m[5]*m[3]*m[14]+m[13]*m[2]*m[7]-m[13]*m[3]*m[6];
    inv[6]=-m[0]*m[6]*m[15]+m[0]*m[7]*m[14]+m[4]*m[2]*m[15]-m[4]*m[3]*m[14]-m[12]*m[2]*m[7]+m[12]*m[3]*m[6];
    inv[10]=m[0]*m[5]*m[15]-m[0]*m[7]*m[13]-m[4]*m[1]*m[15]+m[4]*m[3]*m[13]+m[12]*m[1]*m[7]-m[12]*m[3]*m[5];
    inv[14]=-m[0]*m[5]*m[14]+m[0]*m[6]*m[13]+m[4]*m[1]*m[14]-m[4]*m[2]*m[13]-m[12]*m[1]*m[6]+m[12]*m[2]*m[5];
    inv[3]=-m[1]*m[6]*m[11]+m[1]*m[7]*m[10]+m[5]*m[2]*m[11]-m[5]*m[3]*m[10]-m[9]*m[2]*m[7]+m[9]*m[3]*m[6];
    inv[7]=m[0]*m[6]*m[11]-m[0]*m[7]*m[10]-m[4]*m[2]*m[11]+m[4]*m[3]*m[10]+m[8]*m[2]*m[7]-m[8]*m[3]*m[6];
    inv[11]=-m[0]*m[5]*m[11]+m[0]*m[7]*m[9]+m[4]*m[1]*m[11]-m[4]*m[3]*m[9]-m[8]*m[1]*m[7]+m[8]*m[3]*m[5];
    inv[15]=m[0]*m[5]*m[10]-m[0]*m[6]*m[9]-m[4]*m[1]*m[10]+m[4]*m[2]*m[9]+m[8]*m[1]*m[6]-m[8]*m[2]*m[5];
    const float det=m[0]*inv[0]+m[1]*inv[4]+m[2]*inv[8]+m[3]*inv[12];
    if (pDeterminant) *pDeterminant=det;
    if (fabsf(det)<1.0e-12f) { D3DXMatrixIdentity(pOut); return pOut; }
    const float invDet=1.0f/det;
    for (int i=0;i<16;++i) (&pOut->m[0][0])[i]=inv[i]*invDet;
    return pOut;
}
inline GLMATRIX* D3DXMatrixMultiply(GLMATRIX* pOut, const GLMATRIX* pM1, const GLMATRIX* pM2) {
    GLMATRIX out;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            out.m[i][j] = pM1->m[i][0] * pM2->m[0][j] + pM1->m[i][1] * pM2->m[1][j] +
                          pM1->m[i][2] * pM2->m[2][j] + pM1->m[i][3] * pM2->m[3][j];
        }
    }
    *pOut = out;
    return pOut;
}
inline GLMATRIX operator*(const GLMATRIX& lhs, const GLMATRIX& rhs) {
    GLMATRIX result;
    D3DXMatrixMultiply(&result, &lhs, &rhs);
    return result;
}
inline GLMATRIX* D3DXMatrixTranslation(GLMATRIX* pOut, float x, float y, float z) {
    D3DXMatrixIdentity(pOut);
    pOut->m[3][0] = x; pOut->m[3][1] = y; pOut->m[3][2] = z;
    return pOut;
}
inline GLMATRIX* D3DXMatrixScaling(GLMATRIX* pOut, float x, float y, float z) {
    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = x; pOut->m[1][1] = y; pOut->m[2][2] = z;
    return pOut;
}
inline GLMATRIX* D3DXMatrixRotationX(GLMATRIX* pOut, float angle) {
    float s = sinf(angle), c = cosf(angle);
    D3DXMatrixIdentity(pOut);
    pOut->m[1][1] = c; pOut->m[1][2] = s;
    pOut->m[2][1] = -s; pOut->m[2][2] = c;
    return pOut;
}
inline GLMATRIX* D3DXMatrixRotationY(GLMATRIX* pOut, float angle) {
    float s = sinf(angle), c = cosf(angle);
    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = c; pOut->m[0][2] = -s;
    pOut->m[2][0] = s; pOut->m[2][2] = c;
    return pOut;
}
inline GLMATRIX* D3DXMatrixRotationZ(GLMATRIX* pOut, float angle) {
    float s = sinf(angle), c = cosf(angle);
    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = c; pOut->m[0][1] = s;
    pOut->m[1][0] = -s; pOut->m[1][1] = c;
    return pOut;
}
inline GLMATRIX* D3DXMatrixRotationQuaternion(GLMATRIX* pOut, const GLQUATERNION* q) {
    const float xx = q->x * q->x, yy = q->y * q->y, zz = q->z * q->z;
    const float xy = q->x * q->y, xz = q->x * q->z, yz = q->y * q->z;
    const float xw = q->x * q->w, yw = q->y * q->w, zw = q->z * q->w;
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 1.0f - 2.0f * (yy + zz); pOut->_12 = 2.0f * (xy + zw);        pOut->_13 = 2.0f * (xz - yw);
    pOut->_21 = 2.0f * (xy - zw);        pOut->_22 = 1.0f - 2.0f * (xx + zz); pOut->_23 = 2.0f * (yz + xw);
    pOut->_31 = 2.0f * (xz + yw);        pOut->_32 = 2.0f * (yz - xw);        pOut->_33 = 1.0f - 2.0f * (xx + yy);
    return pOut;
}
inline GLQUATERNION* D3DXQuaternionRotationYawPitchRoll(GLQUATERNION* pOut, float yaw, float pitch, float roll) {
    const float sy = sinf(yaw * 0.5f), cy = cosf(yaw * 0.5f);
    const float sp = sinf(pitch * 0.5f), cp = cosf(pitch * 0.5f);
    const float sr = sinf(roll * 0.5f), cr = cosf(roll * 0.5f);
    pOut->x = sp * cy * cr + cp * sy * sr;
    pOut->y = cp * sy * cr - sp * cy * sr;
    pOut->z = cp * cy * sr - sp * sy * cr;
    pOut->w = cp * cy * cr + sp * sy * sr;
    return pOut;
}
inline GLQUATERNION* D3DXQuaternionRotationAxis(GLQUATERNION* pOut, const GLVECTOR3* axis, float angle) {
    GLVECTOR3 normalized;
    D3DXVec3Normalize(&normalized, axis);
    const float half = angle * 0.5f;
    const float scale = sinf(half);
    pOut->x = normalized.x * scale;
    pOut->y = normalized.y * scale;
    pOut->z = normalized.z * scale;
    pOut->w = cosf(half);
    return pOut;
}
inline GLQUATERNION* D3DXQuaternionMultiply(GLQUATERNION* pOut, const GLQUATERNION* a, const GLQUATERNION* b) {
    GLQUATERNION result(
        a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y,
        a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x,
        a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w,
        a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z);
    *pOut = result;
    return pOut;
}
inline GLQUATERNION* D3DXQuaternionConjugate(GLQUATERNION* pOut, const GLQUATERNION* pQ) {
    *pOut = GLQUATERNION(-pQ->x, -pQ->y, -pQ->z, pQ->w);
    return pOut;
}
inline GLVECTOR3* D3DXVec3Translation(GLVECTOR3* pOut, const GLVECTOR3* pV, const GLMATRIX* pM) { *pOut = *pV; return pOut; }
inline GLPLANE* D3DXPlaneNormalize(GLPLANE* pOut, const GLPLANE* pPlane) {
    const float length = sqrtf(pPlane->a*pPlane->a + pPlane->b*pPlane->b + pPlane->c*pPlane->c);
    if (length > 0.0f) {
        const float inverse = 1.0f / length;
        GLPLANE normalized(pPlane->a * inverse, pPlane->b * inverse, pPlane->c * inverse, pPlane->d * inverse);
        *pOut = normalized;
    } else {
        *pOut = *pPlane;
    }
    return pOut;
}
inline float D3DXPlaneDotCoord(const GLPLANE* pP, const GLVECTOR3* pV) {
    return pP->a*pV->x + pP->b*pV->y + pP->c*pV->z + pP->d;
}
inline float D3DXMatrixfDeterminant(const GLMATRIX* pM) { return 1.0f; }
inline GLMATRIX* D3DXMatrixLookAtRH(GLMATRIX* pOut, const GLVECTOR3* pEye, const GLVECTOR3* pAt, const GLVECTOR3* pUp) {
    GLVECTOR3 f, s, u;
    GLVECTOR3 tmp(pAt->x - pEye->x, pAt->y - pEye->y, pAt->z - pEye->z);
    D3DXVec3Normalize(&f, &tmp);
    D3DXVec3Cross(&s, &f, pUp);
    D3DXVec3Normalize(&s, &s);
    D3DXVec3Cross(&u, &s, &f);

    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = s.x; pOut->m[1][0] = s.y; pOut->m[2][0] = s.z;
    pOut->m[0][1] = u.x; pOut->m[1][1] = u.y; pOut->m[2][1] = u.z;
    pOut->m[0][2] = -f.x; pOut->m[1][2] = -f.y; pOut->m[2][2] = -f.z;
    pOut->m[3][0] = -D3DXVec3Dot(&s, pEye);
    pOut->m[3][1] = -D3DXVec3Dot(&u, pEye);
    pOut->m[3][2] = D3DXVec3Dot(&f, pEye);
    return pOut;
}
inline GLMATRIX* D3DXMatrixPerspectiveFovRH(GLMATRIX* pOut, float fovy, float Aspect, float zn, float zf) {
    float h = 1.0f / tanf(fovy / 2.0f);
    float w = h / Aspect;
    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = w;
    pOut->m[1][1] = h;
    pOut->m[2][2] = zf / (zn - zf);
    pOut->m[2][3] = -1.0f;
    pOut->m[3][2] = (zn * zf) / (zn - zf);
    pOut->m[3][3] = 0.0f;
    return pOut;
}
inline GLMATRIX* D3DXMatrixOrthoRH(GLMATRIX* pOut, float w, float h, float zn, float zf) {
    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = 2.0f / w;
    pOut->m[1][1] = 2.0f / h;
    pOut->m[2][2] = 1.0f / (zn - zf);
    pOut->m[3][2] = zn / (zn - zf);
    return pOut;
}
inline GLMATRIX* D3DXMatrixOrthoOffCenterRH(GLMATRIX* pOut, float l, float r, float b, float t, float zn, float zf) { 
    D3DXMatrixIdentity(pOut);
    if (r != l) pOut->m[0][0] = 2.0f / (r - l);
    if (t != b) pOut->m[1][1] = 2.0f / (t - b);
    if (zn != zf) {
        pOut->m[2][2] = 1.0f / (zn - zf);
        pOut->m[3][2] = zn / (zn - zf);
    }
    if (r != l) pOut->m[3][0] = (l + r) / (l - r);
    if (t != b) pOut->m[3][1] = (t + b) / (b - t);
    return pOut; 
}
inline HRESULT D3DXGetImageInfoFromFileInMemory(LPCVOID pSrcData, UINT SrcDataSize, D3DXIMAGE_INFO* pSrcInfo) {
    if (pSrcInfo) {
        pSrcInfo->Width = 256; pSrcInfo->Height = 256; pSrcInfo->Format = D3DFMT_A8R8G8B8;
    }
    return S_OK;
}
inline GLMATRIX* D3DXMatrixRotationAxis(GLMATRIX* pOut, const GLVECTOR3* pV, float angle) {
    GLVECTOR3 axis;
    D3DXVec3Normalize(&axis, pV);
    const float x = axis.x, y = axis.y, z = axis.z;
    const float s = sinf(angle), c = cosf(angle), t = 1.0f - c;
    D3DXMatrixIdentity(pOut);
    pOut->_11 = t*x*x + c;   pOut->_12 = t*x*y + s*z; pOut->_13 = t*x*z - s*y;
    pOut->_21 = t*x*y - s*z; pOut->_22 = t*y*y + c;   pOut->_23 = t*y*z + s*x;
    pOut->_31 = t*x*z + s*y; pOut->_32 = t*y*z - s*x; pOut->_33 = t*z*z + c;
    return pOut;
}
inline GLMATRIX* D3DXMatrixRotationYawPitchRoll(GLMATRIX* pOut, float y, float p, float r) {
    GLMATRIX my, mp, mr;
    D3DXMatrixRotationY(&my, y);
    D3DXMatrixRotationX(&mp, p);
    D3DXMatrixRotationZ(&mr, r);
    D3DXMatrixMultiply(pOut, &mr, &mp);
    D3DXMatrixMultiply(pOut, pOut, &my);
    return pOut;
}
inline GLVECTOR3* D3DXVec3Project(GLVECTOR3* pOut, const GLVECTOR3* pV, const D3DVIEWPORT8* pViewport, const GLMATRIX* pProjection, const GLMATRIX* pView, const GLMATRIX* pWorld) {
    GLMATRIX mat;
    D3DXMatrixMultiply(&mat, pWorld, pView);
    D3DXMatrixMultiply(&mat, &mat, pProjection);
    GLVECTOR3 ndc;
    D3DXVec3TransformCoord(&ndc, pV, &mat);
    pOut->x = pViewport->X + (1.0f + ndc.x) * pViewport->Width * 0.5f;
    pOut->y = pViewport->Y + (1.0f - ndc.y) * pViewport->Height * 0.5f;
    pOut->z = pViewport->MinZ + ndc.z * (pViewport->MaxZ - pViewport->MinZ);
    return pOut;
}
inline GLVECTOR3* D3DXVec3Unproject(GLVECTOR3* pOut, const GLVECTOR3* pV, const D3DVIEWPORT8* pViewport, const GLMATRIX* pProjection, const GLMATRIX* pView, const GLMATRIX* pWorld) {
    GLMATRIX mat, inverse;
    D3DXMatrixMultiply(&mat, pWorld, pView);
    D3DXMatrixMultiply(&mat, &mat, pProjection);
    D3DXMatrixInverse(&inverse, NULL, &mat);
    GLVECTOR3 ndc;
    ndc.x = ((pV->x - pViewport->X) / pViewport->Width) * 2.0f - 1.0f;
    ndc.y = 1.0f - ((pV->y - pViewport->Y) / pViewport->Height) * 2.0f;
    ndc.z = (pV->z - pViewport->MinZ) / (pViewport->MaxZ - pViewport->MinZ);
    D3DXVec3TransformCoord(pOut, &ndc, &inverse);
    return pOut;
}
inline float D3DXVec2CCW(const GLVECTOR2* pV1, const GLVECTOR2* pV2) {
    return pV1->x * pV2->y - pV1->y * pV2->x;
}
#define D3DX_PI 3.141592654f
#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree( radian ) ((radian) * (180.0f / D3DX_PI))

// Map D3D names to our OpenGL simulation if ENABLE_OPENGL is on
#define D3DX_DEFAULT ((UINT)-1)
#define D3DX_DEFAULT_NONPOW2 ((UINT)-2)

#define D3DFMT_A8R8G8B8 21
#define D3DFMT_X8R8G8B8 22
#define D3DFMT_D16 80
#define D3DPOOL_MANAGED 1
#define D3DX_FILTER_NONE 1
#define D3DX_FILTER_LINEAR 2

// Constants consumed by the compatibility classes below must be visible
// before the inline method bodies (the shared archive put them at EOF).
#define D3DPTADDRESSCAPS_BORDER 0x00000080
#define D3DDEVCAPS_HWTRANSFORMANDLIGHT 0x00000010
#define D3DCAPS2_CANRENDERWINDOWED 0x00080000
#define D3DPMISCCAPS_CLIPTLVERTS 0x00000100
#define D3DVTXPCAPS_DIRECTIONALLIGHTS 0x00000008
#define D3DVTXPCAPS_POSITIONALLIGHTS 0x00000010
#define D3DVTXPCAPS_TEXGEN 0x00000040
#define D3DRS_ZENABLE 7
#define D3DRS_ZWRITEENABLE 14
#define D3DRS_ALPHATESTENABLE 15
#define D3DRS_SRCBLEND 19
#define D3DRS_DESTBLEND 20
#define D3DRS_CULLMODE 22
#define D3DRS_ZFUNC 23
#define D3DRS_ALPHAFUNC 25
#define D3DRS_ALPHABLENDENABLE 27
#define D3DRS_FOGENABLE 28
#define D3DRS_FOGCOLOR 34
#define D3DRS_FOGTABLEMODE 35
#define D3DRS_FOGSTART 36
#define D3DRS_FOGEND 37
#define D3DRS_FOGDENSITY 38
#define D3DRS_RANGEFOGENABLE 48
#define D3DRS_FOGVERTEXMODE 140
#define D3DBLEND_ZERO 1
#define D3DBLEND_ONE 2
#define D3DBLEND_SRCCOLOR 3
#define D3DBLEND_INVSRCCOLOR 4
#define D3DBLEND_SRCALPHA 5
#define D3DBLEND_INVSRCALPHA 6
#define D3DBLEND_DESTALPHA 7
#define D3DBLEND_INVDESTALPHA 8
#define D3DBLEND_DESTCOLOR 9
#define D3DBLEND_INVDESTCOLOR 10
#define D3DBLEND_SRCALPHASAT 11
#define D3DCULL_CW 2
#define D3DCULL_NONE 3

#ifdef ENABLE_OPENGL
    #define D3DXMATRIX GLMATRIX
    #define D3DXVECTOR3 GLVECTOR3
    #define D3DVECTOR GLVECTOR3
    #define D3DXVECTOR2 GLVECTOR2
    #define D3DCOLOR DWORD
    #define D3DXCOLOR GLCOLOR
    
    struct GL_LIGHT {
        int Type;
        GLCOLOR Diffuse;
        GLCOLOR Specular;
        GLCOLOR Ambient;
        GLVECTOR3 Position;
        GLVECTOR3 Direction;
        float Range;
        float Falloff;
        float Attenuation0;
        float Attenuation1;
        float Attenuation2;
        float Theta;
        float Phi;
    };
    #define D3DLIGHT8 GL_LIGHT
    
    struct GL_MATERIAL {
        GLCOLOR Diffuse;
        GLCOLOR Ambient;
        GLCOLOR Specular;
        GLCOLOR Emissive;
        float Power;
    };
    #define D3DMATERIAL8 GL_MATERIAL
    
    #define D3DXQUATERNION GLQUATERNION
    #define D3DXPLANE GLPLANE
    #define D3DXVECTOR4 GLVECTOR4
    
inline HRESULT D3DXCreateTexture(LPDIRECT3DDEVICE8 pDevice, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8* ppTexture) {
        if (!ppTexture) return E_INVALIDARG;
#ifdef ANDROID
        *ppTexture = Lupus2CreateGLESTexture(Width, Height);
        return *ppTexture ? S_OK : E_OUTOFMEMORY;
#else
        *ppTexture = NULL;
        return E_NOTIMPL;
#endif
    }

    inline HRESULT D3DXCreateTextureFromFileInMemoryEx(LPDIRECT3DDEVICE8 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE8* ppTexture) {
        if (!ppTexture || !pSrcData || SrcDataSize == 0) return E_INVALIDARG;
#ifdef ANDROID
        *ppTexture = Lupus2CreateGLESTextureFromMemory(pSrcData, SrcDataSize, pSrcInfo);
        return *ppTexture ? S_OK : E_FAIL;
#else
        *ppTexture = NULL;
        return E_NOTIMPL;
#endif
    }

    inline HRESULT D3DXLoadSurfaceFromSurface(LPDIRECT3DSURFACE8 pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPDIRECT3DSURFACE8 pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey) {
        return S_OK;
    }

    #define D3DVSD_STREAM(s) (s)
    #define D3DVSD_REG(r, t) (r)
    #define D3DVSD_END() 0
    #define D3DVSDT_FLOAT3 0
    #define D3DVSDT_FLOAT2 0
    #define D3DVSDT_FLOAT4 0
    #define D3DVSDT_D3DCOLOR 0
    #define D3DVSDE_POSITION 0
    #define D3DVSDE_NORMAL 3
    #define D3DVSDE_DIFFUSE 5
    #define D3DVSDE_TEXCOORD0 7
    #define D3DVSDE_TEXCOORD1 0
    #define D3DVSDE_TEXCOORD2 9
    #define D3DVSDE_POSITION2 0
    #define D3DVSDE_NORMAL2 0

    // DirectInput Stubs
    typedef void* LPDIRECTINPUT8;
    typedef void* LPDIRECTINPUTDEVICE8;
    #define DIRECTINPUT_VERSION 0x0800
    #define IID_IDirectInput8 (void*)0
    #define GUID_SysKeyboard (void*)0
    #define DISCL_FOREGROUND 0
    #define DISCL_EXCLUSIVE 0
    #define DISCL_NONEXCLUSIVE 0
    #define DISCL_BACKGROUND 0
    #define SAFE_RELEASE(p) if(p) { p = NULL; }
    #define FAILED(hr) ((hr) < 0)
    #define S_OK 0
    inline int DirectInput8Create(void*, int, void*, void**, void*) { return 0; }
    struct IDirectInputDevice8 {
        int SetDataFormat(void*) { return 0; }
        int SetCooperativeLevel(void*, int) { return 0; }
        int Acquire() { return 0; }
        int Unacquire() { return 0; }
        int GetDeviceState(int, void*) { return 0; }
        void Release() {}
    };
    extern void* c_dfDIKeyboard;

    struct IDirect3D8 {
        UINT GetAdapterCount() { return 1; }
        HRESULT GetAdapterIdentifier(UINT, DWORD, D3DADAPTER_IDENTIFIER8* pIdentifier) {
            if (pIdentifier) {
                strcpy(pIdentifier->Driver, "OpenGL Generic");
                strcpy(pIdentifier->Description, "OpenGL Port");
            }
            return S_OK;
        }
        UINT GetAdapterModeCount(UINT) { return 1; }
        HRESULT EnumAdapterModes(UINT, UINT, D3DDISPLAYMODE* pMode) {
            // The detector searches the application's actual surface size.
            if (pMode) { pMode->Width = 2268; pMode->Height = 1017; pMode->RefreshRate = 60; pMode->Format = D3DFMT_X8R8G8B8; }
            return S_OK;
        }
        HRESULT GetAdapterDisplayMode(UINT, D3DDISPLAYMODE* pMode) {
            if (pMode) { pMode->Width = 2268; pMode->Height = 1017; pMode->RefreshRate = 60; pMode->Format = D3DFMT_X8R8G8B8; }
            return S_OK;
        }
        HRESULT CheckDeviceType(UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, BOOL) { return S_OK; }
        HRESULT CheckDeviceFormat(UINT, int, int, DWORD, int, int) { return S_OK; }
        HRESULT CheckDepthStencilMatch(UINT, int, int, int, int) { return S_OK; }
        HRESULT GetDeviceCaps(UINT, int, D3DCAPS8* pCaps) {
            if (pCaps) {
                memset(pCaps, 0, sizeof(D3DCAPS8));
                pCaps->MaxTextureWidth = 8192;
                pCaps->MaxTextureHeight = 8192;
                pCaps->DevCaps = D3DDEVCAPS_HWTRANSFORMANDLIGHT;
                pCaps->Caps2 = D3DCAPS2_CANRENDERWINDOWED;
                pCaps->VertexProcessingCaps = D3DVTXPCAPS_DIRECTIONALLIGHTS | D3DVTXPCAPS_POSITIONALLIGHTS | D3DVTXPCAPS_TEXGEN;
                pCaps->PrimitiveMiscCaps = D3DPMISCCAPS_CLIPTLVERTS;
                pCaps->TextureAddressCaps = D3DPTADDRESSCAPS_BORDER;
            }
            return S_OK;
        }
        HRESULT CreateDevice(UINT, int, HWND hWnd, DWORD, D3DPRESENT_PARAMETERS*, IDirect3DDevice8** ppDevice) {
#ifndef ANDROID
            HDC hDC = GetDC(hWnd);
            static PIXELFORMATDESCRIPTOR pfd = {
                sizeof(PIXELFORMATDESCRIPTOR), 1,
                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 8, 0,
                PFD_MAIN_PLANE, 0, 0, 0, 0
            };
            int pixelFormat = ChoosePixelFormat(hDC, &pfd);
            SetPixelFormat(hDC, pixelFormat, &pfd);
            HGLRC hRC = wglCreateContext(hDC);
            wglMakeCurrent(hDC, hRC);

            *ppDevice = new IDirect3DDevice8();
            (*ppDevice)->m_hDC = hDC;
#else
            *ppDevice = Lupus2CreateGLESDevice();
#endif
            return *ppDevice ? S_OK : E_NOTIMPL;
        }
        ULONG Release() { delete this; return 0; }
    };
    
    inline IDirect3D8* Direct3DCreate8(UINT) { return new IDirect3D8(); }
    struct IDirect3DDevice8 {
#ifndef ANDROID
        HDC m_hDC;
#endif
        ULONG m_refCount;
        GLuint m_program;
        GLint m_positionLocation, m_colorLocation, m_texCoordLocation;
        GLint m_mvpLocation, m_worldViewLocation, m_viewportLocation, m_screenSpaceLocation, m_flipYLocation;
        GLint m_useTextureLocation, m_useTexture1Location, m_alphaRefLocation;
        GLint m_fogEnabledLocation, m_fogRangeModeLocation, m_fogColorLocation, m_fogParametersLocation, m_fogFromDiffuseAlphaLocation;
        GLint m_alphaStageLocation[2], m_textureFactorAlphaLocation, m_textureFactorColorLocation, m_colorArg1Location, m_effectColorModeLocation, m_effectColorOpLocation, m_alphaStage1EnabledLocation;
        float m_alphaStage[2][3]; // ALPHAOP, ALPHAARG1, ALPHAARG2 (legacy mobile enums)
        float m_textureFactorAlpha;
        float m_textureFactorColor[3];
        float m_colorArg1;
        bool m_effectColorMode;
        float m_effectColorOp;
        GLint m_textureMatrix0Location, m_textureMatrix1Location;
        GLint m_generateTexture0Location, m_generateTexture1Location;
        GLint m_transformTexture0Location, m_transformTexture1Location;
        GLMATRIX m_worldMatrix, m_viewMatrix, m_projectionMatrix;
        GLMATRIX m_textureMatrix[2];
        D3DVIEWPORT8 m_d3dViewport;
        bool m_textureEnabled[2];
        bool m_generateTextureCoords[2];
        bool m_transformTextureCoords[2];
        bool m_textureStageActive[2];
        bool m_textureStage1AlphaActive;
        GLuint m_boundTexture[2];
        bool m_boundTextureHasMipmaps[2];
        GLenum m_wrapS[2], m_wrapT[2], m_minFilter[2], m_magFilter[2];
        DWORD m_mipFilter[2];

        void ApplyTextureSampler(DWORD stage) {
#ifdef ANDROID
            if (!m_boundTexture[stage]) return;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS[stage]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT[stage]);
            GLenum minFilter = m_minFilter[stage];
            if (m_boundTextureHasMipmaps[stage] && m_mipFilter[stage] != 0)
            {
                if (m_minFilter[stage] == GL_NEAREST)
                    minFilter = m_mipFilter[stage] == 1
                        ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_LINEAR;
                else
                    minFilter = m_mipFilter[stage] == 1
                        ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter[stage]);
#endif
        }
        bool m_alphaTestEnabled;
        float m_alphaRef;
        bool m_fogEnabled;
        bool m_rangeFogEnabled;
        DWORD m_fogMode;
        float m_fogStart;
        float m_fogEnd;
        float m_fogDensity;
        float m_fogColor[3];
        GLenum m_srcBlendFactor;
        GLenum m_dstBlendFactor;

        GLuint CompileShader(GLenum type, const char* source) {
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &source, NULL);
            glCompileShader(shader);
            GLint ok = GL_FALSE;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
            if (!ok) {
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        }

        void InitializeGLESProgram() {
#ifdef ANDROID
            static const char* vertexSource =
                "attribute vec4 aPosition;\n"
                "attribute vec4 aColor;\n"
                "attribute vec2 aTexCoord;\n"
                "uniform mat4 uMvp;\n"
                "uniform mat4 uWorldView;\n"
                "uniform mat4 uTextureMatrix0;\n"
                "uniform mat4 uTextureMatrix1;\n"
                "uniform vec2 uViewport;\n"
                "uniform float uScreenSpace;\n"
                "uniform float uFlipY;\n"
                "uniform float uGenerateTexture0;\n"
                "uniform float uGenerateTexture1;\n"
                "uniform float uTransformTexture0;\n"
                "uniform float uTransformTexture1;\n"
                "uniform float uFogRangeMode;\n"
                "varying vec4 vColor;\n"
                "varying highp vec2 vTexCoord;\n"
                "varying highp vec2 vTexCoord1;\n"
                "varying highp float vFogDistance;\n"
                "void main(){\n"
                " if(uScreenSpace > 0.5) gl_Position=vec4((aPosition.x/uViewport.x)*2.0-1.0, 1.0-(aPosition.y/uViewport.y)*2.0, aPosition.z, 1.0);\n"
                " else { vec4 p=uMvp*vec4(aPosition.xyz,1.0); if(uFlipY > 0.5) p.y=-p.y; gl_Position=p; }\n"
                " vec4 cameraPosition=uWorldView*vec4(aPosition.xyz,1.0);\n"
                " vFogDistance=mix(abs(cameraPosition.z),length(cameraPosition.xyz),uFogRangeMode);\n"
                " vColor=aColor.bgra;\n"
                " vec4 coord0=(uGenerateTexture0>0.5 ? cameraPosition : vec4(aTexCoord,0.0,1.0));\n"
                " vec4 coord1=(uGenerateTexture1>0.5 ? cameraPosition : vec4(aTexCoord,0.0,1.0));\n"
                " vTexCoord=(uTransformTexture0>0.5 ? uTextureMatrix0*coord0 : coord0).xy;\n"
                " vTexCoord1=(uTransformTexture1>0.5 ? uTextureMatrix1*coord1 : coord1).xy;\n"
                "}\n";
            static const char* fragmentSource =
                "precision highp float;\n"
                "varying vec4 vColor;\n"
                "varying highp vec2 vTexCoord;\n"
                "varying highp vec2 vTexCoord1;\n"
                "varying highp float vFogDistance;\n"
                "uniform sampler2D uTexture;\n"
                "uniform sampler2D uTexture1;\n"
                "uniform float uUseTexture;\n"
                "uniform float uUseTexture1;\n"
                "uniform float uAlphaRef;\n"
                "uniform vec3 uAlphaStage0;\n"
                "uniform vec3 uAlphaStage1;\n"
                "uniform float uAlphaStage1Enabled;\n"
                "uniform float uTextureFactorAlpha;\n"
                "uniform vec3 uTextureFactorColor;\n"
                "uniform float uColorArg1;\n"
                "uniform float uEffectColorMode;\n"
                "uniform float uEffectColorOp;\n"
                "uniform float uFogEnabled;\n"
                "uniform vec3 uFogColor;\n"
                "uniform vec4 uFogParameters;\n"
                "uniform float uFogFromDiffuseAlpha;\n"
                "vec3 effectColor(vec3 factor, vec4 tex, float op){\n"
                " vec3 result;\n"
                " if(op<2.5) result=factor;\n"
                " else if(op<3.5) result=tex.rgb;\n"
                " else if(op<4.5) result=factor*tex.rgb;\n"
                " else if(op<5.5) result=2.0*factor*tex.rgb;\n"
                " else if(op<6.5) result=4.0*factor*tex.rgb;\n"
                " else if(op<7.5) result=factor+tex.rgb;\n"
                " else if(op<8.5) result=factor+tex.rgb-vec3(0.5);\n"
                " else if(op<9.5) result=2.0*(factor+tex.rgb-vec3(0.5));\n"
                " else if(op<10.5) result=factor-tex.rgb;\n"
                " else if(op<11.5) result=factor+tex.rgb-factor*tex.rgb;\n"
                " else if(op<12.5) result=mix(tex.rgb,factor,vColor.a);\n"
                " else if(op<13.5) result=mix(tex.rgb,factor,tex.a);\n"
                " else if(op<14.5) result=mix(tex.rgb,factor,uTextureFactorAlpha);\n"
                " else if(op<15.5) result=factor+tex.rgb*(1.0-tex.a);\n"
                " else if(op<16.5) result=mix(tex.rgb,factor,vColor.a);\n"
                " else if(op<17.5) result=factor*tex.rgb;\n"
                " else if(op<18.5) result=factor*uTextureFactorAlpha+tex.rgb;\n"
                " else if(op<19.5) result=factor*tex.rgb+vec3(uTextureFactorAlpha);\n"
                " else if(op<20.5) result=factor+(1.0-uTextureFactorAlpha)*tex.rgb;\n"
                " else result=(vec3(1.0)-factor)*tex.rgb+vec3(uTextureFactorAlpha);\n"
                " return clamp(result,0.0,1.0);\n"
                "}\n"
                "float alphaArg(float arg, float tex, float current, float diffuse){\n"
                " if(arg<1.5) return uTextureFactorAlpha;\n"
                " if(arg<2.5) return tex;\n"
                " if(arg<3.5) return current;\n"
                " return diffuse;\n"
                "}\n"
                "float alphaStage(vec3 state, float tex, float current, float diffuse){\n"
                " if(state.x<1.5) return current;\n"
                " float a=alphaArg(state.y,tex,current,diffuse);\n"
                " float b=alphaArg(state.z,tex,current,diffuse);\n"
                " if(state.x<2.5) return a;\n"
                " if(state.x<3.5) return b;\n"
                " return a*b;\n"
                "}\n"
                "void main(){\n"
                " vec4 color=vColor;\n"
                " vec4 tex0=vec4(1.0);\n"
                " if(uUseTexture > 0.5) tex0=texture2D(uTexture,vTexCoord).bgra;\n"
                " if(uEffectColorMode > 0.5) color.rgb=effectColor(uTextureFactorColor,tex0,uEffectColorOp);\n"
                " else if(uEffectColorOp > 1.5 && uEffectColorOp < 2.5 && uColorArg1 < 1.5) color.rgb=uTextureFactorColor;\n"
                " else if(uUseTexture > 0.5) {\n"
                "  if(uEffectColorOp > 11.5 && uEffectColorOp < 12.5) color.rgb=mix(uTextureFactorColor,tex0.rgb,vColor.a);\n"
                "  else color.rgb*=tex0.rgb;\n"
                " }\n"
                " color.a=alphaStage(uAlphaStage0,tex0.a,vColor.a,vColor.a);\n"
                " if(uAlphaStage1Enabled > 0.5) {\n"
                "  float tex1=1.0;\n"
                "  if(uUseTexture1 > 0.5) tex1=texture2D(uTexture1,vTexCoord1).a;\n"
                "  color.a=alphaStage(uAlphaStage1,tex1,color.a,vColor.a);\n"
                " }\n"
                " if(uAlphaRef >= 0.0 && color.a < uAlphaRef) discard;\n"
                " if(uFogEnabled > 0.5) {\n"
                "  float fogFactor=1.0;\n"
                "  if(uFogFromDiffuseAlpha > 0.5) fogFactor=vColor.a;\n"
                "  else if(uFogParameters.w < 1.5) fogFactor=exp(-uFogParameters.z*vFogDistance);\n"
                "  else if(uFogParameters.w < 2.5) { float d=uFogParameters.z*vFogDistance; fogFactor=exp(-(d*d)); }\n"
                "  else fogFactor=clamp((uFogParameters.y-vFogDistance)/max(uFogParameters.y-uFogParameters.x,0.0001),0.0,1.0);\n"
                "  color.rgb=mix(uFogColor,color.rgb,clamp(fogFactor,0.0,1.0));\n"
                " }\n"
                " gl_FragColor=color;\n"
                "}\n";
            GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
            GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
            if (!vertex || !fragment) return;
            m_program = glCreateProgram();
            glAttachShader(m_program, vertex);
            glAttachShader(m_program, fragment);
            glBindAttribLocation(m_program, 0, "aPosition");
            glBindAttribLocation(m_program, 1, "aColor");
            glBindAttribLocation(m_program, 2, "aTexCoord");
            glLinkProgram(m_program);
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            GLint linked = GL_FALSE;
            glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
            if (!linked) { glDeleteProgram(m_program); m_program = 0; return; }
            m_positionLocation = 0;
            m_colorLocation = 1;
            m_texCoordLocation = 2;
            m_mvpLocation = glGetUniformLocation(m_program, "uMvp");
            m_worldViewLocation = glGetUniformLocation(m_program, "uWorldView");
            m_viewportLocation = glGetUniformLocation(m_program, "uViewport");
            m_screenSpaceLocation = glGetUniformLocation(m_program, "uScreenSpace");
            m_flipYLocation = glGetUniformLocation(m_program, "uFlipY");
            m_useTextureLocation = glGetUniformLocation(m_program, "uUseTexture");
            m_useTexture1Location = glGetUniformLocation(m_program, "uUseTexture1");
            m_alphaRefLocation = glGetUniformLocation(m_program, "uAlphaRef");
            m_alphaStageLocation[0] = glGetUniformLocation(m_program, "uAlphaStage0");
            m_alphaStageLocation[1] = glGetUniformLocation(m_program, "uAlphaStage1");
            m_textureFactorAlphaLocation = glGetUniformLocation(m_program, "uTextureFactorAlpha");
            m_textureFactorColorLocation = glGetUniformLocation(m_program, "uTextureFactorColor");
            m_colorArg1Location = glGetUniformLocation(m_program, "uColorArg1");
            m_effectColorModeLocation = glGetUniformLocation(m_program, "uEffectColorMode");
            m_effectColorOpLocation = glGetUniformLocation(m_program, "uEffectColorOp");
            m_fogEnabledLocation = glGetUniformLocation(m_program, "uFogEnabled");
            m_fogRangeModeLocation = glGetUniformLocation(m_program, "uFogRangeMode");
            m_fogColorLocation = glGetUniformLocation(m_program, "uFogColor");
            m_fogParametersLocation = glGetUniformLocation(m_program, "uFogParameters");
            m_fogFromDiffuseAlphaLocation = glGetUniformLocation(m_program, "uFogFromDiffuseAlpha");
            m_alphaStage1EnabledLocation = glGetUniformLocation(m_program, "uAlphaStage1Enabled");
            m_textureMatrix0Location = glGetUniformLocation(m_program, "uTextureMatrix0");
            m_textureMatrix1Location = glGetUniformLocation(m_program, "uTextureMatrix1");
            m_generateTexture0Location = glGetUniformLocation(m_program, "uGenerateTexture0");
            m_generateTexture1Location = glGetUniformLocation(m_program, "uGenerateTexture1");
            m_transformTexture0Location = glGetUniformLocation(m_program, "uTransformTexture0");
            m_transformTexture1Location = glGetUniformLocation(m_program, "uTransformTexture1");
            glUseProgram(m_program);
            glUniform1i(glGetUniformLocation(m_program, "uTexture"), 0);
            glUniform1i(glGetUniformLocation(m_program, "uTexture1"), 1);
#endif
        }

        const void* VertexOffset(const void* base, UINT offset) const {
            return base ? (const void*)((const BYTE*)base + offset)
                        : (const void*)(uintptr_t)offset;
        }

        bool PrepareDraw(const void* base, UINT stride, UINT vertexByteOffset = 0) {
#ifdef ANDROID
            if (!m_program || stride == 0) {
                static bool reportedFailure = false;
                if (!reportedFailure) {
                    __android_log_print(ANDROID_LOG_ERROR, "Metin2Mobile", "GLES draw rejected: program=%u stride=%u", m_program, stride);
                    reportedFailure = true;
                }
                return false;
            }
            glUseProgram(m_program);
            // Resource uploads and legacy helpers also bind textures directly.
            // Restore the D3D device's logical bindings before every draw.
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_boundTexture[1]);
            ApplyTextureSampler(1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_boundTexture[0]);
            ApplyTextureSampler(0);
            GLMATRIX worldView, mvp;
            D3DXMatrixMultiply(&worldView, &m_worldMatrix, &m_viewMatrix);
            D3DXMatrixMultiply(&mvp, &worldView, &m_projectionMatrix);
            glUniformMatrix4fv(m_mvpLocation, 1, GL_FALSE, &mvp.m[0][0]);
            glUniformMatrix4fv(m_worldViewLocation, 1, GL_FALSE, &worldView.m[0][0]);
            glUniformMatrix4fv(m_textureMatrix0Location, 1, GL_FALSE, &m_textureMatrix[0].m[0][0]);
            glUniformMatrix4fv(m_textureMatrix1Location, 1, GL_FALSE, &m_textureMatrix[1].m[0][0]);
            glUniform1f(m_generateTexture0Location, m_generateTextureCoords[0] ? 1.0f : 0.0f);
            glUniform1f(m_generateTexture1Location, m_generateTextureCoords[1] ? 1.0f : 0.0f);
            glUniform1f(m_transformTexture0Location, m_transformTextureCoords[0] ? 1.0f : 0.0f);
            glUniform1f(m_transformTexture1Location, m_transformTextureCoords[1] ? 1.0f : 0.0f);
            glUniform1f(m_fogRangeModeLocation, m_rangeFogEnabled ? 1.0f : 0.0f);
            glUniform2f(m_viewportLocation,
                        (float)std::max<DWORD>(1, m_d3dViewport.Width),
                        (float)std::max<DWORD>(1, m_d3dViewport.Height));
            const bool screenSpace = (m_dwFVF & 0x004) != 0;
            // D3D's perspective clip-space Y direction differs from GLES.
            // Orthographic matrices are used by the 2D UI and must remain in
            // their native top-left convention.
            const bool perspective = fabsf(m_projectionMatrix._44) < 0.5f;
            glUniform1f(m_screenSpaceLocation, screenSpace ? 1.0f : 0.0f);
            // GLES and D3D both use +Y toward the top after projection.  The
            // Android viewport conversion already handles the top-left
            // viewport origin; flipping clip-space Y here inverted and
            // displaced the intro character models.
            glUniform1f(m_flipYLocation, 0.0f);
            glUniform1f(m_useTextureLocation, (m_textureEnabled[0] && m_textureStageActive[0]) ? 1.0f : 0.0f);
            glUniform1f(m_useTexture1Location, (m_textureEnabled[1] && m_textureStageActive[1] && m_textureStage1AlphaActive) ? 1.0f : 0.0f);
            glUniform3fv(m_alphaStageLocation[0], 1, m_alphaStage[0]);
            glUniform3fv(m_alphaStageLocation[1], 1, m_alphaStage[1]);
            glUniform1f(m_textureFactorAlphaLocation, m_textureFactorAlpha);
            glUniform3fv(m_textureFactorColorLocation, 1, m_textureFactorColor);
            glUniform1f(m_colorArg1Location, m_colorArg1);
            glUniform1f(m_effectColorModeLocation, m_effectColorMode ? 1.0f : 0.0f);
            glUniform1f(m_effectColorOpLocation, m_effectColorOp);
            glUniform1f(m_alphaStage1EnabledLocation, m_textureStageActive[1] ? 1.0f : 0.0f);
            glUniform1f(m_alphaRefLocation, m_alphaTestEnabled ? m_alphaRef : -1.0f);
            const bool transformedVertexFog = screenSpace && (m_dwFVF & 0x080) != 0;
            const bool applyFog = m_fogEnabled && (transformedVertexFog || (m_fogMode != 0 && !screenSpace && perspective));
            glUniform1f(m_fogEnabledLocation, applyFog ? 1.0f : 0.0f);
            glUniform1f(m_fogFromDiffuseAlphaLocation, transformedVertexFog ? 1.0f : 0.0f);
            glUniform3fv(m_fogColorLocation, 1, m_fogColor);
            glUniform4f(m_fogParametersLocation, m_fogStart, m_fogEnd, m_fogDensity, (float)m_fogMode);
            UINT offset = 0;
            glEnableVertexAttribArray(m_positionLocation);
            glVertexAttribPointer(m_positionLocation, screenSpace ? 4 : 3, GL_FLOAT,
                                  GL_FALSE, (GLsizei)stride, VertexOffset(base, vertexByteOffset + offset));
            offset += screenSpace ? 16 : 12;
            if (m_dwFVF & 0x010) offset += 12;
            if (m_dwFVF & 0x040) {
                glEnableVertexAttribArray(m_colorLocation);
                glVertexAttribPointer(m_colorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                                      (GLsizei)stride, VertexOffset(base, vertexByteOffset + offset));
                offset += 4;
            } else {
                glDisableVertexAttribArray(m_colorLocation);
                glVertexAttrib4f(m_colorLocation, 1, 1, 1, 1);
            }
            if (m_dwFVF & 0x080) offset += 4;
            const int textureCount = (m_dwFVF & 0xf00) >> 8;
            if (textureCount > 0 || (m_dwFVF & 0x100)) {
                glEnableVertexAttribArray(m_texCoordLocation);
                glVertexAttribPointer(m_texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                      (GLsizei)stride, VertexOffset(base, vertexByteOffset + offset));
            } else {
                glDisableVertexAttribArray(m_texCoordLocation);
                glVertexAttrib2f(m_texCoordLocation, 0, 0);
            }
            return true;
#else
            return true;
#endif
        }

        IDirect3DDevice8() {
            m_refCount = 1;
            m_dwFVF = 0;
            m_pStreamSource = NULL;
            m_StreamStride = 0;
            m_pIndexBuffer = NULL;
			m_BaseVertexIndex = 0;
            m_program = 0;
            m_textureEnabled[0] = m_textureEnabled[1] = false;
            m_generateTextureCoords[0] = m_generateTextureCoords[1] = false;
            m_transformTextureCoords[0] = m_transformTextureCoords[1] = false;
            m_textureStageActive[0] = true;
            m_textureStageActive[1] = false;
            m_textureStage1AlphaActive = false;
            m_alphaStage[0][0] = 2.0f; // SELECTARG1
            m_alphaStage[1][0] = 1.0f; // DISABLE
            m_alphaStage[0][1] = m_alphaStage[1][1] = 2.0f; // TEXTURE
            m_alphaStage[0][2] = m_alphaStage[1][2] = 3.0f; // CURRENT
            m_textureFactorAlpha = 1.0f;
            m_textureFactorColor[0] = m_textureFactorColor[1] = m_textureFactorColor[2] = 1.0f;
            m_colorArg1 = 2.0f; // D3D stage-0 default: D3DTA_TEXTURE
            m_effectColorMode = false;
            m_effectColorOp = 4.0f; // D3DTOP_MODULATE
            m_boundTexture[0] = m_boundTexture[1] = 0;
            m_boundTextureHasMipmaps[0] = m_boundTextureHasMipmaps[1] = false;
            m_wrapS[0] = m_wrapS[1] = GL_REPEAT;
            m_wrapT[0] = m_wrapT[1] = GL_REPEAT;
            m_minFilter[0] = m_minFilter[1] = GL_LINEAR;
            m_magFilter[0] = m_magFilter[1] = GL_LINEAR;
            m_mipFilter[0] = m_mipFilter[1] = 2;
            m_alphaTestEnabled = false;
            m_alphaRef = 0.0f;
            m_fogEnabled = false;
            m_rangeFogEnabled = false;
            m_fogMode = 0;
            m_fogStart = 0.0f;
            m_fogEnd = 1.0f;
            m_fogDensity = 1.0f;
            m_fogColor[0] = m_fogColor[1] = m_fogColor[2] = 0.0f;
            m_srcBlendFactor = GL_ONE;
            m_dstBlendFactor = GL_ZERO;
            m_d3dViewport.X = 0;
            m_d3dViewport.Y = 0;
#ifdef ANDROID
            m_d3dViewport.Width = (DWORD)std::max(1, AndroidGetVirtualWidth());
            m_d3dViewport.Height = (DWORD)std::max(1, AndroidGetVirtualHeight());
#else
            m_d3dViewport.Width = 1;
            m_d3dViewport.Height = 1;
#endif
            m_d3dViewport.MinZ = 0.0f;
            m_d3dViewport.MaxZ = 1.0f;
            D3DXMatrixIdentity(&m_worldMatrix);
            D3DXMatrixIdentity(&m_viewMatrix);
            D3DXMatrixIdentity(&m_projectionMatrix);
            D3DXMatrixIdentity(&m_textureMatrix[0]);
            D3DXMatrixIdentity(&m_textureMatrix[1]);
#ifndef ANDROID
            m_hDC = NULL;
#endif
            InitializeGLESProgram();
        }
        ULONG AddRef() { return ++m_refCount; }
        HRESULT CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8* ppTexture) { 
            if (!ppTexture) return E_INVALIDARG;
#ifdef ANDROID
            *ppTexture = Lupus2CreateGLESTexture(Width, Height);
            return *ppTexture ? S_OK : E_OUTOFMEMORY;
#else
            *ppTexture = NULL;
            return E_NOTIMPL;
#endif
        }
        HRESULT SetLight(DWORD,const void*) { return S_OK; }
        HRESULT LightEnable(DWORD,BOOL) { return S_OK; }
        DWORD m_dwFVF;
        HRESULT GetVertexShader(DWORD* pShader) { *pShader = m_dwFVF; return S_OK; }
        HRESULT SetVertexShader(DWORD Shader) { m_dwFVF = Shader; return S_OK; }
        HRESULT CreateVertexShader(const DWORD* pDeclaration, const DWORD* pFunction, DWORD* pHandle, DWORD Usage) { *pHandle = 1; return S_OK; }
        HRESULT DeleteVertexShader(DWORD Handle) { return S_OK; }
        HRESULT CreatePixelShader(const DWORD* pFunction, DWORD* pHandle) { *pHandle = 2; return S_OK; }
        HRESULT DeletePixelShader(DWORD Handle) { return S_OK; }
        HRESULT SetPixelShader(DWORD Handle) { return S_OK; }
        HRESULT SetVertexShaderConstant(DWORD Register, const void* pData, DWORD Count) { return S_OK; }
        HRESULT SetPixelShaderConstant(DWORD Register, const void* pData, DWORD Count) { return S_OK; }
        HRESULT Clear(DWORD n, const void* pRects, DWORD flags, D3DCOLOR color, float z, DWORD stencil) { 
            GLbitfield mask = 0;
#ifdef ANDROID
            // D3D Clear with no rectangles clears the complete render target.
            // GLES glClear is clipped by the current scissor and write masks;
            // UI clipping from the previous frame otherwise leaves rotating EGL
            // back buffers partially uncleared (old screen fragments + flicker).
            const GLboolean scissorWasEnabled = glIsEnabled(GL_SCISSOR_TEST);
            GLboolean oldColorMask[4] = { GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE };
            GLboolean oldDepthMask = GL_TRUE;
            glGetBooleanv(GL_COLOR_WRITEMASK, oldColorMask);
            glGetBooleanv(GL_DEPTH_WRITEMASK, &oldDepthMask);
            if (scissorWasEnabled)
                glDisable(GL_SCISSOR_TEST);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDepthMask(GL_TRUE);
#endif
            if (flags & 1) { // D3DCLEAR_TARGET
                glClearColor(((color >> 16) & 0xff) / 255.0f,
                             ((color >> 8) & 0xff) / 255.0f,
                             (color & 0xff) / 255.0f,
                             ((color >> 24) & 0xff) / 255.0f);
                mask |= GL_COLOR_BUFFER_BIT;
            }
            if (flags & 2) { // D3DCLEAR_ZBUFFER
                #ifndef ANDROID
                    glClearDepth(z);
                #else
                    glClearDepthf(z);
                #endif
                mask |= GL_DEPTH_BUFFER_BIT;
            }
            if (mask) glClear(mask);
#ifdef ANDROID
            glColorMask(oldColorMask[0], oldColorMask[1], oldColorMask[2], oldColorMask[3]);
            glDepthMask(oldDepthMask);
            if (scissorWasEnabled)
                glEnable(GL_SCISSOR_TEST);
#endif
            return S_OK; 
        }
        HRESULT TestCooperativeLevel() { return S_OK; }
        HRESULT Reset(D3DPRESENT_PARAMETERS*) { return S_OK; }
        HRESULT Present(const RECT*,const RECT*,HWND,const void*) { 
#ifndef ANDROID
            if (m_hDC) SwapBuffers(m_hDC);
#endif
            return S_OK; 
        }
        HRESULT CreateDepthStencilSurface(UINT,UINT,D3DFORMAT,D3DMULTISAMPLE_TYPE,LPDIRECT3DSURFACE8*) { return S_OK; }
        HRESULT GetRenderTarget(LPDIRECT3DSURFACE8*) { return S_OK; }
        HRESULT GetDepthStencilSurface(LPDIRECT3DSURFACE8*) { return S_OK; }
        HRESULT GetViewport(D3DVIEWPORT8* viewport) {
            if (!viewport) return E_INVALIDARG;
            *viewport = m_d3dViewport;
            return S_OK;
        }
        HRESULT SetRenderTarget(LPDIRECT3DSURFACE8,LPDIRECT3DSURFACE8) { return S_OK; }
        HRESULT SetViewport(const D3DVIEWPORT8* v) { 
            if (!v) return E_INVALIDARG;
            m_d3dViewport = *v;
#ifdef ANDROID
            const int vw = std::max(1, AndroidGetVirtualWidth());
            const int vh = std::max(1, AndroidGetVirtualHeight());
            const int sw = std::max(1, AndroidGetSurfaceWidth());
            const int sh = std::max(1, AndroidGetSurfaceHeight());
            const int px = int((int64_t)v->X * sw / vw);
            const int pw = std::max(1, int((int64_t)v->Width * sw / vw));
            const int ph = std::max(1, int((int64_t)v->Height * sh / vh));
            const int pyTop = int((int64_t)v->Y * sh / vh);
            glViewport(px, std::max(0, sh - pyTop - ph), pw, ph);
#else
            glViewport(v->X, v->Y, v->Width, v->Height);
#endif
            return S_OK; 
        }
        ULONG Release() { ULONG result = --m_refCount; if (!result) { if (m_program) glDeleteProgram(m_program); delete this; } return result; }
        HRESULT BeginScene() { return S_OK; }
        HRESULT EndScene() { return S_OK; }
        HRESULT GetDeviceCaps(D3DCAPS8* caps) {
            if (!caps) return E_INVALIDARG;
            memset(caps, 0, sizeof(*caps));
            GLint maxTextureSize = 0;
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
            caps->MaxTextureWidth = (DWORD)maxTextureSize;
            caps->MaxTextureHeight = (DWORD)maxTextureSize;
            caps->DevCaps = D3DDEVCAPS_HWTRANSFORMANDLIGHT;
            caps->Caps2 = D3DCAPS2_CANRENDERWINDOWED;
            caps->VertexProcessingCaps = D3DVTXPCAPS_DIRECTIONALLIGHTS |
                                         D3DVTXPCAPS_POSITIONALLIGHTS |
                                         D3DVTXPCAPS_TEXGEN;
            caps->PrimitiveMiscCaps = D3DPMISCCAPS_CLIPTLVERTS;
            caps->TextureAddressCaps = D3DPTADDRESSCAPS_BORDER;
            return S_OK;
        }
        void SetGammaRamp(DWORD,DWORD,const void*) {}
        HRESULT GetBackBuffer(UINT,D3DBACKBUFFER_TYPE,LPDIRECT3DSURFACE8*) { return S_OK; }
        UINT GetAvailableTextureMem() { return 128 * 1024 * 1024; }
        LPDIRECT3DVERTEXBUFFER8 m_pStreamSource;
        UINT m_StreamStride;
        HRESULT SetStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER8 pStreamData, UINT Stride) { 
            m_pStreamSource = pStreamData;
            m_StreamStride = Stride;
            if (pStreamData) glBindBuffer(GL_ARRAY_BUFFER, Lupus2GLESVertexBufferHandle(pStreamData));
            else glBindBuffer(GL_ARRAY_BUFFER, 0);
            return S_OK; 
        }
        HRESULT CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, LPDIRECT3DVERTEXBUFFER8* ppVertexBuffer) { 
            if (!ppVertexBuffer) return E_INVALIDARG;
#ifdef ANDROID
            *ppVertexBuffer = Lupus2CreateGLESVertexBuffer(Length, (Usage & 2u) != 0u);
            return *ppVertexBuffer ? S_OK : E_OUTOFMEMORY;
#else
            *ppVertexBuffer = NULL;
            return E_NOTIMPL;
#endif
        }
        HRESULT CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DINDEXBUFFER8* ppIndexBuffer) { 
            if (!ppIndexBuffer) return E_INVALIDARG;
#ifdef ANDROID
            *ppIndexBuffer = Lupus2CreateGLESIndexBuffer(Length);
            return *ppIndexBuffer ? S_OK : E_OUTOFMEMORY;
#else
            *ppIndexBuffer = NULL;
            return E_NOTIMPL;
#endif
        }
        LPDIRECT3DINDEXBUFFER8 m_pIndexBuffer;
        UINT m_BaseVertexIndex;
        HRESULT SetIndices(LPDIRECT3DINDEXBUFFER8 pIndexBuffer, UINT BaseVertexIndex) {
            m_pIndexBuffer = pIndexBuffer;
			m_BaseVertexIndex = BaseVertexIndex;
            if (pIndexBuffer) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Lupus2GLESIndexBufferHandle(pIndexBuffer));
            else glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            return S_OK;
        }
        HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
            GLenum mode = GL_TRIANGLES;
            GLsizei count = 0;
            switch(PrimitiveType) {
                case 1: mode = GL_POINTS; count = (GLsizei)primCount; break;
                case 2: mode = GL_LINES; count = (GLsizei)primCount * 2; break;
                case 3: mode = GL_LINE_STRIP; count = (GLsizei)primCount + 1; break;
                case 4: mode = GL_TRIANGLES; count = (GLsizei)primCount * 3; break;
                case 5: mode = GL_TRIANGLE_STRIP; count = (GLsizei)primCount + 2; break;
                case 6: mode = GL_TRIANGLE_FAN; count = (GLsizei)primCount + 2; break;
            }
            
#ifdef ANDROID
            if (!m_pStreamSource || !m_pIndexBuffer)
                return E_FAIL;
            glBindBuffer(GL_ARRAY_BUFFER, Lupus2GLESVertexBufferHandle(m_pStreamSource));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Lupus2GLESIndexBufferHandle(m_pIndexBuffer));
            if (!PrepareDraw(NULL, m_StreamStride, m_BaseVertexIndex * m_StreamStride)) return E_FAIL;
            glDrawElements(mode, count, GL_UNSIGNED_SHORT, (void*)(uintptr_t)(startIndex * 2));
#else
            glEnableClientState(GL_VERTEX_ARRAY);
            
            int offset = 0;
            // Position (XYZ)
            glVertexPointer(3, GL_FLOAT, (GLsizei)m_StreamStride, (void*)(size_t)offset);
            offset += 12; 

            if (m_dwFVF & D3DFVF_XYZRHW) {
                // RHW not really supported in pure OpenGL fixed pipeline easily without shaders
                // but we shift if needed. Metin2 uses XYZ usually.
            }

            if (m_dwFVF & D3DFVF_NORMAL) {
                glEnableClientState(GL_NORMAL_ARRAY);
                glNormalPointer(GL_FLOAT, (GLsizei)m_StreamStride, (void*)(size_t)offset);
                offset += 12;
            } else {
                glDisableClientState(GL_NORMAL_ARRAY);
            }

            if (m_dwFVF & D3DFVF_DIFFUSE) {
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(4, GL_UNSIGNED_BYTE, (GLsizei)m_StreamStride, (void*)(size_t)offset);
                offset += 4;
            } else {
                glDisableClientState(GL_COLOR_ARRAY);
            }

            if (m_dwFVF & D3DFVF_SPECULAR) {
                offset += 4;
            }

            // Texture Coordinates
            int numTex = (m_dwFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
            if (numTex > 0) {
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(2, GL_FLOAT, (GLsizei)m_StreamStride, (void*)(size_t)offset);
                offset += 8 * numTex;
            } else if (m_dwFVF & D3DFVF_TEX1) {
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(2, GL_FLOAT, (GLsizei)m_StreamStride, (void*)(size_t)offset);
                offset += 8;
            } else {
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
            
            glDrawElements(mode, count, GL_UNSIGNED_SHORT, (void*)(size_t)(startIndex * 2));
            
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
            return S_OK;
        }
        HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
#ifdef ANDROID
            GLenum mode = GL_TRIANGLES;
            GLsizei count = 0;
            switch(PrimitiveType) {
                case 1: mode = GL_POINTS; count = PrimitiveCount; break;
                case 2: mode = GL_LINES; count = PrimitiveCount * 2; break;
                case 3: mode = GL_LINE_STRIP; count = PrimitiveCount + 1; break;
                case 4: mode = GL_TRIANGLES; count = PrimitiveCount * 3; break;
                case 5: mode = GL_TRIANGLE_STRIP; count = PrimitiveCount + 2; break;
                case 6: mode = GL_TRIANGLE_FAN; count = PrimitiveCount + 2; break;
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            if (!PrepareDraw(pVertexStreamZeroData, VertexStreamZeroStride)) return E_FAIL;
            glDrawElements(mode, count,
                           IndexDataFormat == 17 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                           pIndexData);
#else
            glEnableClientState(GL_VERTEX_ARRAY);
            if (VertexStreamZeroStride >= 20) {
                glEnableClientState(GL_COLOR_ARRAY);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glVertexPointer(3, GL_FLOAT, VertexStreamZeroStride, pVertexStreamZeroData);
                glColorPointer(4, GL_UNSIGNED_BYTE, VertexStreamZeroStride, (char*)pVertexStreamZeroData + 12);
                glTexCoordPointer(2, GL_FLOAT, VertexStreamZeroStride, (char*)pVertexStreamZeroData + 16);
            } else {
                glVertexPointer(3, GL_FLOAT, VertexStreamZeroStride, pVertexStreamZeroData);
            }

            GLenum mode = GL_TRIANGLES;
            GLsizei count = 0;
            switch(PrimitiveType) {
                case 4: mode = GL_TRIANGLES; count = PrimitiveCount * 3; break;
                case 5: mode = GL_TRIANGLE_STRIP; count = PrimitiveCount + 2; break;
                case 6: mode = GL_TRIANGLE_FAN; count = PrimitiveCount + 2; break;
            }

            glDrawElements(mode, count, (IndexDataFormat == 17 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT), pIndexData);

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
            return S_OK;
        }
        HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) { 
            GLenum mode = GL_TRIANGLES;
            GLsizei count = 0;
            switch(PrimitiveType) {
                case 1: mode = GL_POINTS; count = PrimitiveCount; break;
                case 2: mode = GL_LINES; count = PrimitiveCount * 2; break;
                case 3: mode = GL_LINE_STRIP; count = PrimitiveCount + 1; break;
                case 4: mode = GL_TRIANGLES; count = PrimitiveCount * 3; break;
                case 5: mode = GL_TRIANGLE_STRIP; count = PrimitiveCount + 2; break;
                case 6: mode = GL_TRIANGLE_FAN; count = PrimitiveCount + 2; break;
            }
#ifdef ANDROID
            if (!m_pStreamSource) return E_FAIL;
            glBindBuffer(GL_ARRAY_BUFFER, Lupus2GLESVertexBufferHandle(m_pStreamSource));
            if (!PrepareDraw(NULL, m_StreamStride)) return E_FAIL;
#endif
            glDrawArrays(mode, StartVertex, count);
            return S_OK; 
        }
        HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
#ifdef ANDROID
            GLenum mode = GL_TRIANGLES;
            GLsizei count = 0;
            switch(PrimitiveType) {
                case 1: mode = GL_POINTS; count = PrimitiveCount; break;
                case 2: mode = GL_LINES; count = PrimitiveCount * 2; break;
                case 3: mode = GL_LINE_STRIP; count = PrimitiveCount + 1; break;
                case 4: mode = GL_TRIANGLES; count = PrimitiveCount * 3; break;
                case 5: mode = GL_TRIANGLE_STRIP; count = PrimitiveCount + 2; break;
                case 6: mode = GL_TRIANGLE_FAN; count = PrimitiveCount + 2; break;
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            if (!PrepareDraw(pVertexStreamZeroData, VertexStreamZeroStride)) return E_FAIL;
            glDrawArrays(mode, 0, count);
#else
            glEnableClientState(GL_VERTEX_ARRAY);
            if (VertexStreamZeroStride >= 20) { // XYZ + COLOR + TEX
                glEnableClientState(GL_COLOR_ARRAY);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glVertexPointer(3, GL_FLOAT, VertexStreamZeroStride, pVertexStreamZeroData);
                glColorPointer(4, GL_UNSIGNED_BYTE, VertexStreamZeroStride, (char*)pVertexStreamZeroData + 12);
                glTexCoordPointer(2, GL_FLOAT, VertexStreamZeroStride, (char*)pVertexStreamZeroData + 16);
            } else {
                glVertexPointer(3, GL_FLOAT, VertexStreamZeroStride, pVertexStreamZeroData);
            }
            DrawPrimitive(PrimitiveType, 0, PrimitiveCount);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
            return S_OK;
        }
        HRESULT SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value) {
#ifdef ANDROID
            if (stage < 2) {
                glActiveTexture(GL_TEXTURE0 + stage);
                switch (type) {
                    case 3: // D3DTSS_COLOROP (legacy mobile enum)
                        m_textureStageActive[stage] = value != 1; // D3DTOP_DISABLE
                        if (stage == 0) m_effectColorOp = (float)value;
                        break;
                    case 1: // D3DTSS_COLORARG1
                        if (stage == 0) m_colorArg1 = (float)(value & 0x0f);
                        break;
                    case 6: // D3DTSS_ALPHAOP (legacy mobile enum)
                        if (stage == 1) m_textureStage1AlphaActive = value != 1; // D3DTOP_DISABLE
                        m_alphaStage[stage][0] = (float)value;
                        break;
                    case 4: // D3DTSS_ALPHAARG1
                        m_alphaStage[stage][1] = (float)value;
                        break;
                    case 5: // D3DTSS_ALPHAARG2
                        m_alphaStage[stage][2] = (float)value;
                        break;
                    case 11: // D3DTSS_TEXCOORDINDEX
                        m_generateTextureCoords[stage] = (value & 0xffff0000u) != 0;
                        break;
                    case 13: // D3DTSS_ADDRESSU
                        m_wrapS[stage] = value == 1 ? GL_REPEAT : GL_CLAMP_TO_EDGE;
                        break;
                    case 14: // D3DTSS_ADDRESSV
                        m_wrapT[stage] = value == 1 ? GL_REPEAT : GL_CLAMP_TO_EDGE;
                        break;
                    case 16: // D3DTSS_MAGFILTER
                        m_magFilter[stage] = value == 1 ? GL_NEAREST : GL_LINEAR;
                        break;
                    case 17: // D3DTSS_MINFILTER
                        m_minFilter[stage] = value == 1 ? GL_NEAREST : GL_LINEAR;
                        break;
                    case 18: // D3DTSS_MIPFILTER
                        m_mipFilter[stage] = value;
                        break;
                    case 24: // D3DTSS_TEXTURETRANSFORMFLAGS
                        // These flags must not erase the independent TEXCOORDINDEX:
                        // the D3D state cache may not resend an unchanged index.
                        m_transformTextureCoords[stage] = (value & 0xffu) != 0;
                        break;
                }
                glActiveTexture(GL_TEXTURE0);
            }
#else
            glActiveTexture(GL_TEXTURE0 + stage);
            switch(type) {
                case 13: // D3DTSS_ADDRESSU
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (value == 1 ? GL_CLAMP : GL_REPEAT));
                    break;
                case 14: // D3DTSS_ADDRESSV
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (value == 1 ? GL_CLAMP : GL_REPEAT));
                    break;
                case 16: // D3DTSS_MAGFILTER
                case 17: // D3DTSS_MINFILTER
                    glTexParameteri(GL_TEXTURE_2D, (type == 17 ? GL_TEXTURE_MIN_FILTER : GL_TEXTURE_MAG_FILTER), (value == 2 ? GL_LINEAR : GL_NEAREST));
                    break;
                case 3: // D3DTSS_COLOROP (simplified)
                    if (value == 2) glDisable(GL_TEXTURE_2D); // D3DTOP_DISABLE
                    else glEnable(GL_TEXTURE_2D);
                    break;
            }
#endif
            return S_OK;
        }
        HRESULT SetRenderState(DWORD Type, DWORD Value) {
            switch(Type) {
                case D3DRS_ZENABLE:
                    if (Value) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
                    break;
                case D3DRS_ZFUNC:
                    glDepthFunc(GL_LEQUAL); // simplified mapping
                    break;
                case D3DRS_ALPHABLENDENABLE:
                    if (Value) glEnable(GL_BLEND); else glDisable(GL_BLEND);
                    break;
                case D3DRS_ZWRITEENABLE:
                    glDepthMask(Value ? GL_TRUE : GL_FALSE);
                    break;
                case D3DRS_ALPHATESTENABLE:
#ifdef ANDROID
                    m_alphaTestEnabled = Value != 0;
#else
                    if (Value) glEnable(GL_ALPHA_TEST); else glDisable(GL_ALPHA_TEST);
#endif
                    break;
                case 24: // D3DRS_ALPHAREF
#ifdef ANDROID
                    m_alphaRef = (float)(Value & 0xff) / 255.0f;
#endif
                    break;
                case 60: // D3DRS_TEXTUREFACTOR, including actor spawn/revive opacity
#ifdef ANDROID
                    m_textureFactorAlpha = (float)((Value >> 24) & 0xff) / 255.0f;
                    m_textureFactorColor[0] = (float)((Value >> 16) & 0xff) / 255.0f;
                    m_textureFactorColor[1] = (float)((Value >> 8) & 0xff) / 255.0f;
                    m_textureFactorColor[2] = (float)(Value & 0xff) / 255.0f;
#endif
                    break;
                case D3DRS_ALPHAFUNC:
                    #ifndef ANDROID
                    glAlphaFunc(GL_GREATER, 0.5f); // simplified mapping
                    #endif
                    break;
                case D3DRS_SRCBLEND:
                case D3DRS_DESTBLEND:
                    {
                        GLenum factor = GL_ONE;
                        switch(Value) {
                            case D3DBLEND_ZERO: factor = GL_ZERO; break;
                            case D3DBLEND_ONE: factor = GL_ONE; break;
                            case D3DBLEND_SRCCOLOR: factor = GL_SRC_COLOR; break;
                            case D3DBLEND_INVSRCCOLOR: factor = GL_ONE_MINUS_SRC_COLOR; break;
                            case D3DBLEND_SRCALPHA: factor = GL_SRC_ALPHA; break;
                            case D3DBLEND_INVSRCALPHA: factor = GL_ONE_MINUS_SRC_ALPHA; break;
                            case D3DBLEND_DESTALPHA: factor = GL_DST_ALPHA; break;
                            case D3DBLEND_INVDESTALPHA: factor = GL_ONE_MINUS_DST_ALPHA; break;
                            case D3DBLEND_DESTCOLOR: factor = GL_DST_COLOR; break;
                            case D3DBLEND_INVDESTCOLOR: factor = GL_ONE_MINUS_DST_COLOR; break;
                            case D3DBLEND_SRCALPHASAT: factor = GL_SRC_ALPHA_SATURATE; break;
                        }
                        if (Type == D3DRS_SRCBLEND)
                            m_srcBlendFactor = factor;
                        else
                            m_dstBlendFactor = factor;
                        glBlendFunc(m_srcBlendFactor, m_dstBlendFactor);
                    }
                    break;
                case D3DRS_CULLMODE:
#ifdef ANDROID
					// D3D and GLES use opposite viewport handedness.  Keep legacy
					// character/terrain geometry visible until every old mesh winding
					// variant has been normalized by the mobile renderer.
					glDisable(GL_CULL_FACE);
#else
                    if (Value == D3DCULL_NONE) glDisable(GL_CULL_FACE);
                    else {
                        glEnable(GL_CULL_FACE);
                        // D3D names the winding that must be rejected.  OpenGL
                        // names front/back faces instead; with its default CCW
                        // front face, rejecting CW means culling GL_BACK.
                        glCullFace(Value == D3DCULL_CW ? GL_BACK : GL_FRONT);
                    }
#endif
                    break;
                case D3DRS_FOGENABLE:
                    m_fogEnabled = Value != 0;
                    break;
                case D3DRS_FOGCOLOR:
                    m_fogColor[0] = (float)((Value >> 16) & 0xff) / 255.0f;
                    m_fogColor[1] = (float)((Value >> 8) & 0xff) / 255.0f;
                    m_fogColor[2] = (float)(Value & 0xff) / 255.0f;
                    break;
                case D3DRS_FOGTABLEMODE:
                    // Metin2's PC renderer uses vertex fog. Keep the table mode
                    // only as a fallback for environments that explicitly set it.
                    if (m_fogMode == 0)
                        m_fogMode = Value;
                    break;
                case D3DRS_FOGVERTEXMODE:
                    m_fogMode = Value;
                    break;
                case D3DRS_FOGSTART:
                    {
                        union { DWORD bits; float value; } fogValue;
                        fogValue.bits = Value;
                        m_fogStart = fogValue.value;
                    }
                    break;
                case D3DRS_FOGEND:
                    {
                        union { DWORD bits; float value; } fogValue;
                        fogValue.bits = Value;
                        m_fogEnd = fogValue.value;
                    }
                    break;
                case D3DRS_FOGDENSITY:
                    {
                        union { DWORD bits; float value; } fogValue;
                        fogValue.bits = Value;
                        m_fogDensity = fogValue.value;
                    }
                    break;
                case D3DRS_RANGEFOGENABLE:
                    m_rangeFogEnabled = Value != 0;
                    break;
            }
            return S_OK;
        }
        void SetMobileEffectColorMode(bool enabled) { m_effectColorMode = enabled; }
        HRESULT SetTexture(DWORD stage, LPDIRECT3DBaseTexture8 tex) { 
            if (stage > 1)
                return S_OK;
            glActiveTexture(GL_TEXTURE0 + stage);
            if (tex) {
                IDirect3DTexture8* gtex = (IDirect3DTexture8*)tex;
                m_boundTexture[stage] = Lupus2GLESTextureHandle(gtex);
                m_boundTextureHasMipmaps[stage] = Lupus2GLESTextureHasMipmaps(gtex);
                glBindTexture(GL_TEXTURE_2D, m_boundTexture[stage]);
#ifdef ANDROID
                m_textureEnabled[stage] = true;
#else
                glEnable(GL_TEXTURE_2D);
#endif
            } else {
                m_boundTexture[stage] = 0;
                m_boundTextureHasMipmaps[stage] = false;
                glBindTexture(GL_TEXTURE_2D, 0);
#ifdef ANDROID
                m_textureEnabled[stage] = false;
#else
                glDisable(GL_TEXTURE_2D);
#endif
            }
            glActiveTexture(GL_TEXTURE0);
            return S_OK; 
        }
        HRESULT SetMaterial(const D3DMATERIAL8*) { return S_OK; }
        HRESULT SetTransform(DWORD type, const GLMATRIX* mat) { 
#ifdef ANDROID
            if (!mat) return E_INVALIDARG;
            if (type == 1) m_worldMatrix = *mat;
            else if (type == 2) m_viewMatrix = *mat;
            else if (type == 3) m_projectionMatrix = *mat;
            else if (type == 16) m_textureMatrix[0] = *mat;
            else if (type == 17) m_textureMatrix[1] = *mat;
#else
            if (type == 3) glMatrixMode(GL_PROJECTION);
            else glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(&mat->m[0][0]);
#endif
            return S_OK; 
        }
        HRESULT GetTransform(DWORD type, GLMATRIX* mat) {
            if (!mat) return E_INVALIDARG;
#ifdef ANDROID
            if (type == 1) *mat = m_worldMatrix;
            else if (type == 2) *mat = m_viewMatrix;
            else if (type == 3) *mat = m_projectionMatrix;
            else if (type == 16) *mat = m_textureMatrix[0];
            else if (type == 17) *mat = m_textureMatrix[1];
            else D3DXMatrixIdentity(mat);
#endif
            return S_OK;
        }
    };
    struct IDirect3DTexture8 {
        GLuint glId;
        int width, height;
        bool hasMipmaps;
        void* pLockedData;
        ULONG refCount;
        IDirect3DTexture8() : glId(0), width(0), height(0), hasMipmaps(false), pLockedData(NULL), refCount(1) {}
        ULONG AddRef() { return ++refCount; }
        UINT GetLevelCount() const { return 1; }
        HRESULT LockRect(UINT Level,D3DLOCKED_RECT* pLockedRect,const RECT* pRect,DWORD Flags) { 
            if (!pLockedData) pLockedData = malloc(width * height * 4);
            pLockedRect->pBits = pLockedData;
            pLockedRect->Pitch = width * 4;
            return S_OK; 
        }
        HRESULT UnlockRect(UINT Level) { 
            if (glId && pLockedData) {
                glBindTexture(GL_TEXTURE_2D, glId);
                glTexSubImage2D(GL_TEXTURE_2D, Level, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pLockedData);
            }
            return S_OK; 
        }
        ULONG Release() { 
            if (--refCount != 0) return refCount;
            if (glId) glDeleteTextures(1, &glId);
            if (pLockedData) free(pLockedData);
            delete this;
            return 0; 
        }
        HRESULT GetSurfaceLevel(UINT,LPDIRECT3DSURFACE8*) { return S_OK; }
    };
    struct IDirect3DSurface8 {
        HRESULT GetDesc(D3DSURFACE_DESC* desc) { if (desc) { desc->Width = 0; desc->Height = 0; desc->Format = 0; } return S_OK; }
        ULONG Release() { return 0; }
    };
    struct IDirect3DBaseTexture8 {
        ULONG Release() { return 0; }
    };
    struct IDirect3DVertexBuffer8 {
        GLuint glVbo;
        UINT length;
        void* pData;
        bool dynamic;
        IDirect3DVertexBuffer8(UINT len, bool isDynamic) : glVbo(0), length(len), pData(NULL), dynamic(isDynamic) {
            glGenBuffers(1, &glVbo);
            pData = malloc(len);
        }
        HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags) { 
            *ppbData = (BYTE*)pData + OffsetToLock;
            return S_OK; 
        }
        HRESULT Unlock() { 
            glBindBuffer(GL_ARRAY_BUFFER, glVbo);
            glBufferData(GL_ARRAY_BUFFER, length, pData, dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return S_OK; 
        }
        ULONG Release() { 
            if (glVbo) glDeleteBuffers(1, &glVbo);
            if (pData) free(pData);
            delete this;
            return 0; 
        }
    };
    struct IDirect3DIndexBuffer8 {
        GLuint glIbo;
        UINT length;
        void* pData;
        IDirect3DIndexBuffer8(UINT len) : glIbo(0), length(len), pData(NULL) {
            glGenBuffers(1, &glIbo);
            pData = malloc(len);
        }
        HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags) { 
            *ppbData = (BYTE*)pData + OffsetToLock;
            return S_OK; 
        }
        HRESULT Unlock() { 
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIbo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, pData, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            return S_OK; 
        }
        ULONG Release() { 
            if (glIbo) glDeleteBuffers(1, &glIbo);
            if (pData) free(pData);
            delete this;
            return 0; 
        }
    };
#ifdef ANDROID
    inline IDirect3DDevice8* Lupus2CreateGLESDevice() {
        return new IDirect3DDevice8();
    }
	inline const void* Lupus2GLESVertexBufferData(IDirect3DVertexBuffer8* buffer) {
		return buffer ? buffer->pData : NULL;
	}
	inline const void* Lupus2GLESIndexBufferData(IDirect3DIndexBuffer8* buffer) {
		return buffer ? buffer->pData : NULL;
	}
	inline unsigned int Lupus2GLESVertexBufferLength(IDirect3DVertexBuffer8* buffer) {
		return buffer ? buffer->length : 0;
	}
	inline unsigned int Lupus2GLESIndexBufferLength(IDirect3DIndexBuffer8* buffer) {
		return buffer ? buffer->length : 0;
	}
    inline IDirect3DTexture8* Lupus2CreateGLESTexture(UINT width, UINT height) {
        IDirect3DTexture8* texture = new IDirect3DTexture8();
        if (!texture) return NULL;
        texture->width = (int)width;
        texture->height = (int)height;
        glGenTextures(1, &texture->glId);
        glBindTexture(GL_TEXTURE_2D, texture->glId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        return texture;
    }
    inline IDirect3DTexture8* Lupus2CreateGLESTextureFromMemory(const void* data, UINT size,
                                                                D3DXIMAGE_INFO* imageInfo) {
        static bool devilInitialized = false;
        if (!devilInitialized) {
            ilInit();
            devilInitialized = true;
        }

        ILuint image = 0;
        ilGenImages(1, &image);
        ilBindImage(image);
        ilEnable(IL_ORIGIN_SET);
        // GLES samples texture row zero from the bottom, while the legacy D3D
        // UI expects UV row zero at the top.  Normalize every decoded raster
        // to a lower-left memory origin before uploading it.
        ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
        const BYTE* encoded = static_cast<const BYTE*>(data);
        const bool isJpeg = size >= 3 && encoded[0] == 0xff && encoded[1] == 0xd8 && encoded[2] == 0xff;
        if (!ilLoadL(IL_TYPE_UNKNOWN, data, size) ||
            !ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE)) {
            ilDeleteImages(1, &image);
            return NULL;
        }

        const UINT width = (UINT)ilGetInteger(IL_IMAGE_WIDTH);
        const UINT height = (UINT)ilGetInteger(IL_IMAGE_HEIGHT);
        IDirect3DTexture8* texture = Lupus2CreateGLESTexture(width, height);
        if (!texture) {
            ilDeleteImages(1, &image);
            return NULL;
        }
        glBindTexture(GL_TEXTURE_2D, texture->glId);
        const ILubyte* uploadPixels = ilGetData();
        std::vector<ILubyte> flippedPixels;
        // The large intro rasters are JPEGs authored for Direct3D's top-left
        // UV convention.  DevIL's Android path leaves their scanlines in the
        // opposite order even with IL_ORIGIN_SET, so flip those rows
        // explicitly.  Small JPEGs (sphere maps) keep their original mapping.
        if (isJpeg && width >= 512 && height >= 512) {
            const size_t rowBytes = static_cast<size_t>(width) * 4u;
            flippedPixels.resize(rowBytes * static_cast<size_t>(height));
            for (UINT row = 0; row < height; ++row)
                memcpy(&flippedPixels[static_cast<size_t>(row) * rowBytes],
                       uploadPixels + static_cast<size_t>(height - 1u - row) * rowBytes,
                       rowBytes);
            uploadPixels = flippedPixels.data();
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, uploadPixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        texture->hasMipmaps = true;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        if (imageInfo) {
            memset(imageInfo, 0, sizeof(*imageInfo));
            imageInfo->Width = width;
            imageInfo->Height = height;
            imageInfo->Depth = 1;
            imageInfo->MipLevels = 1;
            imageInfo->Format = D3DFMT_A8R8G8B8;
        }
        ilDeleteImages(1, &image);
        return texture;
    }
    inline IDirect3DVertexBuffer8* Lupus2CreateGLESVertexBuffer(UINT length, bool dynamic) {
        return new IDirect3DVertexBuffer8(length, dynamic);
    }
    inline IDirect3DIndexBuffer8* Lupus2CreateGLESIndexBuffer(UINT length) {
        return new IDirect3DIndexBuffer8(length);
    }
    inline GLuint Lupus2GLESVertexBufferHandle(IDirect3DVertexBuffer8* buffer) {
        return buffer ? buffer->glVbo : 0;
    }
    inline GLuint Lupus2GLESIndexBufferHandle(IDirect3DIndexBuffer8* buffer) {
        return buffer ? buffer->glIbo : 0;
    }
    inline GLuint Lupus2GLESTextureHandle(IDirect3DTexture8* texture) {
        return texture ? texture->glId : 0;
    }
    inline bool Lupus2GLESTextureHasMipmaps(IDirect3DTexture8* texture) {
        return texture && texture->hasMipmaps;
    }
#endif
    struct ID3DXMatrixStack {
        std::vector<GLMATRIX> m_stack;
        ID3DXMatrixStack() { m_stack.push_back(GLMATRIX()); }
        
        HRESULT Push() { m_stack.push_back(m_stack.back()); return S_OK; }
        HRESULT Pop() { if (m_stack.size() > 1) m_stack.pop_back(); return S_OK; }
        HRESULT LoadIdentity() { D3DXMatrixIdentity(&m_stack.back()); return S_OK; }
        HRESULT LoadMatrix(const GLMATRIX* pMat) { m_stack.back() = *pMat; return S_OK; }
        HRESULT MultMatrix(const GLMATRIX* pMat) { D3DXMatrixMultiply(&m_stack.back(), &m_stack.back(), pMat); return S_OK; }
        HRESULT MultMatrixLocal(const GLMATRIX* pMat) { D3DXMatrixMultiply(&m_stack.back(), pMat, &m_stack.back()); return S_OK; }
        HRESULT Scale(float x, float y, float z) { 
            GLMATRIX s; D3DXMatrixScaling(&s, x, y, z);
            MultMatrix(&s); return S_OK; 
        }
        HRESULT Translate(float x, float y, float z) {
            GLMATRIX t; D3DXMatrixTranslation(&t, x, y, z);
            MultMatrix(&t); return S_OK;
        }
        HRESULT RotateAxis(const GLVECTOR3* pV, float angle) {
            // Rotation around axis implementation...
            return S_OK;
        }
        HRESULT RotateAxisLocal(const GLVECTOR3* pV, float angle) {
            return S_OK;
        }
        HRESULT RotateYawPitchRollLocal(float y, float p, float r) {
            GLMATRIX m; D3DXMatrixRotationYawPitchRoll(&m, y, p, r);
            MultMatrixLocal(&m); return S_OK;
        }
        GLMATRIX* GetTop() { return &m_stack.back(); }
        ULONG Release() { delete this; return 0; }
    };
    #ifndef LUPUS2_D3D_CORE_TYPES
    struct D3DVIEWPORT8 {
        DWORD X, Y, Width, Height;
        float MinZ, MaxZ;
    };
    struct D3DCAPS8 {
        DWORD TextureCaps;
        DWORD MaxTextureWidth, MaxTextureHeight;
    };
    struct D3DPRESENT_PARAMETERS {
        UINT BackBufferWidth, BackBufferHeight;
        int Windowed;
    };
    #endif

    struct ID3DXBuffer {
        void* GetBufferPointer() { return 0; }
        DWORD GetBufferSize() { return 0; }
        ULONG Release() { delete this; return 0; }
    };
    struct ID3DXMesh {
        HRESULT GetIndexBuffer(LPDIRECT3DINDEXBUFFER8*) { return S_OK; }
        HRESULT GetVertexBuffer(LPDIRECT3DVERTEXBUFFER8*) { return S_OK; }
        DWORD GetFVF() { return 0; }
        DWORD GetNumVertices() { return 0; }
        DWORD GetNumFaces() { return 0; }
        ULONG Release() { return 0; }
    };
    
    // Missing Constants
    #define D3DRS_FOGENABLE 28
    #define D3DTSS_TEXTURETRANSFORMFLAGS 24
    #define D3DTS_TEXTURE0 16
    #define D3DRS_DIFFUSEMATERIALSOURCE 0
    #define D3DRS_SPECULARMATERIALSOURCE 0
    #define D3DRS_AMBIENTMATERIALSOURCE 0
    #define D3DRS_EMISSIVEMATERIALSOURCE 0
    #define D3DRS_LINEPATTERN 0
    #define D3DRS_LASTPIXEL 0
    #define D3DRS_ALPHAREF 0
    #define D3DRS_ALPHAFUNC 0
    #define D3DRS_ZVISIBLE 0
    #define D3DRS_FOGSTART 36
    #define D3DRS_FOGEND 37
    #define D3DRS_FOGDENSITY 38
    #define D3DRS_EDGEANTIALIAS 0
    #define D3DRS_ZBIAS 0
    #define D3DRS_STENCILWRITEMASK 0
    #define D3DRS_AMBIENT 0
    #define D3DRS_LOCALVIEWER 0
    #define D3DRS_NORMALIZENORMALS 0
    #define D3DRS_VERTEXBLEND 0
    #define D3DRS_CLIPPLANEENABLE 0
    #define D3DRS_SOFTWAREVERTEXPROCESSING 0
    #define D3DRS_MULTISAMPLEANTIALIAS 0
    #define D3DRS_MULTISAMPLEMASK 0
    #define D3DRS_INDEXEDVERTEXBLENDENABLE 0
    #define D3DRS_COLORWRITEENABLE 0
    #define D3DRS_BLENDOP 0
    #define D3DRS_POSITIONDEGREE 0
    #define D3DRS_NORMALDEGREE 0
    #define D3DRS_SCISSORTESTENABLE 0
    #define D3DRS_SLOPESCALEDEPTHBIAS 0
    #define D3DRS_ANTIALIASEDLINEENABLE 0
    #define D3DRS_MINTESSELLATIONLEVEL 0
    #define D3DRS_MAXTESSELLATIONLEVEL 0
    #define D3DRS_ADAPTIVETESS_X 0
    #define D3DRS_ADAPTIVETESS_Y 0
    #define D3DRS_ADAPTIVETESS_Z 0
    #define D3DRS_ADAPTIVETESS_W 0
    #define D3DRS_ENABLEADAPTIVETESSELLATION 0
    #define D3DRS_TWOSIDEDSTENCILMODE 0
    #define D3DRS_CCW_STENCILFAIL 0
    #define D3DRS_CCW_STENCILZFAIL 0
    #define D3DRS_CCW_STENCILPASS 0
    #define D3DRS_CCW_STENCILFUNC 0
    #define D3DRS_COLORWRITEENABLE1 0
    #define D3DRS_COLORWRITEENABLE2 0
    #define D3DRS_COLORWRITEENABLE3 0
    #define D3DRS_BLENDFACTOR 0
    #define D3DRS_SRGBWRITEENABLE 0
    #define D3DRS_DEPTHBIAS 0
    #define D3DRS_WRAP8 0
    #define D3DRS_WRAP9 0
    #define D3DRS_WRAP10 0
    #define D3DRS_WRAP11 0
    #define D3DRS_WRAP12 0
    #define D3DRS_WRAP13 0
    #define D3DRS_WRAP14 0
    #define D3DRS_WRAP15 0
    #define D3DRS_SEPARATEALPHABLENDENABLE 0
    #define D3DRS_SRCBLENDALPHA 0
    #define D3DRS_DESTBLENDALPHA 0
    #define D3DRS_BLENDOPALPHA 0
    #define D3DRS_FOGCOLOR 34
    #define D3DRS_FOGTABLEMODE 35
    #define D3DRS_FOGVERTEXMODE 140
    #define D3DRS_RANGEFOGENABLE 48
    #define D3DRS_DITHERENABLE 0
    #define D3DRS_STENCILENABLE 0
    #define D3DRS_CLIPPING 0
    #define D3DRS_SPECULARENABLE 0
    #define D3DRS_COLORVERTEX 0
    #define D3DRS_WRAP0 0
    #define D3DRS_WRAP2 0
    #define D3DRS_WRAP3 0
    #define D3DRS_WRAP4 0
    #define D3DRS_WRAP5 0
    #define D3DRS_WRAP6 0
    #define D3DRS_WRAP7 0
    #define D3DTSS_TEXCOORDINDEX 11
    #define D3DDEVTYPE_HAL 1
    #define D3DDEVTYPE_REF 2
    #define D3DPTADDRESSCAPS_BORDER 0x00000080
    #define D3DDEVCAPS_HWTRANSFORMANDLIGHT 0x00000010
    #define D3DDEVCAPS_PUREDEVICE 0x00000010
    #define D3DCAPS2_CANRENDERWINDOWED 0x00080000
    #define D3DPMISCCAPS_CLIPTLVERTS 0x00000100
    #define D3DVTXPCAPS_DIRECTIONALLIGHTS 0x00000008
    #define D3DVTXPCAPS_POSITIONALLIGHTS 0x00000010
    #define D3DVTXPCAPS_TEXGEN 0x00000040
    
    #ifndef LUPUS2_D3D_CORE_TYPES
    struct D3DADAPTER_IDENTIFIER8 {
        char Driver[512];
        char Description[512];
    };
    #endif
    
    #define D3DVS_VERSION(a,b) 0
    #define D3D_SDK_VERSION 0
    #define D3DX_PI 3.141592654f
    
    // Abstracting Device
    typedef IDirect3DSurface8* LPDIRECT3DSURFACE8;
    typedef IDirect3DVertexBuffer8* LPDIRECT3DVERTEXBUFFER8; // VBO
    typedef IDirect3DIndexBuffer8* LPDIRECT3DINDEXBUFFER8; // IBO/EBO
    typedef ID3DXMesh* LPD3DXMESH;
    
    // D3D Enum Types Stubbed
    typedef int D3DFORMAT;
    typedef int D3DPRIMITIVETYPE;
    typedef int D3DDEVTYPE;
    typedef int D3DTEXTUREOP;
    typedef int D3DRENDERSTATETYPE;
    typedef int D3DTEXTURESTAGESTATETYPE;
    typedef int D3DBLEND;
    typedef int D3DCMPFUNC;
    typedef int D3DCULL;
    typedef int D3DLIGHTSTATETYPE;
    #ifndef LUPUS2_D3D_CORE_TYPES
    typedef int D3DMATERIAL8;
    typedef int D3DLIGHT8;
    #endif
    // Keep this as a distinct type from D3DXCOLOR.  The original Direct3D
    // headers expose two layout-compatible structs and legacy code overloads
    // functions for both of them.
    struct D3DCOLORVALUE {
        float r, g, b, a;
        D3DCOLORVALUE() : r(0), g(0), b(0), a(0) {}
        D3DCOLORVALUE(float _r, float _g, float _b, float _a)
            : r(_r), g(_g), b(_b), a(_a) {}
    };
    typedef int D3DFILLMODE;
    typedef int D3DMULTISAMPLE_TYPE;
    typedef int D3DTRANSFORMSTATETYPE;
    typedef int D3DBACKBUFFER_TYPE;
    
    #ifndef LUPUS2_D3D_CORE_TYPES
    struct D3DDISPLAYMODE {
        UINT Width, Height, RefreshRate;
        D3DFORMAT Format;
    };
    #endif
    
    typedef IDirect3D8* LPDIRECT3D8;
    typedef IDirect3DDevice8* LPDIRECT3DDEVICE8;
    typedef IDirect3DTexture8* LPDIRECT3DTEXTURE8;
    typedef IDirect3DTexture8* LPDIRECT3DBaseTexture8;
    
    #define D3DCOLOR_XRGB(r,g,b) ((DWORD)(0xff000000u | ((DWORD)(r) << 16) | ((DWORD)(g) << 8) | (DWORD)(b)))
    #define D3DCOLOR_ARGB(a,r,g,b) ((DWORD)(((DWORD)(a) << 24) | ((DWORD)(r) << 16) | ((DWORD)(g) << 8) | (DWORD)(b)))
    #define PALETTEENTRY GLCOLOR
    
    typedef ID3DXBuffer* LPD3DXBUFFER;
    typedef ID3DXMatrixStack* LPD3DXMATRIXSTACK;
    
    // Stub functions
    inline HRESULT D3DXCreateMatrixStack(DWORD flags, LPD3DXMATRIXSTACK* ppStack) { *ppStack = new ID3DXMatrixStack(); return *ppStack ? S_OK : E_OUTOFMEMORY; }
    inline HRESULT D3DXCreateSphere(LPDIRECT3DDEVICE8 pDevice, float radius, UINT slices, UINT stacks, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency) { *ppMesh = 0; return S_OK; }
    inline HRESULT D3DXCreateCylinder(LPDIRECT3DDEVICE8 pDevice, float radius1, float radius2, float length, UINT slices, UINT stacks, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency) { *ppMesh = 0; return S_OK; }
    inline HRESULT D3DXAssembleShaderFromFile(LPCSTR pSrcFile, DWORD flags, LPD3DXBUFFER* ppConstants, LPD3DXBUFFER* ppCompiledShader, LPD3DXBUFFER* ppCompilationErrors) { 
        if (ppCompiledShader) *ppCompiledShader = new ID3DXBuffer();
        if (ppCompilationErrors) *ppCompilationErrors = new ID3DXBuffer();
        return S_OK; 
    }

    inline HRESULT D3DXAssembleShader(LPCSTR pSrcData, UINT SrcDataLen, DWORD flags, LPD3DXBUFFER* ppConstants, LPD3DXBUFFER* ppCompiledShader, LPD3DXBUFFER* ppCompilationErrors) { 
        if (ppCompiledShader) *ppCompiledShader = new ID3DXBuffer();
        if (ppCompilationErrors) *ppCompilationErrors = new ID3DXBuffer();
        return S_OK; 
    }

    #define D3DX_FILTER_NONE    1
    #define D3DX_FILTER_LINEAR  2
    #define D3DX_DEFAULT        ((UINT)-1)
    
    #ifndef LUPUS2_D3D_CORE_TYPES
    inline HRESULT D3DXCreateTextureFromFileInMemoryEx(LPDIRECT3DDEVICE8 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, int Format, int Pool, DWORD Filter, DWORD MipFilter, GLCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE8* ppTexture) { *ppTexture = 0; return S_OK; }
    inline HRESULT D3DXCreateTexture(LPDIRECT3DDEVICE8 pDevice, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, int Format, int Pool, LPDIRECT3DTEXTURE8* ppTexture) { *ppTexture = 0; return S_OK; }
    inline HRESULT D3DXLoadSurfaceFromSurface(void* pDestSurface, const PALETTEENTRY* pDestPalette, const RECT* pDestRect, void* pSrcSurface, const PALETTEENTRY* pSrcPalette, const RECT* pSrcRect, DWORD Filter, GLCOLOR ColorKey) { return S_OK; }
    #endif

    // Constants stub
    #define D3DTS_WORLD 1
    #define D3DTS_VIEW 2
    #define D3DTS_PROJECTION 3

    #define D3DCLEAR_TARGET 1
    #define D3DCLEAR_ZBUFFER 2

    #define D3DFVF_XYZ 0x002
    #define D3DFVF_XYZRHW 0x004
    #define D3DFVF_NORMAL 0x010
    #define D3DFVF_DIFFUSE 0x040
    #define D3DFVF_SPECULAR 0x080
    #define D3DFVF_TEX1 0x100
    #define D3DFVF_TEX2 0x200
    #define D3DFVF_TEX3 0x300
    #define D3DFVF_TEXCOUNT_MASK 0xf00
    #define D3DFVF_TEXCOUNT_SHIFT 8
    #define D3DFVF_TEXCOORDSIZE1(CoordIndex) (3u << ((CoordIndex) * 2 + 16))
    #define D3DFVF_TEXCOORDSIZE2(CoordIndex) (0u)
    #define D3DFVF_TEXCOORDSIZE3(CoordIndex) (1u << ((CoordIndex) * 2 + 16))
    #define D3DFVF_TEXCOORDSIZE4(CoordIndex) (2u << ((CoordIndex) * 2 + 16))
    inline UINT D3DXGetFVFVertexSize(DWORD fvf) {
        UINT size = 0;
        if (fvf & D3DFVF_XYZRHW) size += sizeof(float) * 4;
        else if (fvf & D3DFVF_XYZ) size += sizeof(float) * 3;
        if (fvf & D3DFVF_NORMAL) size += sizeof(float) * 3;
        if (fvf & D3DFVF_DIFFUSE) size += sizeof(DWORD);
        if (fvf & D3DFVF_SPECULAR) size += sizeof(DWORD);
        size += ((fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT) * sizeof(float) * 2;
        return size;
    }
    
    #define D3DRS_ZENABLE 7
    #define D3DRS_FILLMODE 8
    #define D3DRS_SHADEMODE 9
    #define D3DRS_ZWRITEENABLE 14
    #define D3DRS_ALPHATESTENABLE 15
    #define D3DRS_SRCBLEND 19
    #define D3DRS_DESTBLEND 20
    #define D3DRS_CULLMODE 22
    #define D3DRS_ZFUNC 23
    #define D3DRS_ALPHAREF 24
    #define D3DRS_ALPHAFUNC 25
    #define D3DRS_ALPHABLENDENABLE 27
    #define D3DRS_LIGHTING 137
    #define D3DRS_TEXTUREFACTOR 60
    #define D3DTSS_COLORARG1 1
    #define D3DTSS_COLORARG2 2
    #define D3DTSS_COLOROP 3
    #define D3DTSS_ALPHAARG1 4
    #define D3DTSS_ALPHAARG2 5
    #define D3DTSS_ALPHAOP 6
    #define D3DTSS_MINFILTER 17
    #define D3DTSS_MAGFILTER 16
    #define D3DTSS_MIPFILTER 18
    #define D3DTSS_ADDRESSU 13
    #define D3DTSS_ADDRESSV 14
    
    #define D3DPT_POINTLIST 1
    #define D3DPT_LINELIST 2
    #define D3DPT_LINESTRIP 3
    #define D3DPT_TRIANGLELIST 4
    #define D3DPT_TRIANGLESTRIP 5
    #define D3DPT_TRIANGLEFAN 6
    
    #define D3DFMT_UNKNOWN 0
    #define D3DFMT_R8G8B8 1
    #define D3DFMT_A8R8G8B8 2
    #define D3DFMT_X8R8G8B8 3
    #define D3DFMT_A4R4G4B4 4
    #define D3DFMT_A1R5G5B5 5
    #define D3DFMT_X1R5G5B5 6
    #define D3DFMT_A16B16G16R16 7
    #define D3DFMT_DXT1 8
    #define D3DFMT_DXT3 9
    #define D3DFMT_DXT5 10
    #define D3DFMT_D16 11
    #define D3DFMT_D15S1 12
    #define D3DFMT_D24X8 13
    #define D3DFMT_D24S8 14
    #define D3DFMT_D24X4S4 15
    #define D3DFMT_D32 16
    #define D3DFMT_INDEX16 17
    #define D3DFMT_R5G6B5 18
    
    #ifndef LUPUS2_D3D_CORE_TYPES
    #define D3DLOCKED_RECT void*
    #endif
    
    #define D3DTA_TFACTOR 1
    #define D3DTA_TEXTURE 2
    #define D3DTA_CURRENT 3
    #define D3DTA_DIFFUSE 4
    
    // Direct3D 8 values.  These must remain distinct: the former shim mapped
    // SELECTARG2 and DISABLE to 2, making terrain passes flicker on GLES.
    #define D3DTOP_DISABLE 1
    #define D3DTOP_SELECTARG1 2
    #define D3DTOP_SELECTARG2 3
    #define D3DTOP_MODULATE 4
    #define D3DTOP_ADD 7
    
    #define D3DBLEND_ZERO 1
    #define D3DBLEND_ONE 2
    #define D3DBLEND_SRCCOLOR 3
    #define D3DBLEND_INVSRCCOLOR 4
    #define D3DBLEND_SRCALPHA 5
    #define D3DBLEND_INVSRCALPHA 6
    #define D3DBLEND_DESTALPHA 7
    #define D3DBLEND_INVDESTALPHA 8
    #define D3DBLEND_DESTCOLOR 9
    #define D3DBLEND_INVDESTCOLOR 10
    #define D3DBLEND_SRCALPHASAT 11
    
    #define D3DCULL_CCW 1
    #define D3DCULL_CW 2
    #define D3DCULL_NONE 3
    
    #define D3DFILL_POINT 1
    #define D3DFILL_WIREFRAME 2
    #define D3DFILL_SOLID 3
    
    #define D3DLIGHT_POINT 1
    #define D3DSHADE_FLAT 1
    #define D3DSHADE_GOURAUD 2
    
    #define D3DCMP_LESSEQUAL 1
    #define D3DCMP_LESS 2
    #define D3DCMP_GREATER 2
    #define D3DCMP_GREATEREQUAL 7
    #define D3DFOG_EXP 1
    #define D3DFOG_NONE 0
    #define D3DFOG_LINEAR 3
    #define D3DMCS_MATERIAL 0
    #define D3DMCS_COLOR1 1
    #define D3DVBF_DISABLE 0
    #define D3DBLENDOP_ADD 1
    #define D3DRS_WRAP1 0
    #define D3DPTFILTERCAPS_MINFANISOTROPIC 0x400
    #define D3DPTFILTERCAPS_MAGFANISOTROPIC 0x4000000
    #define D3DTTFF_DISABLE 0
    #define D3DTTFF_COUNT2 2
    #define D3DTSS_TCI_CAMERASPACEPOSITION 0x00010000
    #define D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR 0x00030000
    #define D3DTS_TEXTURE1 17
    #define D3DTOP_MODULATEALPHA_ADDCOLOR 18
    #define D3DTOP_MODULATEINVALPHA_ADDCOLOR 19
    #define D3DTOP_BLENDDIFFUSEALPHA 12
    #define D3DTADDRESS_WRAP 1
    #define D3DTEXF_NONE 0
    #define D3DTEXF_LINEAR 2
    #define D3DTEXF_ANISOTROPIC 3
    #define D3DTSS_MAXANISOTROPY 21
    #define D3DLIGHT_POINT 1
    #define D3DLIGHT_SPOT 3
    #define D3DLIGHT_DIRECTIONAL 3
    
    #define D3DTEXF_POINT 1
    #define D3DTADDRESS_CLAMP 3
    #define D3DTADDRESS_BORDER 4
    #define D3DTSS_BORDERCOLOR 12
    #define D3DPOOL_MANAGED 1
    #define D3DPOOL_SYSTEMMEM 2
    #define D3DPOOL_DEFAULT 3
    #define D3DPOOL_SCRATCH 4
    #define D3DLOCK_READONLY 1
    #define D3DLOCK_NO_DIRTY_UPDATE 2
    #define D3DLOCK_DISCARD 3
    #define D3DUSAGE_WRITEONLY 1
    #define D3DUSAGE_DYNAMIC 2
    #define D3DUSAGE_DEPTHSTENCIL 3
    #define D3DUSAGE_RENDERTARGET 4
    
    #define D3D_OK 0
    #define D3DERR_DEVICELOST 1
    #define D3DERR_DEVICENOTRESET 2
    #define D3DERR_INVALIDCALL 3
    #define D3DERR_NOTAVAILABLE 4
    #define D3DERR_OUTOFVIDEOMEMORY 5
    #define D3DERR_WASSTILLDRAWING 6
    
    #define D3DADAPTER_DEFAULT 0
    #define D3DSWAPEFFECT_COPY 1
    #define D3DSWAPEFFECT_DISCARD 2
    #define D3DPRESENT_INTERVAL_IMMEDIATE 1
    #define D3DPRESENTFLAG_LOCKABLE_BACKBUFFER 1
    #define D3DENUM_NO_WHQL_LEVEL 1
    
    #define D3DCREATE_PUREDEVICE 1
    #define D3DCREATE_HARDWARE_VERTEXPROCESSING 2
    #define D3DCREATE_MIXED_VERTEXPROCESSING 3
    #define D3DCREATE_SOFTWARE_VERTEXPROCESSING 4
    
    #define D3DRTYPE_SURFACE 1
    #define D3DRTYPE_TEXTURE 2
    #define D3DMULTISAMPLE_NONE 0
    
    #define D3DDEVICETYPE_HAL 1
    #define D3DDEVICETYPE_REF 2
    
    #define D3DPMISCCAPS_CLIPTLVERTS 1
    #define D3DVTXPCAPS_DIRECTIONALLIGHTS 1
    #define D3DVTXPCAPS_POSITIONALLIGHTS 2
    #define D3DVTXPCAPS_TEXGEN 3
    #define D3DCAPS2_CANRENDERWINDOWED 1
    #define D3DDEVCAPS_HWTRANSFORMANDLIGHT 1
    #define D3DDEVCAPS_PUREDEVICE 2

    #define D3DVS_VERSION(major, minor) (0)
#endif

#ifdef LUPUS2_GRP_OPENGL_IOS_ANDROID_ALIAS
    #undef __android_log_print
    #undef ANDROID_LOG_ERROR
    #undef ANDROID
    #undef LUPUS2_GRP_OPENGL_IOS_ANDROID_ALIAS
#endif
