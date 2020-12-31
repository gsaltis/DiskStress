CC				       = gcc
LINK				       = gcc
CC_OPTS				       = -c -g -Wall
CC_RELEASE_OPTS			       = 
CC_DEVEL_OPTS			       = -DANSI_COLORS_SUPPORTED
CC_INCS				       = 
LINK_OPTS			       = -g -LGeneralUtilities -LRPiBaseModules
LINK_LIBS			       = -lpthread -lutils -lmongoose -llinenoise -ljson -lm

TARGET				       = diskstress

SRCS		  		       = $(sort					\
					   main.c				\
					   UserInputServerThread.c		\
					   DiskStressThread.c			\
					   WebSocketServerThread.c		\
					   WebConnection.c			\
					   HTTPServerThread.c			\
					   Log.c				\
					   JSONIF.c				\
					   JSONOut.c				\
					   DiskInformation.c			\
					   FileInfoBlock.c			\
					  )


RELEASE_OBJS		  	       = $(patsubst %.c,rel/%.o,$(SRCS))

DEVEL_OBJS	  		       = $(patsubst %.c,dev/%.o,$(SRCS))

LIBS				       = 

dev/%.o				       : %.c
					 @echo [CC] $@
					 @$(CC) $(CC_OPTS) $(CC_DEVEL_OPTS) $(CC_INCS) $< -o $@
				        
rel/%.o				       : %.c
					 @echo [CC] $@
					 @$(CC) $(CC_OPTS) $(CC_RELEASE_OPTS) $(CC_INCS) $< -o $@
all				       : 
					 @echo Must make either 'release' or 'devel'

release				       : $(RELEASE_OBJS)
					 @echo [LD] $(TARGET):RELEASE
					 @$(LINK) $(LINK_OPTS) -o $(TARGET) $(RELEASE_OBJS) $(LINK_LIBS) $(LIBS)
					 @cp $(TARGET) rel

devel				       : $(DEVEL_OBJS)
					 @echo [LD] $(TARGET):DEVELOPMENT
					 @$(LINK) $(LINK_OPTS) -o $(TARGET) $(DEVEL_OBJS) $(LINK_LIBS) $(LIBS)
					 @cp $(TARGET) dev

include					 depends.mk

.PHONY				       : junkclean
junkclean			       : 
					 rm -rf $(wildcard *~ *-bak www/*~)

.PHONY				       : clean
clean				       : junkclean
					 rm -rf $(wildcard $(RELEASE_OBJS) $(DEVEL_OBJS) dev/$(TARGET) rel/$(TARGET) $(TARGET))
