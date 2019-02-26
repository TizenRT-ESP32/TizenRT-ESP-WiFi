/******************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <rom/ets_sys.h>
#include <time.h>
#include "esp_wifi_os_adapter.h"
#include "event_groups.h"
#include "esp32_queue_api.h"
#include <rom/ets_sys.h>
#include <time.h>

extern void wifi_station_entry();
extern void wifi_softap_entry();

pthread_addr_t esp32_demo_entry(pthread_addr_t arg)
{
	printf("start esp32 wifi demo!\n");

#ifdef CONFIG_ESP_WIFI_MODE_STATION
	wifi_station_entry();
#else
	wifi_softap_entry();
#endif
	return NULL;
}
