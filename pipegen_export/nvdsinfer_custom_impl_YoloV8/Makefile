CUDA_VER ?= 12.6
CC := g++
CFLAGS := -Wall -std=c++17 -shared -fPIC -Wno-error=deprecated-declarations
CFLAGS += -I/opt/nvidia/deepstream/deepstream/sources/includes -I/usr/local/cuda-$(CUDA_VER)/include

LIBS := -Wl,--start-group -lnvdsgst_meta -lnvds_meta -lnvds_infer -lnvbufsurface -lnvbufsurftransform -ldl -lpthread -Wl,--end-group
LIBS += -L/opt/nvidia/deepstream/deepstream/lib -L/usr/local/cuda-$(CUDA_VER)/lib64 -lcudart

TARGET_LIB := libnvdsinfer_custom_impl_YoloV8.so
SRCS := nvdsparsebbox_yolov8.cpp

all: $(TARGET_LIB)

$(TARGET_LIB): $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET_LIB)
