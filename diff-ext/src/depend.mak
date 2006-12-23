class_factory.o: \
  class_factory.cpp \
  class_factory.h \
  $(ROOT)/util/include/util/cont/dlist.h \
  $(ROOT)/util/include/util/string.h \
  $(ROOT)/util/include/log/file_sink.h \
  $(ROOT)/util/include/log/log_sink.h \
  diff_ext.h \
  server.h

diff_ext.o: \
  diff_ext.cpp \
  $(ROOT)/util/include/log/log.h \
  $(ROOT)/util/include/util/cont/list.h \
  $(ROOT)/util/include/log/log_message.h \
  $(ROOT)/util/include/util/string.h \
  $(ROOT)/util/include/debug/trace.h \
  $(ROOT)/util/include/util/cont/dlist.h \
  $(ROOT)/util/include/log/file_sink.h \
  $(ROOT)/util/include/log/log_sink.h \
  diff_ext.h \
  server.h \
  ../include/resource.h

icon_factory.o: \
  icon_factory.cpp \
  server.h \
  $(ROOT)/util/include/util/string.h \
  $(ROOT)/util/include/log/file_sink.h \
  $(ROOT)/util/include/log/log_sink.h icon_factory.h \
  
server.o: \
  server.cpp \
  $(ROOT)/util/include/log/log.h \
  $(ROOT)/util/include/util/cont/list.h \
  $(ROOT)/util/include/log/log_message.h \
  $(ROOT)/util/include/util/string.h \
  $(ROOT)/util/include/log/file_sink.h \
  $(ROOT)/util/include/log/log_sink.h \
  $(ROOT)/util/include/debug/trace.h \
  $(ROOT)/util/include/util/cont/dlist.h \
  server.h \
  class_factory.h 
  
resource.res: \
  resource.rc \
  ..\include\resource.h  

.version: \
  class_factory.cpp \
  diff_ext.cpp \
  server.cpp \
  resource.rc
