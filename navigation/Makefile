NAME	= navigation

SRCS	= src/device/ardrone.c			\
	  src/device/nmeap01.c			\
	  src/device/gps.c			\
	  src/device/serial.c			\
	  src/httpd/b64.c			\
	  src/httpd/mihl.c			\
	  src/httpd/tcp_utils.c			\
	  src/http_handler.c			\
	  src/pid.c				\
	  src/planner.c				\
	  src/navigation.c

CFLAGS	= -funroll-loops -fpromote-loop-indices -O2 -Wall -DNDEBUG -fpromote-loop-indices -funroll-loops
LDFLAGS	= -lm


###############################################################################
APATH   = /home/gostai/ardrone/ardrone_sdk/ARDrone_SDK_1_6_20110224/ARDroneLib



I_AR_LIB= $(APATH)/Soft/Lib/
I_AR_API= $(APATH)/Soft/Common/
I_AR_SDK= $(APATH)/VP_SDK/
I_AR_SOS= $(APATH)/VP_SDK/VP_Os/linux/

ifeq ($(ARM),yes)
CC	= arm-none-linux-gnueabi-gcc
STRIP	= arm-none-linux-gnueabi-strip
L_SUFFIX= PROD_MODE_Linux_2.6.35-22-generic-pae_GNU_Linux_arm-none-linux-gnueabi-gcc_4.3.3/
CFLAGS += -D_ARM_
else
CC	= gcc
STRIP	= strip
L_SUFFIX= PROD_MODE_Linux_2.6.35-22-generic-pae_GNU_Linux_gcc_4.4.5/
endif


L_AR_SDK= $(APATH)/Soft/Build/targets_versions/sdk_$(L_SUFFIX)
L_AR_PCA= $(APATH)/Soft/Build/targets_versions/ardrone_lib_$(L_SUFFIX)

CFLAGS += -I. -I$(APATH) -I$(I_AR_LIB) -I$(I_AR_API) -I$(I_AR_SDK) -I$(I_AR_SOS) -DNO_ARDRONE_MAINLOOP
LDFLAGS+= -L$(L_AR_PCA) -lpc_ardrone -L$(L_AR_SDK) -lsdk -lpthread



###############################################################################

OBJS		= $(SRCS:.c=.o)

all:		$(NAME)

$(NAME):	$(OBJS)
		@echo [LD] $(NAME)
		@$(CC) -o $@ $^ $(LDFLAGS)
		@$(STRIP) --strip-unneeded $(NAME)

%.o: %.c
		@echo [CC] $<
		@$(CC) -o $@ -c $< $(CFLAGS)

clean:
		@echo [RM]
		@rm -f $(OBJS)
		@rm -f *~
		@rm -f \#*\#
		@rm -f core

fclean:		clean
		@rm -f $(NAME)

re:		clean all

tgz:		fclean
		@echo [TGZ] $(NAME)-src-$$(date +"%y%m%d%H%M%S").tgz
		@tar cf ../$(NAME)-src-$$(date +"%y%m%d%H%M%S").tgz \
			../navigation

run:		all
		./$(NAME)
