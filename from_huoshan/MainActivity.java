package com.huoshan.playerdj;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

import androidx.annotation.Nullable;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.animation.ObjectAnimator;
import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.ConfigurationInfo;
import android.content.pm.PackageManager;
import android.graphics.PixelFormat;
import android.graphics.PointF;

import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.DragEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.LinearInterpolator;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.opengl.GLSurfaceView;

import com.google.android.exoplayer2.C;
import com.google.android.exoplayer2.DefaultRenderersFactory;
import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.MediaItem;
import com.google.android.exoplayer2.Renderer;
import com.google.android.exoplayer2.RenderersFactory;
import com.google.android.exoplayer2.audio.AudioCapabilities;
import com.google.android.exoplayer2.audio.AudioProcessor;
import com.google.android.exoplayer2.audio.AudioRendererEventListener;
import com.google.android.exoplayer2.audio.AudioSink;
import com.google.android.exoplayer2.audio.DefaultAudioSink;
import com.google.android.exoplayer2.audio.SilenceSkippingAudioProcessor;
import com.google.android.exoplayer2.audio.SonicAudioProcessor;
import com.google.android.exoplayer2.audio.TeeAudioProcessor;
import com.google.android.exoplayer2.metadata.MetadataOutput;
import com.google.android.exoplayer2.source.MediaSource;
import com.google.android.exoplayer2.text.TextOutput;
import com.google.android.exoplayer2.ui.StyledPlayerView;
import com.google.android.exoplayer2.upstream.DataSource;
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory;
import com.google.android.exoplayer2.upstream.RawResourceDataSource;
import com.google.android.exoplayer2.util.Util;
import com.google.android.exoplayer2.video.VideoRendererEventListener;

import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity{
    RendererJNI mRenderer;
    DrawVerteicesView glView_play0;
    DrawVerteicesView glView_play1;
    DrawVerteicesView glView_preview0;
    DrawVerteicesView glView_preview1;
    GLSurfaceView glView_effect;
    LinearLayout track_list;
    private String sel_track_path;
    private Boolean sel_a_track=false;
    ExoPlayer testPlayer;
    StyledPlayerView exoPlayerView;
    private SliderBar mSliderBarL = null;     // 左 拖动条
    private SliderBar mSliderBarR = null;     // 右 拖动条

    private float values1 = 0f;
    private float values2 = 0f;

    public ImageView mImgViewL;                // 左转盘
    public ImageView mImgViewR;                // 右转盘

    public ObjectAnimator mDishAnimatorL;      // 左边动画
    public ObjectAnimator mDishAnimatorR;      // 右边动画
    // 球星效果

    public static final int AUDIO_PERMISSION_REQUEST_CODE = 102;
    public static final String[] WRITE_EXTERNAL_STORAGE_PERMS = {
            Manifest.permission.RECORD_AUDIO
    };
    public static class PermisionUtils {
        private static final int REQUEST_EXTERNAL_STORAGE = 1;
        private static String[] PERMISSIONS_STORAGE = {
                Manifest.permission.RECORD_AUDIO,
                Manifest.permission.MODIFY_AUDIO_SETTINGS
        };

        public static boolean verifyStoragePermissions(Activity activity) {
            for (String node : PERMISSIONS_STORAGE) {
                int permission = ActivityCompat.checkSelfPermission(activity, node);
                if (permission != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(activity, new String[]{node}, REQUEST_EXTERNAL_STORAGE);
                    return false;
                }else{
                    System.out.println(" *********************** hav "+node+" permission");
                }
            }
            return true;
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == PermisionUtils.REQUEST_EXTERNAL_STORAGE) {
            PermisionUtils.verifyStoragePermissions(this);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //隐藏标题
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        //设置全屏
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

        // 隐藏标题栏--全屏窗口
        View decorView = getWindow().getDecorView();
        int option = View.SYSTEM_UI_FLAG_FULLSCREEN;
        decorView.setSystemUiVisibility(option);
        ActionBar actionBar = getSupportActionBar();
        actionBar.hide();

        //  左 右拖动条
        mSliderBarL = this.findViewById(R.id.sliderBarxL);
        mSliderBarL.setImagex(R.drawable.bkx,R.drawable.bgx);

        mSliderBarR = this.findViewById(R.id.sliderBarxR);
        mSliderBarR.setImagex(R.drawable.bkx,R.drawable.bgx);

        PermisionUtils.verifyStoragePermissions(this);
        initVertexViews();
        init_track_list();
        init_discs();
        init_exoplayer();
        init_play_media_item();
        ImageButton pButton = (ImageButton)findViewById(R.id.btnPlayerL);
        pButton.setOnClickListener(new View.OnClickListener()
        {
            @Override public void onClick(View arg0)
            {
                initAnimation();
                Log.i("ccb", "-----onClick-----");

            }
        });
    }
    private void init_glview_with_render(GLSurfaceView glview,RendererJNI render){
        glview.setEGLContextClientVersion(3);
        glview.getHolder().setFormat(PixelFormat.TRANSPARENT);//.TRANSLUCENT);
        // 设置与当前GLSurfaceView绑定的Renderer
        glview.setRenderer(render);
        // 设置渲染的模式
        glview.setRenderMode(RENDERMODE_WHEN_DIRTY);//mGLSurfaceView.requestRender();
    }
    private void init_exoplayer(){
        //1.Create a default TrackSelector
        exoPlayerView=this.findViewById(R.id.exoPlayerView);

        RenderersFactory renderersFactory =
                new DefaultRenderersFactory(/* context= */ this) {
                    @Nullable
                    @Override
                    protected AudioSink buildAudioSink(
                            Context context,
                            boolean enableFloatOutput,
                            boolean enableAudioTrackPlaybackParams,
                            boolean enableOffload) {
                        SonicAudioProcessor sonicAudioProcessor = new SonicAudioProcessor();
                        return new DefaultAudioSink(
                                AudioCapabilities.DEFAULT_AUDIO_CAPABILITIES,
                                new DefaultAudioSink.DefaultAudioProcessorChain(
                                        new AudioProcessor[]{
                                                new TeeAudioProcessor(new TeeAudioProcessor.AudioBufferSink() {
                                                    @Override
                                                    public void flush(int sampleRateHz, int channelCount, @C.PcmEncoding int encoding) {
                                                        Log.w("DEBUG", "PCM configuration: sampleRateHz=" + sampleRateHz + ", channelCount=" + channelCount + ", encoding=" + encoding);
                                                    }
                                                    @Override
                                                    public void handleBuffer(ByteBuffer buffer) {
                                                        Log.w("PCM","bufflen:"+buffer.remaining());
                                                        /*
                                                        StringBuilder sb = new StringBuilder();
                                                        while (buffer.hasRemaining()) {
                                                            sb.append(String.format("%02x", buffer.get()));
                                                        }
                                                        Log.w("DEBUG", "PCM data: " + sb);
                                                        */
                                                    }
                                                })
                                        },
                                        new SilenceSkippingAudioProcessor(),
                                        sonicAudioProcessor),
                                enableFloatOutput,
                                enableAudioTrackPlaybackParams,
                                enableOffload
                                        ? DefaultAudioSink.OFFLOAD_MODE_ENABLED_GAPLESS_REQUIRED
                                        : DefaultAudioSink.OFFLOAD_MODE_DISABLED);
                    }
                };

        testPlayer= new ExoPlayer.Builder(this,renderersFactory).build();
        exoPlayerView.setPlayer(testPlayer);
        //testPlayer.setAuxEffectInfo();
        //testPlayer.getAudioSessionId();
    }
    private void init_play_media_item(){
        Uri testUri=RawResourceDataSource.buildRawResourceUri(R.raw.test);
        MediaItem testItem=MediaItem.fromUri(testUri);
        testPlayer.addMediaItem(testItem);
        // Prepare the player with the source.
        testPlayer.prepare();
        testPlayer.play();
    }
    private void initVertexViews(){
        float [] pcm_test={10,5,12,13,10,5,16,20,30,10,20,40,20,20,15,40,60,30,40,20,40,60,70,20,30,40,30,20};
        glView_play0 = this.findViewById(R.id.mediaWaveTL);
        glView_play0.updateVertices(pcm_test);
        glView_play1 = this.findViewById(R.id.mediaWaveTR);
        glView_play1.updateVertices(pcm_test);
        glView_preview0 = this.findViewById(R.id.mediaWaveL);
        glView_preview0.updateVertices(pcm_test);
        glView_preview1 = this.findViewById(R.id.mediaWaveR);
        glView_preview1.updateVertices(pcm_test);

        glView_effect=this.findViewById(R.id.mediaEffect);
        mRenderer=new RendererJNI(this);
        init_glview_with_render(glView_effect,mRenderer);
    }
    private void init_discs(){
        // 左转盘
        mImgViewL = (ImageView)findViewById(R.id.mediaDishL);
        View.OnTouchListener rotate_disc=new View.OnTouchListener() {
            private boolean start_drag=false;
            private PointF start_point=new PointF();

            @Override
            public boolean onTouch(View v, MotionEvent event) {

                switch (event.getAction()){
                    case MotionEvent.ACTION_DOWN:
                        start_drag=true;
                        start_point.set(event.getX(),event.getY());
                        break;
                    case MotionEvent.ACTION_UP:
                        start_drag=false;
                    case MotionEvent.ACTION_MOVE:
                        if(start_drag){
                            PointF cur_point = new PointF();
                            cur_point.set(event.getX(),event.getY());

                            start_point=cur_point;
                        }
                        break;
                }
                return true;
            }
        };
        // 右转盘
        mImgViewR = (ImageView)findViewById(R.id.mediaDishR);
    }
    private void init_track_list(){
        //initialize track_list
        View.OnDragListener dragListener=new View.OnDragListener() {
            @Override
            public boolean onDrag(View v, DragEvent event) {
                switch (event.getAction()){
                    case DragEvent.ACTION_DRAG_STARTED:
                        /** 拖拽开始时 */

                        break;
                    case DragEvent.ACTION_DRAG_ENTERED:
                        /** 拖拽进入区域时 */
                        break;
                    case DragEvent.ACTION_DRAG_LOCATION:
                        /** 拖拽进入区域后，仍在区域内拖动时 */
                        break;
                    case DragEvent.ACTION_DRAG_EXITED:
                        /** 离开区域时 */
                        break;
                    case DragEvent.ACTION_DROP:
                        /** 在区域内放开时 */
                        if (sel_a_track){
                            String playstr=String.format("play item:%s",sel_track_path);
                            Log.i("degplay",playstr);
                            sel_a_track=false;
                        }
                        break;
                    case DragEvent.ACTION_DRAG_ENDED:
                        /** 结束时 */
                    default:
                        break;
                }
                return true;
            }
        };
        track_list=(LinearLayout)findViewById(R.id.track_list);
        View.OnLongClickListener selItem= new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                Button track_item=(Button) v;
                sel_track_path=track_item.getText().toString();
                return true;
            }
        };
        for (int ii=0;ii<10;++ii){
            Button trackItem=new Button(this);
            trackItem.setText("track"+ii);
            trackItem.setOnLongClickListener(selItem);
            track_list.addView(trackItem);
            //trackItem.setOnDragListener(dragListener);
        }

    }
    public void initAnimation(){

        // 左转盘
        mImgViewL = (ImageView)findViewById(R.id.mediaDishL);

        // 左边动画
        mDishAnimatorL = ObjectAnimator.ofFloat( mImgViewL,"rotation", 0f, 360f);
        //旋转不停顿
        mDishAnimatorL.setInterpolator(new LinearInterpolator());
        //设置动画重复次数
        mDishAnimatorL.setRepeatCount(Animation.INFINITE);
        //设置重复模式为逆向重复
        //mDishAnimatorL.setRepeatMode( RESTART);
        //旋转时长
        mDishAnimatorL.setDuration(36000);
        mDishAnimatorL.setFloatValues(values1,values2);
        values1 ++;
        values2 = values1 + 5;
        //开始旋转
        mDishAnimatorL.start();
    }
    private boolean detectOpenGLES() {
        ActivityManager am = (ActivityManager)getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        return (info.reqGlEsVersion >= 0x20000);
    }
    @Override
    protected void onResume() {
        super.onResume();

        if(getRequestedOrientation()!= ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE){
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        }
        initTunnelPlayerWorkaround();
        init();
/*
        // 左边动画
        mDishAnimatorL = ObjectAnimator.ofFloat( mSliderBarL,"rotation", 0f, 360f);
        //旋转不停顿
        mDishAnimatorL.setInterpolator(new LinearInterpolator());
        //设置动画重复次数
        mDishAnimatorL.setRepeatCount(100);
        //设置重复模式为逆向重复
        //mDishAnimatorL.setRepeatMode(ObjectAnimator.REVERSE);
        //旋转时长
        mDishAnimatorL.setDuration(36000);
        //开始旋转
        mDishAnimatorL.start();
*/
        // 右边动画
        mDishAnimatorR = ObjectAnimator.ofFloat( mImgViewR,"rotation", 0f, 360f);
        //旋转不停顿
        mDishAnimatorR.setInterpolator(new LinearInterpolator());
        //设置动画重复次数
        mDishAnimatorR.setRepeatCount(100);
        //设置重复模式为逆向重复
        //mDishAnimatorR.setRepeatMode(ObjectAnimator.REVERSE);
        //旋转时长
        mDishAnimatorR.setDuration(36000);
        //开始旋转
        mDishAnimatorR.start();
    }
    @Override
    protected void onStop() {
        super.onStop();
    }
    @Override
    protected void onPause() {
        cleanUp();
        super.onPause();
    }
    @Override
    protected void onDestroy() {
        cleanUp();
        super.onDestroy();
    }
    private void init() {
        // 左播放器
        setActionBar();
        initialize();
    }
    private void cleanUp() {

    }
    private void initTunnelPlayerWorkaround() {
        // Read "tunnel.decode" system property to determine
        // the workaround is needed
    }

    private void initialize() {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M &&
                checkSelfPermission(Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(WRITE_EXTERNAL_STORAGE_PERMS, AUDIO_PERMISSION_REQUEST_CODE);
        }
    }
    private void setActionBar() {
        if (getActionBar() != null) {
            getActionBar().setDisplayHomeAsUpEnabled(true);
        }
    }
}
