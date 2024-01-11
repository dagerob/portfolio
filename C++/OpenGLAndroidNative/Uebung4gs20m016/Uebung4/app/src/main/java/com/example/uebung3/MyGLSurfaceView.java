package com.example.uebung3;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MyGLSurfaceView extends GLSurfaceView {
    private static final String TAG = "MyGLSurfaceView";
    private static final boolean DEBUG = true;
    private Renderer renderer;

    public MyGLSurfaceView(Context context) {
        super(context);

        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);
        Renderer.sAssetManager = context.getAssets();
        renderer = new Renderer();
        setRenderer(renderer);
        setOnTouchListener(handleTouch);
    }

    private View.OnTouchListener handleTouch = new View.OnTouchListener() {

        @Override
        public boolean onTouch(View v, MotionEvent event) {

            // get the amount of fingers on the screen
            // according to that update the required values
            int pointerCount = event.getPointerCount();

            if(pointerCount == 1){
                renderer.x1 = (int)event.getX();
                renderer.y1 = (int)event.getY();
            }else {
                renderer.x1 = (int)event.getX(0);
                renderer.y1 = (int)event.getY(0);
                renderer.x2 = (int)event.getX(1);
                renderer.y2 = (int)event.getY(1);
            }
            // if fingers were taken off the screen reset the variables accordingly
            int action = event.getActionMasked();
            int actionIndex = event.getActionIndex();
            if(action == MotionEvent.ACTION_POINTER_UP){
                renderer.x2 = -1;
                renderer.y2 = -1;
            }else if(action == MotionEvent.ACTION_UP){
                renderer.x1 = -1;
                renderer.y1 = -1;
                renderer.x2 = -1;
                renderer.y2 = -1;
            }
            return true;
        }
    };

    private static class Renderer implements GLSurfaceView.Renderer {
        public static AssetManager sAssetManager;
        public int x1 = -1, y1 = -1, x2 = -1, y2 = -1;
        public void onDrawFrame(GL10 gl) {
            GLLib.step(x1, y1, x2, y2);
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            GLLib.resize(width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            GLLib.init(sAssetManager);
        }
    }
}
