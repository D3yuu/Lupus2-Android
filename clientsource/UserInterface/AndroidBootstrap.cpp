#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>

#define LOG_TAG "Lupus2Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace
{
    int g_width = 1;
    int g_height = 1;
    float g_touchX = 0.0f;
    float g_touchY = 0.0f;
}

extern "C"
{
JNIEXPORT void JNICALL
Java_com_metin2_client_NativeLib_init(JNIEnv*, jobject, jobject, jstring, jint width, jint height)
{
    g_width = width > 0 ? width : 1;
    g_height = height > 0 ? height : 1;
    glViewport(0, 0, g_width, g_height);
    LOGI("Lupus2 native ARM64 bootstrap initialized at %dx%d", g_width, g_height);
}

JNIEXPORT void JNICALL
Java_com_metin2_client_NativeLib_render(JNIEnv*, jobject)
{
    const float x = g_touchX / static_cast<float>(g_width);
    const float y = g_touchY / static_cast<float>(g_height);
    glClearColor(0.015f + x * 0.04f, 0.025f + y * 0.05f, 0.075f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

JNIEXPORT void JNICALL
Java_com_metin2_client_NativeLib_touchEvent(JNIEnv*, jobject, jint, jfloat x, jfloat y)
{
    g_touchX = x;
    g_touchY = y;
}
}
