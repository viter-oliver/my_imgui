package com.huoshan.playerdj.CustomView;

import static com.google.common.primitives.Ints.min;
import static java.lang.Math.PI;
import static java.lang.Math.cos;
import static java.lang.Math.sin;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.AttributeSet;
import android.view.View;

import androidx.annotation.Nullable;

import com.huoshan.playerdj.CustomExoplayer.AudioDataReceiver;

public class CircleColumnChartView extends View implements AudioDataReceiver.AudioDataListener {


    private class LineSegment{
        float mStartX,mStartY,mEndX,mEndY;
        void setValue(float startX,float startY,float endX,float endY){
            mStartX=startX;
            mStartY=startY;
            mEndX=endX;
            mEndY=endY;
        }
    }
    private final int mCols=512;
    private double mUnitAngle=2*PI/mCols;
    private LineSegment[] mLineSegments=new LineSegment[mCols];
    private float mRadius=20.F;
    private float mMaxColumnHeight;
    private boolean mClockwise=true;
    private Paint mPaint =new Paint();
    private boolean getSize=false;
    private void calculateMaxColumnHeight(){
        int minSz=min(getHeight(),getWidth());
        mRadius=minSz*0.25F;
        mMaxColumnHeight=mRadius;
    }
    public CircleColumnChartView(Context context, @Nullable AttributeSet attrs, int defStyle) {
        super(context,attrs);

    }
    public CircleColumnChartView(Context context, @Nullable AttributeSet attrs){
        this(context, attrs, 0);
    }
    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        super.onLayout(changed, l, t, r, b);
        getSize=true;
        for (int ix=0;ix<mLineSegments.length;++ix){
            mLineSegments[ix]=new LineSegment();
        }
        calculateMaxColumnHeight();
     }

    public void setRadius(float rd){
        mRadius=rd;
        calculateMaxColumnHeight();
    }
    @Override
    protected void onDraw(Canvas canvas){
        super.onDraw(canvas);
        mPaint.setColor(Color.CYAN);
        mPaint.setStyle(Paint.Style.FILL);
        mPaint.setAntiAlias(true);
        float cx=getWidth()*0.5F;
        float cy=getHeight()*0.5F;
        canvas.drawCircle(cx,cy,mRadius, mPaint);
        Path path=new Path();

        mPaint.setColor(Color.RED);
        mPaint.setStyle(Paint.Style.FILL_AND_STROKE);
        float pW=(float) PI*mRadius*2/(float)mCols;
        mPaint.setStrokeWidth(pW);
        for (int ix=0;ix<mLineSegments.length;++ix){
            path.moveTo(mLineSegments[ix].mStartX,mLineSegments[ix].mStartY);
            path.lineTo(mLineSegments[ix].mEndX,mLineSegments[ix].mEndY);
        }
        canvas.drawPath(path, mPaint);

    }
    public void handleFloatArrayValues(float[] values, float maxValue) {

        double inclination0=0.f;
        float cx=getWidth()*0.5F;
        float cy=getHeight()*0.5F;
        int vCnt=min(values.length,mCols);
        for (int ix = 0; ix < vCnt; ++ix) {
            float bx= (float) (mRadius*cos(inclination0))+cx;
            float by=(float) (mRadius*sin(inclination0))+cy;
            float drawHeight=values[ix]*mMaxColumnHeight/maxValue;
            float drawRadius=mRadius+drawHeight;
            float tx=(float) (drawRadius*cos(inclination0))+cx;
            float ty=(float) (drawRadius*sin(inclination0))+cy;
            mLineSegments[ix].setValue(bx,by,tx,ty);
            inclination0+=mUnitAngle;
        }
    }

    @Override
    public void setRawAudioBytes(short[] shorts,short maxValue) {
        if (!getSize)
            return;
        double inclination0=0.f;
        float cx=getWidth()*0.5F;
        float cy=getHeight()*0.5F;
        int vCnt=min(shorts.length,mCols);
        for (int ix = 0; ix < vCnt; ++ix) {
            float bx= (float) (mRadius*cos(inclination0))+cx;
            float by=(float) (mRadius*sin(inclination0))+cy;
            float drawHeight=shorts[ix]*mMaxColumnHeight/maxValue;
            if (drawHeight<0)
                drawHeight=-drawHeight;
            float drawRadius=mRadius+drawHeight;
            float tx=(float) (drawRadius*cos(inclination0))+cx;
            float ty=(float) (drawRadius*sin(inclination0))+cy;
            mLineSegments[ix].setValue(bx,by,tx,ty);
            inclination0+=mUnitAngle;
        }
        postInvalidate();
    }
}
