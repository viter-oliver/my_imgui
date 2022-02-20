package com.huoshan.playerdj.CustomView;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;

public class DrawVerteicesView extends View {
    enum rhombus_pos{
        en_top_x,
        en_top_y,
        en_md_left_x,
        en_md_left_y,
        en_md_right_x,
        en_md_right_y,
        en_md_right2_x,
        en_md_right2_y,
        en_md_left2_x,
        en_md_left2_y,
        en_bottom_x,
        en_bottom_y
    };
    enum rhombus_col{
        en_top,
        en_md_left,
        en_md_right,
        en_md_right2,
        en_md_left2,
        en_bottom
    };
    private float base_x=0.F;
    private float unit_x=4.F;
    private float[] _vertices;
    private int[] _cols;
    private float xoff=0.F,yoff=0.F;
    private int paint_col=Color.RED,back_ground_col=Color.BLACK;
    private float[] _texs;
    private Paint _paint= new Paint();
    private Paint _paint_mask=new Paint();
    public DrawVerteicesView(Context context, @Nullable AttributeSet attrs, int defStyle){
        super(context, attrs);
        if (android.os.Build.VERSION.SDK_INT >= 11) {
            //setLayerType(View.LAYER_TYPE_SOFTWARE, _paint);
        }
    }
    public DrawVerteicesView(Context context, @Nullable AttributeSet attrs){
        this(context, attrs, 0);

    }

    public DrawVerteicesView(Context context){
        this(context, null, 0);
    }

    @Override
    protected void onDraw(Canvas canvas){
        super.onDraw(canvas);
        canvas.drawColor(back_ground_col);
        if (_vertices==null||_vertices.length==0)
            return;
        canvas.translate(xoff,yoff);
        _paint.setColor(paint_col);
        //int widgetWidth = this.getWidth();
        int widgetHeight = this.getHeight();
        int tempTop = 0;
        for(int index=0;/*index<widgetWidth &&*/ index < _vertices.length;index++){
            tempTop = (widgetHeight - (int)_vertices[index]) / 2;
            canvas.drawLine(index,tempTop,index,tempTop + _vertices[index],_paint);
        }

       /* _paint.setColor(Color.WHITE);
        _paint.setAlpha(0x80);
        _paint.setStyle(Paint.Style.FILL);
        int mask_width=this.getWidth()/2;
        canvas.drawRect(0,0,widgetHeight,mask_width,_paint);


        canvas.translate(0,getHeight()*0.5F);
        canvas.drawVertices(Canvas.VertexMode.TRIANGLES,
                _vertices.length,_vertices,0,
                null,0,_cols,0,null,
                0,0,
                _paint);*/
    }
    private void set_rhombus_pos(int index_base,float fvalue){
        _vertices[index_base+ rhombus_pos.en_md_left_x.ordinal()]=base_x;
        _vertices[index_base+ rhombus_pos.en_md_left2_x.ordinal()]=base_x;
        _vertices[index_base+ rhombus_pos.en_md_right_x.ordinal()]=base_x+unit_x;
        _vertices[index_base+ rhombus_pos.en_md_right2_x.ordinal()]=base_x+unit_x;
        _vertices[index_base+ rhombus_pos.en_top_x.ordinal()] = (float) (base_x+unit_x*0.5);
        _vertices[index_base+ rhombus_pos.en_bottom_x.ordinal()] = (float) (base_x+unit_x*0.5);
        _vertices[index_base+ rhombus_pos.en_top_y.ordinal()]=fvalue;
        _vertices[index_base+ rhombus_pos.en_bottom_y.ordinal()]=-fvalue;
        base_x+=unit_x;
    }
    private void set_peak_col(int index_base,int col){
        _cols[index_base+ rhombus_col.en_top.ordinal()]=col;
        _cols[index_base+ rhombus_col.en_bottom.ordinal()]=col;
    }
    private void set_waist_col(int index_base,int col){
        _cols[index_base+ rhombus_col.en_md_left.ordinal()]=col;
        _cols[index_base+ rhombus_col.en_md_right.ordinal()]=col;
        _cols[index_base+ rhombus_col.en_md_left2.ordinal()]=col;
        _cols[index_base+ rhombus_col.en_md_right2.ordinal()]=col;
    }
    public void set_offset(float ofx,float ofy){
        xoff=ofx;
        yoff=ofy;
    }
    public void set_paint_col(int pcol){
        paint_col=pcol;
    }
    public void set_back_ground_col(int bcol){
        back_ground_col=bcol;
    }
    public int verticesLen(){
        if (_vertices==null)
            return 0;
        else
            return _vertices.length;
    }
    public void updateVertices(float[] valuse,int msgLen){
        _vertices = new float[msgLen];
        System.arraycopy(valuse,0,_vertices,0,msgLen);

        boolean succ = false;
        if (!succ){
            return;
        }
        _vertices=new float[valuse.length*12];
        _cols=new int[valuse.length*6];
        base_x=0;


        for (int ix=0;ix<valuse.length;++ix){
            int ix_v_base=ix*12;
            set_rhombus_pos(ix_v_base,valuse[ix]);
            int ix_c_base=ix*6;
            set_peak_col(ix_c_base,0xFF080000);
            set_waist_col(ix_c_base,0xFFFF0000);
        }
    }
}
