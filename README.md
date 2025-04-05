# Waveshare 2.13inch Touch e-Paper HAT for Orangpi3b

移植微雪2.13寸墨水屏到OrangePi 3B，同时添加了 `CMakeLists.txt`用于将微雪的程序打包成CMake子项目

* 微雪源代码：[https://github.com/waveshareteam/Touch_e-Paper_HAT](https://github.com/waveshareteam/Touch_e-Paper_HAT)
* 仅移植了C代码，使用liblgpio控制GPIO，Opi3b应打开设备树overlay:`overlays=i2c2-m1 spi3-m0-cs0-spidev`
* 可修改udev使 `eink`用户组有权限访问  `iic`,`spi`, `gpio`:

  ```shell
  KERNEL=="gpiochip*", GROUP="eink", MODE="0666"
  KERNEL=="spidev*", GROUP="eink", MODE="0666"
  KERNEL=="i2c-[0-9]*", GROUP="eink", MODE="0666"
  ```

* 使用时可能要连接`liblgpio`
  ```cmake
  find_library(LGPIO_LIB lgpio REQUIRED)
  target_link_libraries(${PROJECT_NAME} ${LGPIO_LIB})
  ```

* 父项目引用此库`CMakeLists.txt`举例：
  ```cmake
  cmake_minimum_required(VERSION 3.21.0)
  
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_C_STANDARD 17)
  
  project(E-Paper)
  
  add_subdirectory(lib/waveshare_eink_2in13_v4_opi3b)
  
  add_executable(${PROJECT_NAME} Program.cpp)

  target_link_libraries(${PROJECT_NAME} WaveshareDrivers)
  ```
