python ../../../tool/mkromfs/mkromfs_for_honeygui.py --binary --addr 0x00950000 root root_0x00950000.bin

srec_cat root_0x00950000.bin -binary -offset 0x00950000 -o root_0x00950000.hex -intel
