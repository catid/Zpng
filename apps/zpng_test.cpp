#include "../zpng.h"

#include <iostream>
using namespace std;

#define STB_IMAGE_IMPLEMENTATION /* compile it here */
#include "thirdparty/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "thirdparty/stb_image_write.h"



#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#elif __MACH__
    #include <mach/mach_time.h>
    #include <mach/mach.h>
    #include <mach/clock.h>

    extern mach_port_t clock_port;
#else
    #include <time.h>
    #include <sys/time.h>
#endif


//------------------------------------------------------------------------------
// Timing

#ifdef _WIN32
static double PerfFrequencyInverse = 0.;

static void InitPerfFrequencyInverse()
{
    LARGE_INTEGER freq = {};
    if (!::QueryPerformanceFrequency(&freq) || freq.QuadPart == 0)
        return;
    PerfFrequencyInverse = 1000000. / (double)freq.QuadPart;
}
#elif __MACH__
static bool m_clock_serv_init = false;
static clock_serv_t m_clock_serv = 0;

static void InitClockServ()
{
    m_clock_serv_init = true;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &m_clock_serv);
}
#endif // _WIN32

uint64_t GetTimeUsec()
{
#ifdef _WIN32
    LARGE_INTEGER timeStamp = {};
    if (!::QueryPerformanceCounter(&timeStamp))
        return 0;
    if (PerfFrequencyInverse == 0.)
        InitPerfFrequencyInverse();
    return (uint64_t)(PerfFrequencyInverse * timeStamp.QuadPart);
#elif __MACH__
    if (!m_clock_serv_init)
        InitClockServ();

    mach_timespec_t tv;
    clock_get_time(m_clock_serv, &tv);

    return 1000000 * tv.tv_sec + tv.tv_nsec / 1000;
#else
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return 1000000 * tv.tv_sec + tv.tv_usec;
#endif
}

uint64_t GetTimeMsec()
{
    return GetTimeUsec() / 1000;
}


#include <fstream>

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}


static const char* kTestImages[] = {
"images/braid/aleph.png",
"images/braid/ape.png",
"images/braid/cannon1.png",
"images/braid/cannon2.png",
"images/braid/claw.png",
"images/braid/clawpipe.png",
"images/braid/door0.png",
"images/braid/door1.png",
"images/braid/door2.png",
"images/braid/door3.png",
"images/braid/door4.png",
"images/braid/door5.png",
"images/braid/door6.png",
"images/braid/flora1.png",
"images/braid/flora2.png",
"images/braid/flora3.png",
"images/braid/flower.png",
"images/braid/gate.png",
"images/braid/greeter1.png",
"images/braid/greeter2.png",
"images/braid/greeter3.png",
"images/braid/greeter4.png",
"images/braid/key1.png",
"images/braid/key2.png",
"images/braid/killsign.png",
"images/braid/mimic1.png",
"images/braid/mimic2.png",
"images/braid/mimic3.png",
"images/braid/mimic4.png",
"images/braid/mimic5.png",
"images/braid/monster1.png",
"images/braid/monster2.png",
"images/braid/stevinus.png",
"images/braid/switch1.png",
"images/braid/switch2.png",
"images/braid/tim0.png",
"images/braid/tim1.png",
"images/braid/tim10.png",
"images/braid/tim11.png",
"images/braid/tim12.png",
"images/braid/tim2.png",
"images/braid/tim3.png",
"images/braid/tim4.png",
"images/braid/tim5.png",
"images/braid/tim6.png",
"images/braid/tim7.png",
"images/braid/tim8.png",
"images/braid/tim9.png",
"images/rgb8bit/bridge.ppm",
"images/rgb8bit/artificial.ppm",
"images/rgb8bit/big_building.ppm",
"images/rgb8bit/big_tree.ppm",
"images/rgb8bit/cathedral.ppm",
"images/rgb8bit/deer.ppm",
"images/rgb8bit/fireworks.ppm",
"images/rgb8bit/flower_foveon.ppm",
"images/rgb8bit/hdr.ppm",
"images/rgb8bit/leaves_iso_1600.ppm",
"images/rgb8bit/leaves_iso_200.ppm",
"images/rgb8bit/nightshot_iso_100.ppm",
"images/rgb8bit/nightshot_iso_1600.ppm",
"images/rgb8bit/spider_web.ppm",
"images/cake/IMG_0008.jpg",
"images/cake/IMG_0017.jpg",
"images/cake/IMG_0025.jpg",
"images/cake/IMG_0033.jpg",
"images/cake/IMG_0041.jpg",
"images/cake/IMG_0009.jpg",
"images/cake/IMG_0018.jpg",
"images/cake/IMG_0026.jpg",
"images/cake/IMG_0034.jpg",
"images/cake/IMG_0042.jpg",
"images/cake/IMG_0010.jpg",
"images/cake/IMG_0019.jpg",
"images/cake/IMG_0027.jpg",
"images/cake/IMG_0035.jpg",
"images/cake/IMG_0043.jpg",
"images/cake/IMG_0011.jpg",
"images/cake/IMG_0020.jpg",
"images/cake/IMG_0028.jpg",
"images/cake/IMG_0036.jpg",
"images/cake/IMG_0044.jpg",
"images/cake/IMG_0013.jpg",
"images/cake/IMG_0021.jpg",
"images/cake/IMG_0029.jpg",
"images/cake/IMG_0037.jpg",
"images/cake/IMG_0045.jpg",
"images/cake/IMG_0014.jpg",
"images/cake/IMG_0022.jpg",
"images/cake/IMG_0030.jpg",
"images/cake/IMG_0038.jpg",
"images/cake/IMG_0046.jpg",
"images/cake/IMG_0015.jpg",
"images/cake/IMG_0023.jpg",
"images/cake/IMG_0031.jpg",
"images/cake/IMG_0039.jpg",
"images/cake/IMG_0047.jpg",
"images/cake/IMG_0016.jpg",
"images/cake/IMG_0024.jpg",
"images/cake/IMG_0032.jpg",
"images/cake/IMG_0040.jpg",
"images/hiking/IMG_0096.jpg",
"images/hiking/IMG_0103.jpg",
"images/hiking/IMG_0109.jpg",
"images/hiking/IMG_0112.jpg",
"images/hiking/IMG_0130.jpg",
"images/hiking/IMG_0102.jpg",
"images/hiking/IMG_0105.jpg",
"images/hiking/IMG_0110.jpg",
"images/hiking/IMG_0125.jpg",
};

static const int kNumTestImages = (int)(sizeof(kTestImages) / sizeof(kTestImages[0]));


static const bool WritePngOutput = false;
static const bool TestDecompress = true;

int main(/*int argc, char** argv*/)
{
    cout << "Testing ZPNG" << endl;

    uint64_t TotalCompressTime = 0;
    uint64_t TotalDecompressTime = 0;
    uint64_t TotalCompressedSize = 0;

    for (int i = 0; i < kNumTestImages; ++i)
    {
        const char* sourceFile = kTestImages[i];

        uint64_t t0 = GetTimeUsec();

        int x, y, comp;
        stbi_uc* data = stbi_load(sourceFile, &x, &y, &comp, 0);
        if (!data)
        {
            cout << "Unable to load file: " << sourceFile << endl;
            continue;
        }

        uint64_t t1 = GetTimeUsec();

        cout << "Loaded " << sourceFile << " in " << (t1 - t0) / 1000.f << " msec" << endl;

        ZPNG_ImageData image;
        image.Buffer.Data = data;
        image.Buffer.Bytes = x * y * comp;
        image.BytesPerChannel = 1;
        image.Channels = comp;
        image.HeightPixels = y;
        image.WidthPixels = x;
        image.StrideBytes = x * image.Channels;

        t0 = GetTimeUsec();

        ZPNG_Buffer buffer = ZPNG_Compress(&image);

        if (!buffer.Data)
        {
            cout << "ZPNG compression failed" << endl;
            return -2;
        }

        t1 = GetTimeUsec();

        TotalCompressTime += t1 - t0;
        TotalCompressedSize += buffer.Bytes;

        cout << "Compressed ZPNG in " << (t1 - t0) / 1000.f << " msec" << endl;

        cout << "ZPNG compression size: " << buffer.Bytes << " bytes" << endl;

        if (TestDecompress)
        {
            t0 = GetTimeUsec();

            ZPNG_ImageData decompressResult = ZPNG_Decompress(buffer);

            t1 = GetTimeUsec();

            TotalDecompressTime += t1 - t0;

            cout << "Decompressed ZPNG in " << (t1 - t0) / 1000.f << " msec" << endl;

            if (!decompressResult.Buffer.Data ||
                decompressResult.BytesPerChannel != 1 ||
                decompressResult.Channels != comp ||
                decompressResult.HeightPixels != y ||
                decompressResult.WidthPixels != x ||
                decompressResult.StrideBytes != x * comp ||
                decompressResult.Buffer.Bytes != image.Buffer.Bytes)
            {
                cout << "Bad output" << endl;
                return -3;
            }

            if (0 != memcmp(image.Buffer.Data, decompressResult.Buffer.Data, image.Buffer.Bytes))
            {
                cout << "Data mismatch" << endl;
                return -4;
            }

            if (WritePngOutput)
            {
                t0 = GetTimeUsec();

                int writeResult = stbi_write_png(
                    "output.png",
                    decompressResult.WidthPixels,
                    decompressResult.HeightPixels,
                    decompressResult.Channels,
                    decompressResult.Buffer.Data,
                    decompressResult.StrideBytes);

                t1 = GetTimeUsec();

                cout << "Compressed PNG in " << (t1 - t0) / 1000.f << " msec" << endl;

                unsigned pngSize = filesize("output.png");

                cout << "Compressed PNG size: " << pngSize << " bytes" << endl;

                cout << "ZPNG / PNG ratio = " << buffer.Bytes / (float)pngSize << endl;

                cout << "Wrote decompressed result to output.png.  Write result = " << writeResult << endl;
            }

            ZPNG_Free(&decompressResult.Buffer);
        }

        stbi_image_free(data);

        ZPNG_Free(&buffer);
    }

    cout << "Total size = " << TotalCompressedSize << endl;
    cout << "Total compress time = " << TotalCompressTime << " usec" << endl;
    cout << "Total de-compress time = " << TotalDecompressTime << " usec" << endl;

    return 0;
}
