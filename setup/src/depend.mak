about.obj: \
  about.c \
  $(ROOT)\util\include\util\dialog.h \
  ..\include\resource.h \

main.obj: \
  main.c \
  $(ROOT)\dialog-layout\include\layout.h \
  page.h \
  ..\include\resource.h \
  
diff_ext_setup.res: \
  diff_ext_setup.rc \
  ..\include\resource.h \

.version: \
  about.c \
  debug_page.c \
  main.c \
  diff_ext_setup.rc \
