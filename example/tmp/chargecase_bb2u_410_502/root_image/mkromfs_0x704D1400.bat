python ..\..\..\..\tool\mkromfs\mkromfs_for_honeygui.py -i root -o root_0x704D1400.bin -b -a 0x704D1400

srec_cat root_0x00950000.bin -binary -offset 0x704D1400 -o root_0x704D1400.hex -intel
