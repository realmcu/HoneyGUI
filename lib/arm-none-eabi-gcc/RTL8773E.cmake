# Apply the flags to C and C++ compiler settings
set(CONFIG_SOC_SERIES_RTL8773E y)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-mfpu=fpv5-sp-d16 \
-mfloat-abi=hard \
-march=armv8-m.main+fp \
-w \
						-g1 \
						-Os \
						-march=armv8-m.main+fp \
					    -mfloat-abi=hard \
						-mfpu=fpv5-sp-d16 \
						-mthumb -Wall \
						-Wno-parentheses  \
						-Wno-unused-variable \
						-Wno-return-type \
						-Wno-address-of-packed-member  \
						-Wno-unknown-pragmas \
						-Wno-format-truncation \
						-Wno-format \
						-Wno-missing-braces \
						-fdata-sections \
						-ffunction-sections \
						-std=gnu99 \
						-fstack-usage \
						-fno-delete-null-pointer-checks \ ")
	
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-std=c++11 \
    -mfpu=fpv5-sp-d16 \
-mfloat-abi=hard \
-march=armv8-m.main+fp \
-w \
						-g1 \
						-Os \
						-march=armv8-m.main+fp \
					    -mfloat-abi=hard \
						-mfpu=fpv5-sp-d16 \
						-mthumb -Wall \
						-Wno-parentheses  \
						-Wno-unused-variable \
						-Wno-return-type \
						-Wno-address-of-packed-member  \
						-Wno-unknown-pragmas \
						-Wno-format-truncation \
						-Wno-format \
						-Wno-missing-braces \
						-fdata-sections \
						-ffunction-sections \
						-std=gnu99 \
						-fstack-usage \
						-fno-delete-null-pointer-checks \
   ")