BIN=./ANYKA37E.BIN
LIB_LEO=./lib/leo/libleo_37e.a
CROSS=/opt/arm-anykav500-linux-uclibcgnueabi/bin/arm-anykav500-linux-uclibcgnueabi-
CC = $(CROSS)gcc  
CPP = $(CROSS)g++  
LD = $(CROSS)ld
AR = $(CROSS)ar
#这里只加入库头文件路径及库路径  
INCS = -I/opt/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include
LIBS = -L/opt/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/lib
LIBS +=-L./lib/anyka
LIBS +=-L./lib/leo
LIBS +=-L./lib/png
LIBS +=-L./lib/ts
LIBS +=-L./lib/freetype
LIBS +=-L./lib/libmad
LIBS +=-L./lib/jpeg
LIBS +=-L./lib/live555
LIBS +=-L./lib/tuya

SUBDIRS =  
#生成依赖信息时的搜索目录，比如到下列目录中搜索一个依赖文件(比如.h文件)，例如 -I"./***/" -I"./base/"  
DEFINED = -I./include
DEFINED += -I./include/anyka
DEFINED += -I./include/api
DEFINED += -I./include/jpeg
DEFINED += -I./include/ts
DEFINED += -I./system/layout
DEFINED += -I./system/src
DEFINED += -I./system/ui
DEFINED += -I./include/libmad
DEFINED += -I./include/freetype2
DEFINED += -I./include/png
DEFINED += -I./include/tuya
#DEFINED += -I./include/ffmpeg

DEFINED += -I./include/live555/BasicUsageEnvironment/include/
DEFINED += -I./include/live555/groupsock/include/
DEFINED += -I./include/live555/UsageEnvironment/include/
DEFINED += -I./include/live555/liveMedia/include/
# += -I./include/libavcodec/
# += -I./include/libavdevice/
# += -I./include/libavfilter/
# += -I./include/libavformat/
# += -I./include/libavutil/
# += -I./include/libpostproc/
# += -I./include/libswresample/
# += -I./include/libswscale/
DEFINED += -I./video_drv_lib
DEFINED += -I./video_pro_lib
DEFINED += -I./queue_lib
DEFINED += -I./audio_input_lib
DEFINED += -I./thumb_lib
DEFINED += -I./network_lib
DEFINED += -I./audio_output_lib
DEFINED += -I./ui_lib
DEFINED += -I./tuya_lib

#给INCS加上依赖搜索路径，分开写可能会产生不一致情况，而且繁琐  
INCS += $(DEFINED)
#  
#maintest.c tree/rbtree.c  多了子目录，那就直接添加 目录/*.c即可   所有的源文件--  .c文件列表  
CSRCS =$(wildcard ./system/layout/*.c ./system/src/*.c)
#$(wildcard ./*.c ./api/*.c ./drive/*.c ./drive/sys/*.c ./drive/live555/*.c ./layout/*.c ./src/*.c ./drive/flv_enc/*.c)  
CPPSRCS =$(wildcard ./system/layout/*.cpp ./system/src/*.cpp)

CSRC_LIB_S  =$(wildcard ./audio_input_lib/*.c ./audio_output_lib/*.c  ./gpio_lib/*.c)
CSRC_LIB_S +=$(wildcard ./audio_play_lib/*.c ./file_lib/*.c  ./video_pro_lib/*.c ./i2c_lib/*.c)
CSRC_LIB_S +=$(wildcard ./jpeg_enc_lib/*.c ./os_lib/*.c ./queue_lib/*.c ./thumb_lib/*.c ./ui_lib/*.c)
CSRC_LIB_S +=$(wildcard ./video_drv_lib/*.c ./video_sensor_lib/*.c ./network_lib/*.c ./tuya_lib/*.c)

#CSRC_LIB_S +=$(wildcard ./video_drv_lib/libavcodec/*.c ./video_drv_lib/libavformat/*.c ./video_drv_lib/libavutil/*.c)

CPPSRC_LIB_S += $(wildcard ./network_lib/*.cpp)
#  
#所有的.o文件列表  
COBJS := $(CSRCS:.c=.o)  
CPPOBJS := $(CPPSRCS:.cpp=.o)

COBJS_LIB :=$(CSRC_LIB_S:.c=.o)
CPPOBJS_LIB :=$(CPPSRC_LIB_S:.cpp=.o)
#  
#生成依赖信息 -MM是只生成自己的头文件信息，-M 包含了标准库头文件信息。  
#-MT 或 -MQ都可以改变生成的依赖  xxx.o:src/xxx.h 为 src/xxx.o:src/xxx.h 当然。前面的 src/xxx.o需自己指定  
#格式为 -MM 输入.c或.cpp  查找依赖路径  -MT或-MQ  生成规则，比如src/xxx.o  
#MAKEDEPEND = gcc -MM -MT  
CFLAGS += $(INCS)  
CFLAGS += -O2 -Wall -fPIE  -std=gnu11 -fexec-charset=utf-8
# -g
CFLAGS_LIB +=$(INCS)
CFLAGS_LIB += -O2 -Wall -fPIE  -std=gnu11 -fexec-charset=utf-8
# -g

CPPFLAGS_LIB += $(INCS)  
CPPFLAGS_LIB += -Wall -fPIC -std=c++1y -fexec-charset=utf-8
# -g  -O2
 # -std=gnu11 
CPPFLAGS += $(INCS)  
CPPFLAGS +=  -Wall -fPIC -std=c++1y -fexec-charset=utf-8
# -g -O2

LDFLAGS +=  -fPIE -lpthread -lm -lrt -lstdc++ -ldl -Wl,--gc-sections


LDFLAGS				+= -Wl,--start-group -lleo_37e -ltuya_ipc -lmbedtls -Wl,--end-group

#LDFLAGS				+= -ltuya_ipc
#LDFLAGS				+= -lmbedtls

LDFLAGS				+= -Wl,--start-group -lliveMedia -lBasicUsageEnvironment -lgroupsock -lUsageEnvironment -Wl,--end-group


#LDFLAGS				+= -lavformat
#LDFLAGS				+= -lavcodec
#LDFLAGS				+= -lavutil

LDFLAGS				+= -lakaudiofilter
LDFLAGS				+= -lplat_vqe
LDFLAGS				+= -lplat_ao
LDFLAGS				+= -lplat_thread
LDFLAGS				+= -lplat_log
LDFLAGS				+= -lplat_common
LDFLAGS				+= -lplat_mem
LDFLAGS				+= -lplat_dbg

LDFLAGS				+= -lplat_tde
#LDFLAGS				+= -lplat_vo
LDFLAGS				+= -lplat_osal
LDFLAGS				+= -lakv_decode
LDFLAGS				+= -lmpi_vdec
LDFLAGS				+= -lakv_encode
LDFLAGS				+= -lmpi_venc
LDFLAGS				+= -lplat_ai
LDFLAGS				+= -lplat_vi

LDFLAGS				+= -lpng16
LDFLAGS				+= -ljpeg
#LDFLAGS				+= -lz
LDFLAGS				+= -lfreetype
LDFLAGS				+= -lts
#LDFLAGS				+= -lmpi_mux
#LDFLAGS				+= -lmpi_demux
#LDFLAGS				+= -lakmedia
LDFLAGS				+= -lmad
#LDFLAGS				+= -liniparser

LDFLAGS				+= -lapp_ats
LDFLAGS				+= -lplat_timer
LDFLAGS				+= -lmpi_aenc
LDFLAGS				+= -lmpi_adec
LDFLAGS				+= -lakaudiocodec


all:$(BIN)
#$(OBJS):%.o :%.c  先用$(OBJS)中的一项，比如foo.o: %.o : %.c  含义为:试着用%.o匹配foo.o。如果成功%就等于foo。如果不成功，  
# Make就会警告，然后。给foo.o添加依赖文件foo.c(用foo替换了%.c里的%)  
# 也可以不要下面的这个生成规则，因为下面的 include $(DEF)  就隐含了。此处为了明了，易懂。故留着
$(COBJS) : %.o: %.c  
	$(CC)   $(CFLAGS)   -c $<   -o  $@
$(CPPOBJS) : %.o : %.cpp  
	$(CPP) $(CPPFLAGS) -c $< -o $@   

# $@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。每次$< $@ 代表的值就是列表中的  
#
$(BIN) : $(COBJS) $(CPPOBJS)
	$(CC) -o $(BIN) $(COBJS) $(CPPOBJS) $(LDFLAGS) $(LIBS)
	rm -rf $(COBJS)
	rm -rf $(CPPOBJS)
	cp ./system/ui/rom.bin ./
	# cp ./ANYKA37E.BIN ../AK37E_SDK_V1.03_wifi/rootfs/rootfs/etc/config/
	# cp ./rom.bin ../AK37E_SDK_V1.03_wifi/rootfs/rootfs/etc/config/
	# tar -zcvf ANYKA37EOS ANYKA37E.BIN rom.bin
	tar -czvf KOCOM_T704I.update ANYKA37E.BIN rom.bin
	# cp ./KOCOM_T704I.update ../../AK37E_SDK_V1.03_wifi/rootfs/rootfs/etc/config/
# cp ./ANYKA37EOS ./
#	cp ./ANYKA37E.BIN /mnt/d/NFS
#	cp ./system/ui/rom.bin  /mnt/d/NFS
# 链接为最终目标

lib:$(LIB_LEO)

$(COBJS_LIB) : %.o: %.c
	$(CC)   $(CFLAGS_LIB)   -c $<   -o  $@
$(CPPOBJS_LIB) : %.o : %.cpp
	$(CPP) $(CPPFLAGS_LIB) -c  $< -o $@

$(LIB_LEO) : $(COBJS_LIB) $(CPPOBJS_LIB)
	$(AR)  cr  $(LIB_LEO) $(COBJS_LIB) $(CPPOBJS_LIB)
	rm -rf $(COBJS_LIB)
	rm -rf $(CPPOBJS_LIB)
  
#引入了.o文件对.c和.h的依赖情况。以后.h被修改也会重新生成，可看看.d文件内容即知道为何  
#引入了依赖就相当于引入了一系列的规则，因为依赖内容例如： 目录/xxx.o:目录/xxx.c 目录/xxx.h 也相当于隐含的引入了生成规则  
#故上面不能在出现如： $(OBJS) : $(DEF)之类。切记  
.PHONY:clean cleanall  
  
#清除所有目标文件以及生成的最终目标文件  
clean:
	rm $(BIN) #$(COBJS) $(CPPOBJS)  
#rm *.d  
cleanall:
	rm $(BIN) $(COBJS) $(CPPOBJS) $(COBJS_LIB)
