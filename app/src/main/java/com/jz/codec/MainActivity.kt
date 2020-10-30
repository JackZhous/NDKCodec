package com.jz.codec

import android.graphics.Matrix
import android.graphics.SurfaceTexture
import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.os.Bundle
import android.util.Log
import android.util.Size
import android.view.Gravity
import android.view.Surface
import android.view.TextureView
import android.view.WindowManager
import android.widget.FrameLayout
import androidx.appcompat.app.AppCompatActivity
import java.util.jar.Manifest

class MainActivity : AppCompatActivity(), TextureView.SurfaceTextureListener {

    private val textureView : TextureView by lazy {
        findViewById<TextureView>(R.id.surface)
    }
    lateinit var cameraPreviewSize : Size
    private var surface : Surface? = null

    private var ndkCamera : Long = 0
    private var ndkEncoder : Long = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.i("j_tag", "main " + Thread.currentThread().toString())
        setContentView(R.layout.activity_main)
        getPermisson()
        window.addFlags(WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED)
        initTextureView()

    }

    private fun initTextureView(){
        textureView.surfaceTextureListener = this
    }

    override fun onSurfaceTextureSizeChanged(p0: SurfaceTexture?, p1: Int, p2: Int) {
        Log.i("j_tag", "onSurfaceTextureSizeChanged");
    }

    override fun onSurfaceTextureUpdated(p0: SurfaceTexture?) {
        Log.i("j_tag", "onSurfaceTextureUpdated");
        MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface
    }

    override fun onSurfaceTextureDestroyed(p0: SurfaceTexture?): Boolean {
        Log.i("j_tag", "onSurfaceTextureDestroyed");
        surface?.let {
            onPreviewSurfaceDestroyed(ndkCamera, it)
            deleteCamera(ndkCamera)
            it.release()
        }
        ndkCamera = 0
        surface = null
        return true
    }

    override fun onSurfaceTextureAvailable(st: SurfaceTexture?, w: Int, h: Int) {
        Log.i("j_tag", "onSurfaceTextureAvailable " + Thread.currentThread().toString())
        st?.apply {
            val disW = resources.displayMetrics.widthPixels
            val disH = resources.displayMetrics.heightPixels
            ndkCamera = createNDKCamera(720, 1280)
            cameraPreviewSize = getPreviewSize(ndkCamera)
//            ndkEncoder = createEncoder(cameraPreviewSize.width, cameraPreviewSize.height)
            resetPreviewViewSize(w, h)
            st.setDefaultBufferSize(cameraPreviewSize.width, cameraPreviewSize.height)
            Surface(st).apply {
                surface = this
                onPreviewSurfaceCreated(ndkCamera, this)
            }
        }
    }

    private fun resetPreviewViewSize(w : Int, h : Int){
        val newHeight = when(windowManager.defaultDisplay.rotation){
            90,270 -> w * cameraPreviewSize.height / cameraPreviewSize.width
            else -> w * cameraPreviewSize.width / cameraPreviewSize.height
        }
        textureView.layoutParams = FrameLayout.LayoutParams(w, newHeight, Gravity.CENTER)

        configuretransform(w, newHeight)
    }

    private fun configuretransform(width : Int, height : Int){
        val mDisplayOrientation = windowManager.defaultDisplay.rotation * 90
        val matrix = Matrix()
        if (mDisplayOrientation % 180 == 90) {
            //final int width = getWidth();
            //final int height = getHeight();
            // Rotate the camera preview when the screen is landscape.
            matrix.setPolyToPoly(
                floatArrayOf(
                    0f, 0f,  // top left
                    width.toFloat(), 0f,  // top right
                    0f, height.toFloat(),  // bottom left
                    width.toFloat(), height.toFloat()
                ), 0,
                if (mDisplayOrientation == 90) floatArrayOf(
                    0f, height.toFloat(),  // top left
                    0f, 0f,  // top right
                    width.toFloat(), height.toFloat(),  // bottom left
                    width.toFloat(), 0f
                ) else floatArrayOf(
                    width.toFloat(), 0f,  // top left
                    width.toFloat(), height.toFloat(),  // top right
                    0f, 0f,  // bottom left
                    0f, height.toFloat()
                ), 0,
                4
            )
        } else if (mDisplayOrientation == 180) {
            matrix.postRotate(180f, width / 2.toFloat(), height / 2.toFloat())
        }
        textureView.setTransform(matrix)
    }

    private fun getPermisson(){
        requestPermissions(arrayOf(android.Manifest.permission.CAMERA), 1);
    }


    external fun createNDKCamera(w : Int, h : Int) : Long

    external fun getPreviewSize(pointer : Long) : Size

    external fun onPreviewSurfaceCreated(pointer : Long, surface: Surface)

    external fun onPreviewSurfaceDestroyed(pointer : Long, surface: Surface)

    external fun  deleteCamera(pointer : Long)

    external fun createEncoder(w : Int, h : Int) : Long

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
