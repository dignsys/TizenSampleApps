/*
 * Copyright (c) 2019 DIGNSYS Inc.
 *
 * Contact: Hyobok Ahn (hbahn@dignsys.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __anchor_log_H__
#define __anchor_log_H__

#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "ANCHOR"

#define LOGE(fmt, args...) dlog_print(DLOG_ERROR, LOG_TAG, "%s : %s(%d) > "fmt"\n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)
#define LOGW(fmt, args...) dlog_print(DLOG_WARN, LOG_TAG, "%s : %s(%d) > "fmt"\n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)
#define LOGI(fmt, args...)  dlog_print(DLOG_INFO, LOG_TAG, "%s : %s(%d) > "fmt"\n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)
#define LOGD(fmt, args...) dlog_print(DLOG_DEBUG, LOG_TAG, "%s : %s(%d) > "fmt"\n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)

#define START dlog_print(DLOG_DEBUG, LOG_TAG, "%s : %s(%d) >>>>>>>> called", rindex(__FILE__, '/') + 1, __func__, __LINE__)
#define END dlog_print(DLOG_DEBUG, LOG_TAG, "%s : %s(%d) <<<<<<<< ended", rindex(__FILE__, '/') + 1, __func__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif /* __anchor_log_H__ */
