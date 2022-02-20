package com.huoshan.playerdj.CustomExoplayer;

import java.nio.ByteBuffer;

public interface AudioDataFetch {
    void setAudioDataAsByteBuffer(ByteBuffer buffer, int sampleRate, int channelCount);
}
