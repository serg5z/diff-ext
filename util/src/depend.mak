hyperlink.o: \
  hyperlink.c
  
file_sink.o: \
  file_sink.cpp \
  ..\..\util\include\log\file_sink.h \
  ..\..\util\include\log\log_sink.h \
  ..\..\util\include\log\log_message.h \
  ..\..\util\include\util\string.h 
  
log.o: \
  log.cpp \
  ..\..\util\include\log\log_message.h \
  ..\..\util\include\util\string.h \
  ..\..\util\include\log\log_sink.h \
  ..\..\util\include\log\log.h \
  ..\..\util\include\util\cont\list.h
  
menu.o: \
  menu.cpp \
  ../../util/include/util/menu.h \
  ../../util/include/util/string.h
  
string.o: \
  string.cpp \
  ..\..\util\include\util\string.h 
  
trace.o: \
  trace.cpp \
  ..\..\util\include\debug\trace.h \
  ..\..\util\include\log\log.h \
  ..\..\util\include\util\cont\list.h \
  ..\..\util\include\log\log_message.h \
  ..\..\util\include\util\string.h
