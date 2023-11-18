# pics2bits - from images to bitmaps in C++

pics2bits is a C++ library to create bitmaps corresponding to images, reducing the needed memory to represent information in applications that do require that type of attention to detail and do not need the whole 256 possible values of a grayscale pixel.

It does not aim to be an efficiency-focused library, unless the application that uses it can benefit from such an implementation, like the one that inspired it.
pics2bits extensively uses the OpenCV library and has builtin integrations for it.

A demo binary is included to show relevant performance metrics and can be compiled with cmake (or via the provided [cmake.sh](./cmake.sh)):

```sh
mkdir -p build &&
cmake build/ &&
cmake --build ./build --config Release --target all
```

## Screenshots

![lena](./screenshots/lena.png)

![add](./screenshots/add.png)

![size_1](./screenshots/size_1.png)

![size_2](./screenshots/size_2.png)

![size_4](./screenshots/size_4.png)