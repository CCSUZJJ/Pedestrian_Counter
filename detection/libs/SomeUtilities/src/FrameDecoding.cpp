#include "CodecDecoder.h"
#include "FrameDecoding.h"

using namespace vipnt;

//FrameCounter should be 3 times in the frame: top middle and bottom
vipnt::TRAF_UINT32 GetFrameCounter(const unsigned char * pY, vipnt::TRAF_UINT32 LineSize, vipnt::TRAF_UINT32 /* Height*/)
{
    CCodecDecoder::SSettings settings;
    settings.s_ImageWidth = LineSize;
    CCodecDecoder codec;
    codec.Configure(settings);
    codec.ReadSample(pY);
    return codec.GetFrameCntr();
}
