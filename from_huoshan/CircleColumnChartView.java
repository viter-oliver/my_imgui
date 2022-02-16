package com.huoshan.playerdj;

import static com.google.common.primitives.Ints.min;

import static java.lang.Math.PI;
import static java.lang.Math.cos;
import static java.lang.Math.sin;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.graphics.RectF;
import android.view.View;

public class CircleColumnChartView extends View {
    private float mRadius=20.F;
    private float mMaxColumnHeight;
    private boolean mClockwise=true;
    private RectF[] mColumns;
    private Paint mPaint =new Paint();
    private void calculateMaxColumnHeight(){
        int minSz=min(getHeight(),getWidth());
        float minR=minSz*0.5F;

        mMaxColumnHeight=minR-mRadius;
    }
    public CircleColumnChartView(Context context) {
        super(context);
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
        for (int ix=0;ix<mColumns.length;++ix){
            path.addRect(mColumns[ix],Path.Direction.CW);

        }
        canvas.drawPath(path, mPaint);

    }
    public void updateColumnValues(float[] values){
        mColumns=new RectF[values.length];
        double unit_angle=2*PI/(double) values.length;
        double inclination0=0.f;
        for (int ix = 0; ix < values.length; ++ix) {
            float rbx= (float) (mRadius*cos(inclination0));
            float rby=(float) (mRadius*sin(inclination0));

            double inclination=inclination0+unit_angle;
            float lbx=(float) (mRadius*cos(inclination));
            float lby=(float) (mRadius*sin(inclination));

            inclination0=inclination;
        }
    }
}
