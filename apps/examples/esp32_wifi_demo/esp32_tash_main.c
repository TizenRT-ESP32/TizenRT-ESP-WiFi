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
/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <tinyara/config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <apps/shell/tash.h>

#include <tinyara/fs/fs.h>
#include <tinyara/fs/ioctl.h>
#include <tinyara/kmalloc.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

/****************************************************************************
 * Definitions
 ****************************************************************************/
#define ESP32_TASH_PRI      (100)
#define ESP32_TASH_STAKSIZE (4096)

/****************************************************************************
 * Private Data & Functions
 ****************************************************************************/
/* example */

/*  Call-back function registered in TASH.
 *   This creates pthread to run an example with ASYNC TASH excution type.
 *   Only three points can be modified
 *   1. priority
 *   2. stacksize
 *   3. register entry function of pthread (example)
 */

extern pthread_addr_t esp32_demo_entry(pthread_addr_t arg);

static int esp32_wifi_demo_cb(int argc, char **args)
{
	pthread_t wifi_thread;

	pthread_attr_t attr;
	struct sched_param sparam;
	int status;

	/* Initialize the attribute variable */
	status = pthread_attr_init(&attr);
	if (status != 0) {
		printf("wifi_thread : pthread_attr_init failed, status=%d\n", status);
	}

	/* 1. set a priority */
	sparam.sched_priority = ESP32_TASH_PRI;
	status = pthread_attr_setschedparam(&attr, &sparam);
	if (status != OK) {
		printf("wifi_thread : pthread_attr_setschedparam failed, status=%d\n", status);
	}

	/* 2. set a stacksize */
	status = pthread_attr_setstacksize(&attr, ESP32_TASH_STAKSIZE);
	if (status != OK) {
		printf("wifi_thread : pthread_attr_setstacksize failed, status=%d\n", status);
	}
	//pthread_attr_set
	//schedpolicy(&attr, SCHED_RR);
	/* 3. create pthread with entry function */

	status = pthread_create(&wifi_thread, &attr, esp32_demo_entry, NULL);
	if (status != 0) {
		printf("wifi_thread: pthread_create failed, status=%d\n", status);
	}

	/* Wait for the threads to stop */
	pthread_join(wifi_thread, NULL);
	printf("esp32_demo_thread is finished\n");

	return 0;

}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int esp32_tash_main(int argc, char **args)
{
	tash_cmd_install("esp32_wifidemo", esp32_wifi_demo_cb, TASH_EXECMD_ASYNC);
	return 0;
}
#endif
