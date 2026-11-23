#include "StdAfx.h"
#include "PythonApplication.h"
#include "PythonNetworkStream.h"
#include "../EterPythonLib/PythonSlotWindow.h"
#include "../mobile/MobileClientAPI.h"
#if defined(__ANDROID__)
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <GLES2/gl2.h>
#elif defined(__IOS__)
#include <OpenGLES/ES3/gl.h>
#include <stdio.h>
#endif
#include <unistd.h>

#if defined(__ANDROID__)
#define LOG_TAG "Metin2Mobile"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#define LOGI(...) do { fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); } while (0)
#define LOGE(...) do { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while (0)
#endif

namespace {
CPythonApplication* g_pythonApplication = NULL;
int g_androidSurfaceWidth = 1;
int g_androidSurfaceHeight = 1;
float g_androidTouchDownX = 0.0f;
float g_androidTouchDownY = 0.0f;
float g_androidLastTouchY = 0.0f;
bool g_androidTouchDragging = false;
bool g_androidTouchScrolling = false;
bool g_androidDeferredSlotPress = false;
bool g_androidSlotDragStarted = false;
DWORD g_androidLastActorTapVID = 0;
DWORD g_androidLastActorTapTime = 0;
bool g_androidPendingActorTap = false;
bool g_androidPendingActorDoubleTap = false;
int g_androidPendingActorPickFrames = 0;
volatile int g_androidFrameStage = 0;
volatile DWORD g_androidFrameStageSince = 0;
volatile unsigned int g_androidMobileControlMask = 0;
volatile int g_androidMobileQuickPage = 0;

bool AndroidIsPointingAtSlot(UI::CWindowManager& wndMgr)
{
    for (UI::CWindow* window = wndMgr.GetPointWindow(); window; window = window->GetParent())
    {
        if (window->IsType(UI::CSlotWindow::Type()))
        {
			// Quest rows are action buttons and must still open with one tap.
			// Item, equipment, quick-slot and skill slots use the deferred
			// behavior so a tap inspects and a drag attaches.
			const char* name = window->GetName();
			if (name && (strstr(name, "Quest") || strstr(name, "quest")))
				return false;
            return true;
		}
    }
    return false;
}

}

extern "C" unsigned int AndroidGetMobileControlMask()
{
    return g_androidMobileControlMask;
}

extern "C" int AndroidGetMobileQuickPage()
{
    return g_androidMobileQuickPage;
}

extern "C" void AndroidSetFrameStage(int stage)
{
    g_androidFrameStage = stage;
    g_androidFrameStageSince = ELTimer_GetMSec();
}

extern "C" int AndroidGetRenderGameStage();
extern "C" DWORD AndroidGetRenderGameCalls();

int AndroidGetVirtualWidth()
{
    // Keep the PC inventory/dialogs (up to 565 px tall) above the taskbar.
    // All rendering and touch conversion use this same logical resolution.
    return std::max(1280, (640 * g_androidSurfaceWidth +
        std::max(1, g_androidSurfaceHeight) / 2) / std::max(1, g_androidSurfaceHeight));
}

int AndroidGetVirtualHeight()
{
    return std::max(1, (AndroidGetVirtualWidth() * g_androidSurfaceHeight + g_androidSurfaceWidth / 2) / g_androidSurfaceWidth);
}

float AndroidGetInterfaceScale()
{
    // Keep the 3D world at its existing virtual resolution while making the
    // original desktop interface easier to use with touch input.
    return 1.12f;
}

float AndroidGetInventoryInterfaceScale()
{
    // Inventory is the tallest stock PC window. Keep it slightly enlarged,
    // but below the global UI factor so its full frame remains visible.
    return 1.055f;
}

int AndroidGetInterfaceWidth()
{
    return std::max(1, static_cast<int>(AndroidGetVirtualWidth() / AndroidGetInterfaceScale() + 0.5f));
}

int AndroidGetInterfaceHeight()
{
    return std::max(1, static_cast<int>(AndroidGetVirtualHeight() / AndroidGetInterfaceScale() + 0.5f));
}

int AndroidGetSurfaceWidth()
{
    return g_androidSurfaceWidth;
}

int AndroidGetSurfaceHeight()
{
    return g_androidSurfaceHeight;
}

extern bool AndroidClientInitialize();

extern "C" bool Lupus2MobileInitialize(const char* workingDirectory, int width, int height) {
    LOGI("Initializing Metin2 Mobile Engine (%dx%d)", width, height);
    g_androidSurfaceWidth = std::max(1, (int)width);
    g_androidSurfaceHeight = std::max(1, (int)height);
    glViewport(0, 0, width, height);

    if (!workingDirectory || chdir(workingDirectory) != 0) {
        LOGE("Unable to select application working directory");
        return false;
    }

    // GLSurfaceView may recreate or resize its surface more than once.
    if (g_pythonApplication)
        return true;

    if (!AndroidClientInitialize()) {
        LOGE("Mobile client bootstrap failed");
        return false;
    }

    g_pythonApplication = &CPythonApplication::Instance();
#if defined(__ANDROID__) || defined(__IOS__)
    // Android's GLSurfaceView already paces frames.  Legacy late-frame
    // skipping can otherwise suppress the first render after startup. Both
    // mobile view loops already pace at the display refresh rate.
    g_pythonApplication->SetFrameSkip(false);
#endif
    
    LOGI("Metin2 Mobile Engine Initialized Successfuly");
    return true;
}

extern "C" void Lupus2MobileRender(void) {
    if (g_pythonApplication) {
        static DWORD dwLastTime = 0;
        DWORD dwCurrentTime = ELTimer_GetMSec();
        
        if (dwLastTime == 0) dwLastTime = dwCurrentTime;
        
        // The native engine owns the update/render ordering and frame pacing.
        AndroidSetFrameStage(1);
        g_pythonApplication->Process();
        AndroidSetFrameStage(0);

		// The stock PC actor picker is updated in the game-frame pipeline, not in
		// the Android event callback.  Resolve a tap only after one full frame has
		// rebuilt the 3D ray and the following frame has run __NEW_Pick().  This is
		// the same m_pkInstPick that produces the red circle seen by the player.
		if (g_androidPendingActorTap)
		{
			if (g_androidPendingActorPickFrames > 0)
			{
				--g_androidPendingActorPickFrames;
			}
			else
			{
				DWORD pickedVID = 0;
				if (CPythonCharacterManager::Instance().OLD_GetPickedInstanceVID(&pickedVID) &&
					CPythonPlayer::Instance().AndroidHandleActorTap(pickedVID, false))
				{
					g_androidLastActorTapVID = pickedVID;
					g_androidLastActorTapTime = ELTimer_GetMSec();
					LOGI("TOUCH_ACTOR frame_select picked=%u target=%u",
						(unsigned int)pickedVID,
						(unsigned int)CPythonPlayer::Instance().GetTargetVID());
					if (g_androidPendingActorDoubleTap)
					{
						LOGI("TOUCH_ACTOR frame_attack locked_vid=%u", (unsigned int)pickedVID);
						CPythonPlayer::Instance().AndroidHandleActorTap(pickedVID, true);
						g_androidLastActorTapVID = 0;
					}
				}
				else
				{
					g_androidLastActorTapVID = 0;
					LOGI("TOUCH_ACTOR frame_select miss");
				}
				g_androidPendingActorTap = false;
				g_androidPendingActorDoubleTap = false;
			}
		}
        
        dwLastTime = dwCurrentTime;
    }
}

#if defined(__ANDROID__)
extern "C" {

JNIEXPORT void JNICALL Java_com_metin2_client_NativeLib_init(JNIEnv* env, jobject,
                                                             jobject assetManager,
                                                             jstring workingDirectory,
                                                             jint width, jint height) {
    extern AAssetManager* g_pAssetManager;
    g_pAssetManager = AAssetManager_fromJava(env, assetManager);

    const char* nativeWorkingDirectory = env->GetStringUTFChars(workingDirectory, NULL);
    Lupus2MobileInitialize(nativeWorkingDirectory, (int)width, (int)height);
    if (nativeWorkingDirectory)
        env->ReleaseStringUTFChars(workingDirectory, nativeWorkingDirectory);
}

JNIEXPORT void JNICALL Java_com_metin2_client_NativeLib_render(JNIEnv*, jobject) {
    Lupus2MobileRender();
}
}
#endif

extern "C" bool Lupus2MobileIsMapReady(void) {
    return g_pythonApplication && CPythonBackground::Instance().IsMapReady();
}

extern "C" void Lupus2MobileResumeFromBackground(int64_t elapsedMilliseconds) {
    if (!g_pythonApplication || elapsedMilliseconds < 60000)
        return;

    CPythonNetworkStream& network = CPythonNetworkStream::Instance();
    if (!network.IsOnline() && !CPythonBackground::Instance().IsMapReady())
        return;

    LOGI("Background session expired after %lld ms; returning to login",
         (long long)elapsedMilliseconds);
    network.ClearLoginInfo();
    network.ClosePhase();
}

extern "C" bool Lupus2MobileIsTextInputActive(void) {
    return g_pythonApplication && CPythonIME::Instance().IsCaptureEnabled();
}

extern "C" bool Lupus2MobileIsTextInputSecret(void) {
    if (!g_pythonApplication || !CPythonIME::Instance().IsCaptureEnabled())
        return false;

    UI::CWindow* activeWindow = UI::CWindowManager::Instance().GetActivateWindow();
    if (!activeWindow || !activeWindow->IsType(UI::CTextLine::Type()))
        return false;

    return static_cast<UI::CTextLine*>(activeWindow)->IsSecret();
}

extern "C" bool Lupus2MobileIsLoginTextInputFocused(void) {
    if (!g_pythonApplication || !CPythonIME::Instance().IsCaptureEnabled())
        return false;

    UI::CWindow* activeWindow = UI::CWindowManager::Instance().GetActivateWindow();
    if (!activeWindow || !activeWindow->IsType(UI::CTextLine::Type()))
        return false;

    const char* name = activeWindow->GetName();
    return name && (!strcmp(name, "ID_EditLine") || !strcmp(name, "Password_EditLine"));
}

static bool g_androidKeyboardRequested = false;

extern "C" bool Lupus2MobileConsumeKeyboardRequest(void) {
    const bool requested = g_androidKeyboardRequested;
    g_androidKeyboardRequested = false;
    return requested;
}

extern "C" void Lupus2MobileTextInput(const uint16_t* characters, int length) {
    if (!g_pythonApplication || !characters || length <= 0)
        return;
    for (int i = 0; i < length; ++i)
        CPythonIME::Instance().WMChar(NULL, WM_CHAR, (WPARAM)characters[i], 0);
}

extern "C" void Lupus2MobileTextKey(int key) {
    if (g_pythonApplication)
        CPythonIME::Instance().WMChar(NULL, WM_CHAR, (WPARAM)key, 0);
}

extern "C" bool Lupus2MobileIsTouchOverUi(float x, float y) {
    if (!g_pythonApplication)
        return false;

    const int eventX = std::max(0, std::min(g_androidSurfaceWidth - 1, (int)x));
    const int eventY = std::max(0, std::min(g_androidSurfaceHeight - 1, (int)y));
    // This JNI entry is invoked through GLSurfaceView.queueEvent so picking and
    // the Python UI tree remain on the game thread.  RunMouseMove refreshes the
    // picked window without clicking it.
    g_pythonApplication->AndroidTouchMove(eventX, eventY);
    UI::CWindow* pointed = UI::CWindowManager::Instance().GetPointWindow();
    if (!pointed)
        return false;

    // The full-screen Python GameWindow is the 3D world hit target.  It must
    // not mask the permanent Android controls.  Every real PC interface child
    // (inventory, dialogs, slots, taskbar, chat, etc.) keeps priority over the
    // mobile overlay when it occupies the same screen area.
    const char* pointedName = pointed->GetName();
    if (pointedName && (!strcmp(pointedName, "game") || !strcmp(pointedName, "GAME")))
        return false;
    return true;
}

extern "C" void Lupus2MobileTouchEvent(int action, float x, float y) {
    if (g_pythonApplication) {
        // action matches Android MotionEvent actions:
        // 0: ACTION_DOWN
        // 1: ACTION_UP
        // 2: ACTION_MOVE
        
        UI::CWindowManager& wndMgr = UI::CWindowManager::Instance();
		const int surfaceWidth = g_androidSurfaceWidth;
		const int surfaceHeight = g_androidSurfaceHeight;
        // The window manager performs physical-surface -> virtual-UI mapping.
        // Feeding it already-scaled coordinates would scale every touch twice.
        const int eventX = std::max(0, std::min(surfaceWidth - 1, (int)x));
        const int eventY = std::max(0, std::min(surfaceHeight - 1, (int)y));

        if (action == 0) { // ACTION_DOWN
			g_androidKeyboardRequested = false;
			g_androidTouchDownX = x;
			g_androidTouchDownY = y;
			g_androidLastTouchY = y;
			g_androidTouchDragging = false;
			g_androidTouchScrolling = false;
			g_androidDeferredSlotPress = false;
			g_androidSlotDragStarted = false;
			// A tap outside the currently focused EditLine must release the
			// native IME capture, otherwise Android immediately reopens its
			// keyboard after every subsequent button/list tap.
			g_pythonApplication->AndroidTouchMove(eventX, eventY);
			if (CPythonIME::Instance().IsCaptureEnabled())
			{
				UI::CWindow* focused = wndMgr.GetActivateWindow();
				UI::CWindow* pointed = wndMgr.GetPointWindow();
				if (focused && pointed != focused)
					wndMgr.DeactivateWindow();
			}
			const bool captureBeforeClick = CPythonIME::Instance().IsCaptureEnabled();
			// A slot tap is selection/inspection on touch screens.  Delay its
			// desktop left-button-down action until the finger actually drags;
			// otherwise every ordinary tap immediately attaches the item/skill.
			g_androidDeferredSlotPress = AndroidIsPointingAtSlot(wndMgr);
			if (!g_androidDeferredSlotPress)
				g_pythonApplication->AndroidTouchDown(eventX, eventY);
			// Open Android's keyboard only when this exact tap activated an
			// EditLine. Merely having a Python EditLine focused (for example the
			// default name field on character creation) must not reopen the IME.
			g_androidKeyboardRequested = !captureBeforeClick && CPythonIME::Instance().IsCaptureEnabled();
		} else if (action == 1) { // ACTION_UP
			if (g_androidDeferredSlotPress && !g_androidSlotDragStarted)
			{
				// The preceding hover already displayed the slot tooltip.  Releasing
				// a tap must not attach or drop the icon at the next screen touch.
				wndMgr.ResetCapture();
			}
			else if (!g_androidTouchScrolling)
				g_pythonApplication->AndroidTouchUp(eventX, eventY);
			else
				wndMgr.ResetCapture();
			// A desktop ground click starts continuous mouse movement.  Touch
			// movement belongs to the virtual joystick, so a released world tap
			// must never leave that desktop movement state latched.
			if (CPythonBackground::Instance().IsMapReady())
				g_pythonApplication->AndroidStopMovement();
			g_androidTouchDragging = false;
			g_androidTouchScrolling = false;
			g_androidDeferredSlotPress = false;
			g_androidSlotDragStarted = false;
        } else if (action == 2) { // ACTION_MOVE
			const float totalDx = x - g_androidTouchDownX;
			const float totalDy = y - g_androidTouchDownY;
			if (!g_androidTouchDragging && totalDx * totalDx + totalDy * totalDy > 400.0f)
			{
				g_androidTouchDragging = true;
				if (g_androidDeferredSlotPress)
				{
					const int downX = std::max(0, std::min(surfaceWidth - 1, (int)g_androidTouchDownX));
					const int downY = std::max(0, std::min(surfaceHeight - 1, (int)g_androidTouchDownY));
					// Start attachment at the slot that was originally touched, then
					// immediately move it to the current finger position.
					g_pythonApplication->AndroidTouchMove(downX, downY);
					g_pythonApplication->AndroidTouchDown(downX, downY);
					g_androidSlotDragStarted = true;
				}
			}
			if (g_androidTouchDragging && !g_androidDeferredSlotPress)
			{
#ifdef ENABLE_MOUSEWHEEL_EVENT
				const float step = y - g_androidLastTouchY;
				if (step >= 12.0f || step <= -12.0f)
				{
					if (g_pythonApplication->AndroidTouchScroll(eventX, eventY, step < 0.0f ? 120 : -120))
					{
						g_androidTouchScrolling = true;
						wndMgr.ResetCapture();
					}
					g_androidLastTouchY = y;
				}
#endif
			}
			g_pythonApplication->AndroidTouchMove(eventX, eventY);
		} else if (action == 3) { // ACTION_CANCEL
			wndMgr.ResetCapture();
			g_pythonApplication->AndroidTouchUp(eventX, eventY);
			g_pythonApplication->AndroidStopMovement();
			g_androidTouchDragging = false;
			g_androidTouchScrolling = false;
			g_androidDeferredSlotPress = false;
			g_androidSlotDragStarted = false;
		} else if (action == 4) { // LONG PRESS -> RIGHT CLICK
			if (g_androidDeferredSlotPress && !g_androidSlotDragStarted)
			{
				const int downX = std::max(0, std::min(surfaceWidth - 1, (int)g_androidTouchDownX));
				const int downY = std::max(0, std::min(surfaceHeight - 1, (int)g_androidTouchDownY));
				g_pythonApplication->AndroidTouchMove(downX, downY);
				g_pythonApplication->AndroidTouchDown(downX, downY);
				g_androidSlotDragStarted = true;
				g_androidTouchDragging = true;
			}
			else
			{
				wndMgr.ResetCapture();
				g_pythonApplication->AndroidTouchRightClick(eventX, eventY);
			}
		} else if (action == 5) { // DOUBLE TAP
			wndMgr.ResetCapture();
			g_pythonApplication->AndroidTouchDoubleClick(eventX, eventY);
		} else if (action == 6) { // PINCH ZOOM; x carries scale factor
			g_pythonApplication->AndroidTouchZoom(x > 1.0f ? 1 : -1);
		} else if (action == 7) { // CAMERA DRAG DOWN
			// Java establishes the relative-delta anchor; no legacy desktop drag.
		} else if (action == 8) { // CAMERA DRAG MOVE
			g_pythonApplication->AndroidTouchCameraDelta(x, y);
		} else if (action == 9) { // CAMERA DRAG UP
			// No inertial desktop EndDrag: rotation stops with the fingers.
		} else if (action == 10 || action == 11) { // WORLD ACTOR TAP / DOUBLE TAP
			// Mobile world input deliberately does not emit desktop mouse down/up:
			// those events would also move toward the ground.  Refresh the PC
			// picker at the released finger and dispatch only the stock actor path.
			if (action == 10)
			{
				g_pythonApplication->AndroidTouchMove(eventX, eventY);
				g_androidPendingActorTap = true;
				g_androidPendingActorDoubleTap = false;
				g_androidPendingActorPickFrames = 1;
				LOGI("TOUCH_ACTOR queued x=%d y=%d", eventX, eventY);
			}
			else
			{
				// GestureDetector already confirmed that both taps were close.  Attack
				// the VID fixed by the first tap instead of picking the crowded scene
				// again at a slightly different finger coordinate.
				const DWORD now = ELTimer_GetMSec();
				if (g_androidLastActorTapVID && now - g_androidLastActorTapTime <= 700)
				{
					LOGI("TOUCH_ACTOR attack locked_vid=%u", (unsigned int)g_androidLastActorTapVID);
					CPythonPlayer::Instance().AndroidHandleActorTap(g_androidLastActorTapVID, true);
				}
				else if (g_androidPendingActorTap)
				{
					g_androidPendingActorDoubleTap = true;
					LOGI("TOUCH_ACTOR attack waiting_for_frame_pick");
				}
				else
				{
					LOGI("TOUCH_ACTOR attack ignored no_locked_vid");
				}
				g_androidLastActorTapVID = 0;
			}
		} else if (action == 201) {
			// Android Back dismisses the software keyboard without sending the PC
			// Escape binding. Release only EditLine focus: Python keeps chat open
			// and restores its original bottom position on the following frame.
			if (CPythonIME::Instance().IsCaptureEnabled())
				wndMgr.DeactivateWindow();
		} else if (action == 202) {
			// Outside tap with no active IME: reuse GameWindow's original Return
			// handler. It closes an open chat, while the Android guard makes this a
			// no-op when chat is already closed.
			g_pythonApplication->RunIMEReturnEvent();
		} else if (action == 200) {
			g_androidMobileQuickPage = 1 - g_androidMobileQuickPage;
		} else if (action >= 100 && action <= 135) {
			// The mobile HUD is only another input device for the original PC
			// bindings.  Keep these indices synchronized with TouchControlsView.
			static const int controlKeys[] = {
				DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT,
				DIK_SPACE,
				DIK_1, DIK_2, DIK_3, DIK_4, DIK_5,
				DIK_TAB, DIK_F1, DIK_F2, DIK_F3, DIK_F4,
				0, // Ctrl+G riding toggle, handled below
				DIK_Z,
				0 // Chat, handled through the original IME Return event below
			};
			const int encoded = action - 100;
			const int keyIndex = encoded / 2;
			const bool pressed = (encoded % 2) == 0;
			if (pressed)
				g_androidMobileControlMask |= (1u << keyIndex);
			else
				g_androidMobileControlMask &= ~(1u << keyIndex);
			if (keyIndex == 15)
			{
				// Reuse the PC client's Ctrl+G path (/ride), including all of its
				// existing gameplay checks.  This is a button chord, not a new rule.
				if (pressed)
				{
					g_pythonApplication->AndroidControlKey(DIK_LCONTROL, true);
					g_pythonApplication->AndroidControlKey(DIK_G, true);
				}
				else
				{
					g_pythonApplication->AndroidControlKey(DIK_G, false);
					g_pythonApplication->AndroidControlKey(DIK_LCONTROL, false);
				}
			}
			else if (keyIndex == 17)
			{
				if (pressed)
				{
					// A dedicated chat button must always follow GameWindow's PC
					// Return binding.  Do not let a stale EditLine capture this tap;
					// Android keyboard Enter still targets ChatLine normally.
					wndMgr.DeactivateWindow();
					g_pythonApplication->RunIMEReturnEvent();
				}
			}
			else if (keyIndex >= 0 && keyIndex < (int)(sizeof(controlKeys) / sizeof(controlKeys[0])))
				g_pythonApplication->AndroidControlKey(controlKeys[keyIndex], pressed);
        }
    }
}

#if defined(__ANDROID__)
extern "C" {
JNIEXPORT jboolean JNICALL Java_com_metin2_client_NativeLib_isMapReady(JNIEnv*, jclass) {
    return Lupus2MobileIsMapReady() ? JNI_TRUE : JNI_FALSE;
}
JNIEXPORT void JNICALL Java_com_metin2_client_NativeLib_resumeFromBackground(JNIEnv*, jclass, jlong elapsed) {
    Lupus2MobileResumeFromBackground((int64_t)elapsed);
}
JNIEXPORT jboolean JNICALL Java_com_metin2_client_NativeLib_isTextInputActive(JNIEnv*, jclass) {
    return Lupus2MobileIsTextInputActive() ? JNI_TRUE : JNI_FALSE;
}
JNIEXPORT jboolean JNICALL Java_com_metin2_client_NativeLib_isTextInputSecret(JNIEnv*, jclass) {
    return Lupus2MobileIsTextInputSecret() ? JNI_TRUE : JNI_FALSE;
}
JNIEXPORT jboolean JNICALL Java_com_metin2_client_NativeLib_isLoginTextInputFocused(JNIEnv*, jclass) {
    return Lupus2MobileIsLoginTextInputFocused() ? JNI_TRUE : JNI_FALSE;
}
JNIEXPORT jboolean JNICALL Java_com_metin2_client_NativeLib_consumeKeyboardRequest(JNIEnv*, jclass) {
    return Lupus2MobileConsumeKeyboardRequest() ? JNI_TRUE : JNI_FALSE;
}
JNIEXPORT void JNICALL Java_com_metin2_client_NativeLib_textInput(JNIEnv* env, jclass, jstring value) {
    if (!value)
        return;
    const jchar* chars = env->GetStringChars(value, NULL);
    Lupus2MobileTextInput(reinterpret_cast<const uint16_t*>(chars), (int)env->GetStringLength(value));
    env->ReleaseStringChars(value, chars);
}
JNIEXPORT void JNICALL Java_com_metin2_client_NativeLib_textKey(JNIEnv*, jclass, jint key) {
    Lupus2MobileTextKey((int)key);
}
JNIEXPORT jboolean JNICALL Java_com_metin2_client_NativeLib_isTouchOverUi(JNIEnv*, jclass, jfloat x, jfloat y) {
    return Lupus2MobileIsTouchOverUi((float)x, (float)y) ? JNI_TRUE : JNI_FALSE;
}
JNIEXPORT void JNICALL Java_com_metin2_client_NativeLib_touchEvent(JNIEnv*, jobject, jint action, jfloat x, jfloat y) {
    Lupus2MobileTouchEvent((int)action, (float)x, (float)y);
}
}
#endif
