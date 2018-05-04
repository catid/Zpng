# Zpng
Small experimental lossless photographic image compression library with a C API and command-line interface.

It's much faster than PNG and compresses better for photographic images.
This compressor often takes less than 6% of the time of a PNG compressor and produces a file that is 66% of the size.
It was written in just 500 lines of C code thanks to Facebook's Zstd library.

The goal was to see if I could create a better lossless compressor than PNG in just one evening (a few hours) using Zstd and some past experience writing my GCIF library.  Zstd is magical.

I'm not expecting anyone else to use this, but feel free if you need some fast compression in just a few hundred lines of C code.


#### Example results

```
$ ./ZpngApp.exe -c IMG_0008.jpg IMG_0008.zpng
Compressing IMG_0008.jpg to IMG_0008.zpng
Loaded IMG_0008.jpg in 338.712 msec
Compressed ZPNG in 248.737 msec
ZPNG compression size: 27731886 bytes

$ ./ZpngApp.exe -d IMG_0008.zpng IMG_0008.png
Decompressing IMG_0008.zpng to IMG_0008.png (output will be PNG format)
Decompressed ZPNG in 123.028 msec
Compressed PNG in 4339.82 msec
Wrote decompressed PNG file: IMG_0008.png

$ ll IMG_0008.*
-rw-r--r-- 1 leon 197121 13991058 Jun 25  2017 IMG_0008.jpg
-rw-r--r-- 1 leon 197121 41897485 May  2 22:29 IMG_0008.png
-rw-r--r-- 1 leon 197121 27731886 May  2 22:29 IMG_0008.zpng
```

FLIF and other formats get better compression ratios but you will wait like 20 seconds for them to finish.

This compressor runs faster than some JPEG decoders!
This compressor takes less than 6% of the time of the PNG compressor and produces a file that is 66% of the size.


#### How it works

This library is similar to PNG in that the image is first filtered, and then submitted to a data compressor.
The filtering step is a bit simpler and faster but somehow more effective than the one used in PNG.
The data compressor used is Zstd, which makes it significantly faster than PNG to compress and decompress.

Filtering:

(1) Reversible color channel transformation.
(2) Split each color channel into a separate color plane.
(3) Subtract each color value from the one to its left.

This kind of filtering works great for large photographic images and is very fast.


#### Experimental results

I ran a few experiments to arrive at this simple codec:

Interleaving is a 1% compression win, and a 0.3% performance win: Not used.

Splitting the data into blocks of 4 at a time actually reduces compression.

```
No filtering:
Total size = 2629842851
Total compress time = 12942621 usec

Subtract only:
Total size = 1570514796
Total compress time = 16961469 usec

Color filter, followed by subtract:
Total size = 1514724952
Total compress time = 16554638 usec

Subtract, followed by color filter:
Total size = 1514724952
Total compress time = 16376380 usec
Total de-compress time = 6511436 usec
Notes: Order of applying filter does not matter but this way is faster.

Subtract, followed by color filter YUVr from JPEG2000:
Total size = 1506802640
Total compress time = 17169743 usec
Total de-compress time = 7107897 usec
Note: Only 0.5% better compression ratio in trade for performance impact.

Subtract, followed by color filter, splitting into YUV color planes:
Total size = 1486938616
Total compress time = 14514563 usec
Total de-compress time = 6596546 usec
Note: Huge improvement!  Let's call this Zpng!
```

#### Credits

Software by Christopher A. Taylor mrcatid@gmail.com

Please reach out if you need support or would like to collaborate on a project.
