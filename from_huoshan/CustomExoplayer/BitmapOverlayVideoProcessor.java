/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.huoshan.playerdj.CustomExoplayer;

import static com.google.android.exoplayer2.util.Assertions.checkNotNull;
import static com.google.common.primitives.Ints.min;

import static java.lang.Math.PI;
import static java.lang.Math.cos;
import static java.lang.Math.sin;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import androidx.annotation.Nullable;
import com.google.android.exoplayer2.C;
import com.google.android.exoplayer2.util.GlUtil;
import com.huoshan.playerdj.CustomView.CircleColumnChartView;
import com.huoshan.playerdj.R;
import java.io.IOException;
import java.util.Locale;
import java.util.concurrent.atomic.AtomicInteger;

import javax.microedition.khronos.opengles.GL10;
//import org.checkerframework.checker.nullness.qual.MonotonicNonNull;

/**
 * Video processor that demonstrates how to overlay a bitmap on video output using a GL shader. The
 * bitmap is drawn using an Android {@link Canvas}.
 */
/* package */ public final class BitmapOverlayVideoProcessor
    implements VideoProcessingGLSurfaceView.VideoProcessor {
  private static final int OVERLAY_HEIGHT = 512;
  private static final int OVERLAY_WIDTH = OVERLAY_HEIGHT*4;

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
  private LineSegment[][] mLineSegments=new LineSegment[2][mCols];
  private AtomicInteger mFront=new AtomicInteger();
  private float mRadius=OVERLAY_HEIGHT*0.25F;
  private float mMaxColumnHeight=mRadius;
  private float cxL=OVERLAY_HEIGHT*0.5F,cyL=cxL;
  private float cxR=OVERLAY_WIDTH-cxL,cyR=cyL;

  private final Context context;
  private final Paint paint;
  private final int[] textures;
  private final Bitmap overlayBitmap;
  private final Bitmap logoBitmap;
  private final Canvas overlayCanvas;

  private GlUtil.Program program;
  @Nullable private GlUtil.Attribute[] attributes;
  @Nullable private GlUtil.Uniform[] uniforms;

  private float bitmapScaleX;
  private float bitmapScaleY;

  public BitmapOverlayVideoProcessor(Context context) {
    this.context = context.getApplicationContext();
    paint = new Paint();
    paint.setTextSize(64);
    paint.setAntiAlias(true);
    paint.setARGB(0xFF, 0xFF, 0xFF, 0xFF);
    textures = new int[1];
    overlayBitmap = Bitmap.createBitmap(OVERLAY_WIDTH, OVERLAY_HEIGHT, Bitmap.Config.ARGB_8888);
    //overlayBitmap = BitmapFactory.decodeFile("/sdcard/123.png");
    overlayCanvas = new Canvas(overlayBitmap);
    logoBitmap=BitmapFactory.decodeResource(this.context.getResources(), R.drawable.music_b);
    mLineSegments[0]=new LineSegment[mCols];
    for (int ix=0;ix<mLineSegments.length;++ix){
      mLineSegments[0][ix]=new LineSegment();
    }
    mLineSegments[1]=new LineSegment[mCols];
    for (int ix=0;ix<mLineSegments.length;++ix){
      mLineSegments[0][ix]=new LineSegment();
    }
  }
  public void handleFloatArrayValues(float[] values, float maxValue) {
    double inclination0=0.f;
    int vCnt=min(values.length,mCols);
    int curId=mFront.get();
    curId++;
    if (curId>1)
      curId=0;
    for (int ix = 0; ix < vCnt; ++ix) {
      float bx= (float) (mRadius*cos(inclination0));
      float by=(float) (mRadius*sin(inclination0));
      float drawHeight=values[ix]*mMaxColumnHeight/maxValue;
      float drawRadius=mRadius+drawHeight;
      float tx=(float) (drawRadius*cos(inclination0));
      float ty=(float) (drawRadius*sin(inclination0));
      mLineSegments[curId][ix].setValue(bx,by,tx,ty);
      inclination0+=mUnitAngle;
    }
    mFront.set(curId);
  }
  private void drawCircleColumns(Canvas canvas){
    Paint paint=new Paint();
    paint.setStyle(Paint.Style.FILL_AND_STROKE);
    paint.setColor(Color.BLACK);
    paint.setAntiAlias(true);
    //2 circles
    canvas.drawCircle(cxL,cyL,mRadius,paint);
    canvas.drawCircle(cxR,cyR,mRadius,paint);
    float pw=(float) PI*mRadius*2/(float) mCols;
    paint.setStrokeWidth(pw);
    //2 circleColumns
    Path path=new Path();
    paint.setColor(Color.RED);
    int curId=mFront.get();
    for (int ix=0;ix<mLineSegments.length;++ix){
      path.moveTo(mLineSegments[curId][ix].mStartX+cxL,mLineSegments[curId][ix].mStartY+cyL);
      path.lineTo(mLineSegments[curId][ix].mEndX+cxL,mLineSegments[curId][ix].mEndY+cyL);
    }
    canvas.drawPath(path, paint);
    path.reset();
    paint.setColor(Color.BLUE);
    for (int ix=0;ix<mLineSegments.length;++ix){
      path.moveTo(mLineSegments[curId][ix].mStartX+cxR,mLineSegments[curId][ix].mStartY+cyR);
      path.lineTo(mLineSegments[curId][ix].mEndX+cxR,mLineSegments[curId][ix].mEndY+cyR);
    }
    canvas.drawPath(path, paint);

  }
  @Override
  public void initialize() {
    try {
      program =
          new GlUtil.Program(
              context,
              /* vertexShaderFilePath= */ "bitmap_overlay_video_processor_vertex.glsl",
              /* fragmentShaderFilePath= */ "bitmap_overlay_video_processor_fragment.glsl");
    } catch (IOException e) {
      throw new IllegalStateException(e);
    }
    program.use();
    GlUtil.Attribute[] attributes = program.getAttributes();
    for (GlUtil.Attribute attribute : attributes) {
      if (attribute.name.equals("a_position")) {
        attribute.setBuffer(
            new float[] {
              -1, -1, 0, 1,
              1, -1, 0, 1,
              -1, 1, 0, 1,
              1, 1, 0, 1
            },
            4);
      } else if (attribute.name.equals("a_texcoord")) {
        attribute.setBuffer(
            new float[] {
              0, 0, 0, 1,
              1, 0, 0, 1,
              0, 1, 0, 1,
              1, 1, 0, 1
            },
            4);
      }
    }
    this.attributes = attributes;
    this.uniforms = program.getUniforms();
    GLES20.glGenTextures(1, textures, 0);
    GLES20.glBindTexture(GL10.GL_TEXTURE_2D, textures[0]);
    GLES20.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
    GLES20.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
    GLES20.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_REPEAT);
    GLES20.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_REPEAT);
    GLUtils.texImage2D(GL10.GL_TEXTURE_2D, /* level= */ 0, overlayBitmap, /* border= */ 0);
  }

  @Override
  public void setSurfaceSize(int width, int height) {
    bitmapScaleX = (float) width / OVERLAY_WIDTH;
    bitmapScaleY = (float) height / OVERLAY_HEIGHT;
  }

  @Override
  public void draw(int frameTexture, long frameTimestampUs, float[] transformMatrix) {
    // Draw to the canvas and store it in a texture.
    String text = String.format(Locale.US, "%.02f", frameTimestampUs / (float) C.MICROS_PER_SECOND);
    overlayBitmap.eraseColor(Color.TRANSPARENT);
    //overlayCanvas.drawBitmap(logoBitmap, /* left= */ 32, /* top= */ 32, paint);
    //overlayCanvas.drawText(text, /* x= */ 200, /* y= */ 130, paint);
    drawCircleColumns(overlayCanvas);
    GLES20.glBindTexture(GL10.GL_TEXTURE_2D, textures[0]);
    GLUtils.texSubImage2D(
        GL10.GL_TEXTURE_2D, /* level= */ 0, /* xoffset= */ 0, /* yoffset= */ 0, overlayBitmap);
    GlUtil.checkGlError();

    // Run the shader program.
    GlUtil.Uniform[] uniforms = checkNotNull(this.uniforms);
    GlUtil.Attribute[] attributes = checkNotNull(this.attributes);
    for (GlUtil.Uniform uniform : uniforms) {
      switch (uniform.name) {
        case "tex_sampler_0":
          uniform.setSamplerTexId(frameTexture, /* unit= */ 0);
          break;
        case "tex_sampler_1":
          uniform.setSamplerTexId(textures[0], /* unit= */ 1);
          break;
        case "scaleX":
          uniform.setFloat(bitmapScaleX);
          break;
        case "scaleY":
          uniform.setFloat(bitmapScaleY);
          break;
        case "tex_transform":
          uniform.setFloats(transformMatrix);
          break;
        default: // fall out
      }
    }
    for (GlUtil.Attribute copyExternalAttribute : attributes) {
      copyExternalAttribute.bind();
    }
    for (GlUtil.Uniform copyExternalUniform : uniforms) {
      copyExternalUniform.bind();
    }
    GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
    GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, /* first= */ 0, /* count= */ 4);
    GlUtil.checkGlError();
  }
}
