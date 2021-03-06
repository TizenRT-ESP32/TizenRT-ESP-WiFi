/****************************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
/****************************************************************************
 * examples/hello/hello_main.c
 *
 *   Copyright (C) 2008, 2011-2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <tinyara/config.h>
#include <stdio.h>
#include <stdlib.h>

/****************************************************************************
 * hello_main
 ****************************************************************************/
extern bool esp_spiram_test();
#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int hello_main(int argc, char *argv[])
#endif
{
	printf("Hello, World!!\n");
	if(atoi(argv[1]) == 1) {
		printf("esp_spiram_test -> \n");
		esp_spiram_test();
	} else {
		int size = 100 * 1024 * sizeof(int);
		int *malloctest = (int*)malloc(size);
		printf("malloctest addr 0x%x\n",malloctest);
		if(malloctest) {
			memset(malloctest, 0xAB, size);
			printf("0x%x, 0x%x, 0x%x\n",malloctest[0], malloctest[1024], malloctest[10240]);
		}

		int *zalloctest = (int*)zalloc(size);
		printf("zalloc  addr 0x%x\n",zalloctest);
		if(zalloctest) {
			printf("0x%x, 0x%x, 0x%x\n",zalloctest[0], zalloctest[1024], zalloctest[10240]);
		}

		int *calloctest = (int*)calloc(100 * 1024, sizeof(int));
		printf("calloc	addr 0x%x\n",calloctest);
		if(calloctest) {
			printf("0x%x, 0x%x, 0x%x\n",calloctest[0], calloctest[1024], calloctest[10240]);
		}

		free(malloctest);
		free(zalloctest);
		free(calloctest);

	}
	return 0;
}
