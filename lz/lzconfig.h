#include "base/log.h"


#define LzLog(level, fmt, ...) LOG_DIR("lzrecorder.log", level, fmt, ##__VA_ARGS__)

#define LzLogInfo(fmt, ...) LzLog(INFO, fmt, ##__VA_ARGS__)
#define LzLogWarn(fmt, ...) LzLog(WARN, fmt, ##__VA_ARGS__)
#define LzLogError(fmt, ...) LzLog(ERROR, fmt, ##__VA_ARGS__)
