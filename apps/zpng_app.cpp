#include "../zpng.h"

#include <iostream>
#include <fstream>
#include <vector>
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



int main(int argc, char** argv)
{
    bool compress = false;
    bool decompress = false;
    const char* sourceFile = nullptr;
    const char* destFile = nullptr;

    if (argc >= 4)
    {
        if (0 == strcmp(argv[1], "-c")) {
            compress = true;
        }
        else if (0 == strcmp(argv[1], "-d")) {
            decompress = true;
        }
        sourceFile = argv[2];
        destFile = argv[3];
    }

    if (compress)
    {
        cout << "Compressing " << sourceFile << " to " << destFile << endl;

        uint64_t t0 = GetTimeUsec();

        int x, y, comp;
        stbi_uc* data = stbi_load(sourceFile, &x, &y, &comp, 0);
        if (!data)
        {
            cout << "Unable to load file: " << sourceFile << endl;
            return -1;
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

        cout << "Compressed ZPNG in " << (t1 - t0) / 1000.f << " msec" << endl;

        cout << "ZPNG compression size: " << buffer.Bytes << " bytes" << endl;

        std::ofstream output(destFile, std::ios::binary);
        if (!output)
        {
            cout << "Could not open output file" << endl;
            return -3;
        }
        output.write((char*)buffer.Data, buffer.Bytes);
        output.close();

        stbi_image_free(data);

        ZPNG_Free(&buffer);
    }
    else if (decompress)
    {
        cout << "Decompressing " << sourceFile << " to " << destFile << " (output will be PNG format)" << endl;

        std::ifstream input(sourceFile, std::ios::binary);
        if (!input)
        {
            cout << "Could not open input file" << endl;
            return -4;
        }

        // copies all data into buffer
        std::vector<char> fileData((
            std::istreambuf_iterator<char>(input)),
            (std::istreambuf_iterator<char>()));

        ZPNG_Buffer buffer;
        buffer.Data = (uint8_t*)&fileData[0];
        buffer.Bytes = (unsigned)fileData.size();

        uint64_t t0 = GetTimeUsec();

        ZPNG_ImageData decompressResult = ZPNG_Decompress(buffer);

        uint64_t t1 = GetTimeUsec();

        if (!decompressResult.Buffer.Data)
        {
            cout << "Decompression failed" << endl;
            return -5;
        }

        cout << "Decompressed ZPNG in " << (t1 - t0) / 1000.f << " msec" << endl;

        t0 = GetTimeUsec();

        int writeResult = stbi_write_png(
            destFile,
            decompressResult.WidthPixels,
            decompressResult.HeightPixels,
            decompressResult.Channels,
            decompressResult.Buffer.Data,
            decompressResult.StrideBytes);

        t1 = GetTimeUsec();

        if (!writeResult)
        {
            cout << "Failed to compress PNG" << endl;
            return -6;
        }

        cout << "Compressed PNG in " << (t1 - t0) / 1000.f << " msec" << endl;

        cout << "Wrote decompressed PNG file: " << destFile << endl;

        ZPNG_Free(&decompressResult.Buffer);
    }
    else
    {
        cout << "Usage: zpng -c Input.PNG Test.ZPNG" << endl;
        cout << "Usage: zpng -d Test.ZPNG Output.PNG" << endl;
    }

    return 0;
}
