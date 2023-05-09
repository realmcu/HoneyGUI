include ../Makefile.def

OUT = ../bin
NAME = libvg_lite.so

SOURCES = vg_lite_dump.c vg_lite.c vg_lite_path.c vg_lite_stroke.c vg_lite_matrix.c vg_lite_image.c ./linux/vg_lite_os.c ./linux/vg_lite_ioctl.c

CFLAGS += -I../inc -I../VGLiteKernel -I. -I./linux
CFLAGS += -D_GNU_SOURCE -lrt -Wno-missing-braces  -Wno-error=unused-function

LIBS += m
LDFLAGS += $(addprefix -l,$(LIBS))
LDFLAGS += -fPIC -shared -Wl,-z,defs
LDFLAGS += -Wl,--version-script=vglite.map
LDFLAGS += -lpthread

all : $(OUT)/$(NAME)

clean:
	@rm -f $(OUT)/$(NAME)

$(OUT):
	@mkdir $(OUT)

$(OUT)/$(NAME) : $(OUT) $(SOURCES)
	@echo $(CC) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS)
	@$(CC) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS)
