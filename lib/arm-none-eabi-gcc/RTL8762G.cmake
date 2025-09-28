set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O1 -march=armv8.1-m.main+dsp+mve+fp -mthumb -mfloat-abi=hard -specs=nano.specs -Wall -fdata-sections -ffunction-sections")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -std=c++11 \
    -march=armv8.1-m.main+dsp+mve+fp \
    -mthumb \
    -std=c11 \
    -mfloat-abi=hard \
    -O1 \
    -Wall -fdata-sections -ffunction-sections \
    -Wno-unused-macros")
