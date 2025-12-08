python ../../../tool/mkromfs/mkromfs_for_honeygui.py -i root -o root_0x00950000.bin -b -a 0x00950000

srec_cat root_0x00950000.bin -binary -offset 0x00950000 -o root_0x00950000.hex -intel
