package com.example.uebung3;

import android.content.res.AssetManager;

public class GLLib {
    static {
        System.loadLibrary("gles3jni");
    }

    public static native void init(AssetManager sAssetManager);
    public static native void resize(int width, int height);
    public static native void step(int x1, int y1, int x2, int y2);
}
