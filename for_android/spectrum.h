#pragma once
struct base_vertex
{
    float x,y,z={0.f};
    float r,g,b;
};
enum en_spectrum_pos
{
    en_left_top,
    en_left_bottom,
    en_right_bottom,
    en_left_top2,
    en_right_bottom2,
    en_right_top,
    en_pos_cnt
};
struct spectrum{
    base_vertex vertexs[en_pos_cnt];
    void set_bottom_base(float y)
    {
        vertexs[en_left_bottom].y=y;
        vertexs[en_right_bottom].y=y;
        vertexs[en_right_bottom2].y=y;
    }
    void set_left_base(float x)
    {
        vertexs[en_left_top].x=x;
        vertexs[en_left_top2].x=x;
        vertexs[en_left_bottom].x=x;
    }
    void set_width(float w)
    {
        vertexs[en_right_top].x=vertexs[en_left_top].x+w;
        vertexs[en_right_bottom].x=vertexs[en_right_top].x;
        vertexs[en_right_bottom2].x=vertexs[en_right_top].x;
    }
    void set_high(float h)
    {
        vertexs[en_left_top].y=vertexs[en_left_bottom].y+h;
        vertexs[en_left_top2].y=vertexs[en_left_top].y;
        vertexs[en_right_top].y=vertexs[en_left_top].y;
    }
    void set_top_col(float r,float g,float b)
    {
        vertexs[en_left_top].r=r;
        vertexs[en_left_top].g=g;
        vertexs[en_left_top].b=b;

        vertexs[en_left_top2].r=r;
        vertexs[en_left_top2].g=g;
        vertexs[en_left_top2].b=b;

        vertexs[en_right_top].r=r;
        vertexs[en_right_top].g=g;
        vertexs[en_right_top].b=b;
    }
    void set_bottom_col(float r,float g,float b)
    {
        vertexs[en_left_bottom].r=r;
        vertexs[en_left_bottom].g=g;
        vertexs[en_left_bottom].b=b;

        vertexs[en_right_bottom2].r=r;
        vertexs[en_right_bottom2].g=g;
        vertexs[en_right_bottom2].b=b;

        vertexs[en_right_bottom].r=r;
        vertexs[en_right_bottom].g=g;
        vertexs[en_right_bottom].b=b;
    }
};
template <int cnt>
class spectrums_manger
{
    spectrum spectrums[cnt];
public:
    spectrums_manger(float base_left,float base_bottom,
                     float spectrum_w,float interval,
                     float spectrum_high=0.7f)
    {
        for (int ii = 0; ii < cnt; ++ii) {
            auto pt_left=base_left+ii*interval;
            spectrums[ii].set_left_base(pt_left);
            spectrums[ii].set_bottom_base(base_bottom);
            spectrums[ii].set_width(spectrum_w);
            spectrums[ii].set_high(spectrum_high);
            spectrums[ii].set_top_col(0.05f,0.0f,0.f);
            spectrums[ii].set_bottom_col(1.f,0.f,0.f);
        }
    }
    void set_spectrum_high(int index,float high)
    {
        spectrums[index].set_high(high);
    }
    float* get_head_address()
    {
        return reinterpret_cast<float *>(spectrums);
    }
    int get_vertex_cnt()
    {
        return cnt*6;
    }
};