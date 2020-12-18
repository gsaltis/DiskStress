DiskInformation.o: DiskInformation.c DiskInformation.h JSONOut.h \
 GeneralUtilities/String.h GeneralUtilities/NumericTypes.h
DiskStressThread.o: DiskStressThread.c DiskStressThread.h \
 GeneralUtilities/ANSIColors.h UserInputServerThread.h
FileInfoBlock.o: FileInfoBlock.c FileInfoBlock.h \
 GeneralUtilities/String.h GeneralUtilities/MemoryManager.h
HTTPServerThread.o: HTTPServerThread.c HTTPServerThread.h \
 GeneralUtilities/ANSIColors.h WebSocketServerThread.h \
 RPiBaseModules/mongoose.h GeneralUtilities/String.h
JSONIF.o: JSONIF.c RPiBaseModules/json.h GeneralUtilities/String.h \
 GeneralUtilities/MemoryManager.h JSONIF.h
JSONOut.o: JSONOut.c JSONOut.h GeneralUtilities/String.h \
 GeneralUtilities/MemoryManager.h
main.o: main.c main.h UserInputServerThread.h WebSocketServerThread.h \
 DiskStressThread.h HTTPServerThread.h DiskInformation.h JSONOut.h \
 GeneralUtilities/String.h
UserInputServerThread.o: UserInputServerThread.c UserInputServerThread.h \
 GeneralUtilities/String.h RPiBaseModules/linenoise.h HTTPServerThread.h
WebConnection.o: WebConnection.c WebConnection.h \
 RPiBaseModules/mongoose.h GeneralUtilities/MemoryManager.h \
 GeneralUtilities/String.h
WebSocketServerThread.o: WebSocketServerThread.c WebSocketServerThread.h \
 GeneralUtilities/ANSIColors.h DiskStressThread.h \
 RPiBaseModules/mongoose.h WebConnection.h GeneralUtilities/String.h \
 JSONIF.h RPiBaseModules/json.h JSONOut.h \
 GeneralUtilities/MemoryManager.h DiskInformation.h
