# pics2bits - from images to bitmaps in C++

pics2bits is a C++ library to create bitmaps corresponding to images, reducing the needed memory to represent information in applications that do require that type of attention to detail and do not need the whole 256 possible values of a grayscale pixel.

It does not aim to be an efficiency-focused library, unless the application that uses it can benefit from such an implementation, like the one that inspired it.

pics2bits extensively uses the OpenCV library and has builtin integrations for it.