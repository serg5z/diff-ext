#include <log/log.h>
#include <log/file_sink.h>
#include <log/log_message.h>

int
main() {
  FILE_SINK file("./test.log", 5);
  LOG* log = LOG::instance();
  LOG_MESSAGE msg("");
  
  log->add_sink(&file);
  
  msg = LOG_MESSAGE("first message\n", 1);
  log->info(msg);
  msg = LOG_MESSAGE("this should not be here\n", 10);
  log->info(msg);
  
  msg = LOG_MESSAGE("first debug message\n", 1);
  log->debug(msg);
  msg = LOG_MESSAGE("this should not be here\n", 10);
  log->debug(msg);
  
  return 0;
}
