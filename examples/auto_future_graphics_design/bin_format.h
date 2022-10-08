#include <vector>
#include "vg_type.h"
namespace vg
{
  namespace bin_format
  {
    enum texture_format
    {
      en_uncompressed_txt,
      en_dxt5,
      en_atc,
      en_etc2,
      en_pvrtc,
      en_bptc,
      en_pallet_txt,
    };
    enum program_format
    {
      en_shader_code,
      en_shader_bin_general,
      en_shader_bin_vivante,
    };
    struct output_bin_format
    {
      int _txt_fmt;
      int _pgm_fmt;
    };
    using texture_buff=std::vector<u8>;
    void get_texture_buff(int txt_id,u32 width,u32 height, texture_buff txtBuff);
    int out_texture_format();
  }
}