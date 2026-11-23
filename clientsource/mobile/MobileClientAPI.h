#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Platform-neutral entry points used by the Android JNI adapter and the iOS
// Objective-C++ host. Coordinates are always physical surface pixels; the
// original PC client performs its own virtual-resolution conversion.
bool Lupus2MobileInitialize(const char* workingDirectory, int width, int height);
void Lupus2MobileRender(void);
void Lupus2MobileTouchEvent(int action, float x, float y);

bool Lupus2MobileIsTouchOverUi(float x, float y);
bool Lupus2MobileIsMapReady(void);
bool Lupus2MobileIsTextInputActive(void);
bool Lupus2MobileIsTextInputSecret(void);
bool Lupus2MobileIsLoginTextInputFocused(void);
bool Lupus2MobileConsumeKeyboardRequest(void);

void Lupus2MobileTextInput(const uint16_t* characters, int length);
void Lupus2MobileTextKey(int key);
void Lupus2MobileResumeFromBackground(int64_t elapsedMilliseconds);

#ifdef __cplusplus
}
#endif
