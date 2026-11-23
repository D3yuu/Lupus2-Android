package com.metin2.client;

public class NativeLib {
    static {
        System.loadLibrary("metin2_mobile");
    }

    public static native void init(Object assetManager, String workingDirectory, int width, int height);
    public static native void render();
    public static native void touchEvent(int action, float x, float y);
    public static native boolean isTouchOverUi(float x, float y);
    public static native boolean isMapReady();
    public static native boolean isTextInputActive();
    public static native boolean isTextInputSecret();
    public static native boolean isLoginTextInputFocused();
    public static native boolean consumeKeyboardRequest();
    public static native void textInput(String value);
    public static native void textKey(int key);
    public static native void resumeFromBackground(long elapsedMilliseconds);
}
