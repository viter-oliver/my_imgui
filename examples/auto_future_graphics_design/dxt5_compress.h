#pragma once
#include "stb_dxt.h"
void ExtractBlock(const uint32_t* inPtr, uint32_t width, uint8_t* colorBlock)
{
	for (int j = 0; j < 4; j++)
	{
		memcpy(&colorBlock[j * 4 * 4], inPtr, 4 * 4);
		inPtr += width;
	}
}
bool compress_image_2_dxt5(uint8_t* outbuff, uint8_t* imgbuff, int width, int height)
{
	uint8_t block[64];

	const uint32_t kBlockSz = 16;
	const uint32_t startBlock = 0;
	

	const uint32_t *inPixels = reinterpret_cast<const uint32_t *>(imgbuff);
	uint32_t endY = height - 4;
	uint32_t startX = 0;
	for (uint32_t j = 0; j <= endY; j += 4) {
	const uint32_t endX = width;
	for (uint32_t i = startX; i < endX; i += 4) {

		const uint32_t kOffset = j*width + i;
			ExtractBlock(inPixels + kOffset, width, block);
			stb_compress_dxt_block(outbuff, block, 1, STB_DXT_DITHER);
			outbuff += 16;
		}
		startX = 0;
	}
	return true;
}