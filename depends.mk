DiskInformation.o: DiskInformation.c DiskInformation.h JSONOut.h \
 GeneralUtilities/String.h GeneralUtilities/NumericTypes.h
DiskStressThread.o: DiskStressThread.c DiskStressThread.h \
 GeneralUtilities/String.h JSONOut.h GeneralUtilities/ANSIColors.h \
 UserInputServerThread.h FileInfoBlock.h GeneralUtilities/MemoryManager.h \
 DiskInformation.h Log.h
FileInfoBlock.o: FileInfoBlock.c FileInfoBlock.h \
 GeneralUtilities/String.h JSONOut.h GeneralUtilities/MemoryManager.h
HTTPServerThread.o: HTTPServerThread.c HTTPServerThread.h \
 GeneralUtilities/String.h GeneralUtilities/ANSIColors.h \
 WebSocketServerThread.h RPiBaseModules/mongoose.h \
 GeneralUtilities/MemoryManager.h Log.h
JSONIF.o: JSONIF.c RPiBaseModules/json.h GeneralUtilities/String.h \
 GeneralUtilities/MemoryManager.h JSONIF.h
JSONOut.o: JSONOut.c JSONOut.h GeneralUtilities/String.h \
 GeneralUtilities/MemoryManager.h
Log.o: Log.c Log.h GeneralUtilities/String.h \
 GeneralUtilities/MemoryManager.h
main.o: main.c main.h UserInputServerThread.h WebSocketServerThread.h \
 GeneralUtilities/String.h DiskStressThread.h JSONOut.h \
 HTTPServerThread.h DiskInformation.h GeneralUtilities/MemoryManager.h \
 GeneralUtilities/ANSIColors.h GeneralUtilities/NumericTypes.h Log.h
UserInputServerThread.o: UserInputServerThread.c UserInputServerThread.h \
 GeneralUtilities/String.h RPiBaseModules/linenoise.h HTTPServerThread.h \
 GeneralUtilities/ANSIColors.h DiskStressThread.h JSONOut.h \
 DiskInformation.h FileInfoBlock.h
WebConnection.o: WebConnection.c WebConnection.h \
 RPiBaseModules/mongoose.h GeneralUtilities/MemoryManager.h \
 GeneralUtilities/String.h
WebSocketServerThread.o: WebSocketServerThread.c WebSocketServerThread.h \
 GeneralUtilities/String.h GeneralUtilities/ANSIColors.h \
 DiskStressThread.h JSONOut.h RPiBaseModules/mongoose.h WebConnection.h \
 JSONIF.h RPiBaseModules/json.h GeneralUtilities/MemoryManager.h \
 DiskInformation.h Log.h FileInfoBlock.h GeneralUtilities/NumericTypes.h
