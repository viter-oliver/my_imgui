#include "rg_etc1.h"
#include <algorithm>
#include <cstring>
bool compress_image_2_etc1(uint8_t* outbuff, uint8_t* imgbuff, int width, int height)
    rg_etc1::etc1_pack_params params;
    params.m_quality = rg_etc1::cLowQuality;
    rg_etc1::pack_etc1_block_init();

    uint32 kBlockSz = 8;
    const uint32 startBlock = 0;
   
    const uint32 endY = height-4;
    uint32 startX = 0;
    for(uint32 j = 0; j <= endY; j += 4) {
      const uint32 endX = j == 0? 0 : width;
      for(uint32 i = startX; i < endX; i += 4) {

        uint32 pixels[16];
        const uint32 *inPixels = reinterpret_cast<const uint32 *>(imgbuff);
        memcpy(pixels, inPixels + j*width + i, 4 * sizeof(uint32));
        memcpy(pixels + 4, inPixels + (j+1)*width + i, 4 * sizeof(uint32));
        memcpy(pixels + 8, inPixels + (j+2)*width + i, 4 * sizeof(uint32));
        memcpy(pixels + 12, inPixels + (j+3)*width + i, 4 * sizeof(uint32));

        pack_etc1_block(outbuff, pixels, params);
        outBuf += kBlockSz;
      }
      startX = 0;
    }
  }
