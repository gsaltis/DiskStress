CC				       = gcc
LINK				       = gcc
CC_OPTS				       = -c -g -Wall -DANSI_COLORS_SUPPORTED
CC_INCS				       = 
LINK_OPTS			       = -g -LGeneralUtilities -LRPiBaseModules
LINK_LIBS			       = -lpthread -lutils -lmongoose -llinenoise -ljson -lm

TARGET				       = diskstress
OBJS				       = $(sort					\
					   main.o				\
					   UserInputServerThread.o		\
					   DiskStressThread.o			\
					   WebSocketServerThread.o		\
					   WebConnection.o			\
					   HTTPServerThread.o			\
					   JSONIF.o				\
					   JSONOut.o				\
					   DiskInformation.o			\
					   FileInfoBlock.o			\
					  )
LIBS				       = 
%.o				      : %.c
					 @echo [CC] $<
					 @$(CC) $(CC_OPTS) $(CC_INCS) $<

.PHONY				       : $(TARGET)
$(TARGET)			       : $(OBJS)
					 @echo [LD] $(TARGET)
					 @$(LINK) $(LINK_OPTS) -o $(TARGET) $(OBJS) $(LINK_LIBS) $(LIBS)

include					 depends.mk

.PHONY				       : junkclean
junkclean			       : 
					 rm -rf $(wildcard *~ *-bak www/*~)

.PHONY				       : clean
clean				       : junkclean
					 rm -rf $(wildcard $(OBJS) $(TARGET))
