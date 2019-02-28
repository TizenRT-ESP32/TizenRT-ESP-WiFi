/****************************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
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
/************************************************************************************
 * Included Files
 ************************************************************************************/

#include <tinyara/config.h>

#include <sys/types.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <tinyara/kmalloc.h>
#include <tinyara/fs/ioctl.h>
#include <tinyara/fs/mtd.h>

/************************************************************************************
 * Pre-processor Definitions
 ************************************************************************************/
#define PAGE_SHIFT (8)
#define ESP32_PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTOR_SHIFT (12)
#define ESP32_SECTOR_SIZE (1 << SECTOR_SHIFT)
#define FLASH_FS_START (0x120000)
#define ESP32_FS_CAPACITY (0x100000)
#define ESP32_NSECTORS (ESP32_FS_CAPACITY / ESP32_SECTOR_SIZE)
#define ESP32_START_SECOTR (FLASH_FS_START / ESP32_SECTOR_SIZE)

/************************************************************************************
 * Private Types
 ************************************************************************************/

/* This type represents the state of the MTD device.  The struct mtd_dev_s must
 * appear at the beginning of the definition so that you can freely cast between
 * pointers to struct mtd_dev_s and struct esp32_dev_s.
 */
struct esp32_dev_s {
	struct mtd_dev_s mtd;		/* MTD interface */
	int nsectors;				/* number of erase sectors */
};

/************************************************************************************
 * Private Function Prototypes
 ************************************************************************************/

/* MTD driver methods */
static int esp32_erase(FAR struct mtd_dev_s *dev, off_t startblock, size_t nblocks);
static ssize_t esp32_bread(FAR struct mtd_dev_s *dev, off_t startblock, size_t nblocks, FAR uint8_t *buf);
static ssize_t esp32_bwrite(FAR struct mtd_dev_s *dev, off_t startblock, size_t nblocks, FAR const uint8_t *buf);
static ssize_t esp32_read(FAR struct mtd_dev_s *dev, off_t offset, size_t nbytes, FAR uint8_t *buffer);
static int esp32_ioctl(FAR struct mtd_dev_s *dev, int cmd, unsigned long arg);
#if defined(CONFIG_MTD_BYTE_WRITE) && !defined(CONFIG_W25_READONLY)
static ssize_t esp32_write(FAR struct mtd_dev_s *dev, off_t offset, size_t nbytes, FAR const uint8_t *buffer);
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/
size_t get_flash_address(off_t page)
{
	return FLASH_FS_START + ESP32_PAGE_SIZE * page;
}

/************************************************************************************
 * Name: esp32_erase
 ************************************************************************************/

static int esp32_erase(FAR struct mtd_dev_s *dev, off_t startblock, size_t nblocks)
{
	ssize_t result;
	/* Erase the specified blocks and return status (OK or a negated errno) */
	while (nblocks > 0) {
		result = spi_flash_erase_sector(startblock + ESP32_START_SECOTR);
		if (result < 0) {
			return (int)result;
		}
		startblock++;
		nblocks--;
	}
	return OK;
}

/************************************************************************************
 * Name: esp32_bread
 ************************************************************************************/
static ssize_t esp32_bread(FAR struct mtd_dev_s *dev, off_t startblock, size_t nblocks, FAR uint8_t *buffer)
{
	ssize_t result;
	result = spi_flash_read(get_flash_address(startblock), buffer, nblocks << PAGE_SHIFT);
	return result < 0 ? result : nblocks;
}

/************************************************************************************
 * Name: esp32_bwrite
 ************************************************************************************/
static ssize_t esp32_bwrite(FAR struct mtd_dev_s *dev, off_t startblock, size_t nblocks, FAR const uint8_t *buffer)
{
	ssize_t result;

	/* Write the specified blocks from the provided user buffer and return status
	 * (The positive, number of blocks actually written or a negated errno)
	 */

	result = spi_flash_write(get_flash_address(startblock), buffer, nblocks << PAGE_SHIFT);
	return result < 0 ? result : nblocks;

}

/************************************************************************************
 * Name: esp32_read
 ************************************************************************************/

static ssize_t esp32_read(FAR struct mtd_dev_s *dev, off_t offset, size_t nbytes, FAR uint8_t *buffer)
{
	ssize_t result;
	result = spi_flash_read(FLASH_FS_START + offset, buffer, nbytes);
	return result < 0 ? result : nbytes;
}

/************************************************************************************
 * Name: esp32_write
 ************************************************************************************/

#if defined(CONFIG_MTD_BYTE_WRITE) && !defined(CONFIG_W25_READONLY)
static ssize_t esp32_write(FAR struct mtd_dev_s *dev, off_t offset, size_t nbytes, FAR const uint8_t *buffer)
{
	FAR struct esp32_dev_s *priv = (FAR struct mtd_dev_s *)dev;
}
#endif							/* defined(CONFIG_MTD_BYTE_WRITE) && !defined(CONFIG_W25_READONLY) */

/************************************************************************************
 * Name: esp32_ioctl
 ************************************************************************************/

static int esp32_ioctl(FAR struct mtd_dev_s *dev, int cmd, unsigned long arg)
{
	int ret = -EINVAL;			/* Assume good command with bad parameters */
	FAR struct esp32_dev_s *priv = (FAR struct mtd_dev_s *)dev;
	fvdbg("cmd: %d \n", cmd);

	switch (cmd) {
	case MTDIOC_GEOMETRY: {
		FAR struct mtd_geometry_s *geo = (FAR struct mtd_geometry_s *)((uintptr_t) arg);
		if (geo) {
			geo->blocksize = ESP32_PAGE_SIZE;
			geo->erasesize = ESP32_SECTOR_SIZE;
			geo->neraseblocks = priv->nsectors;
			ret = OK;
			fvdbg("blocksize: %d erasesize: %d neraseblocks: %d\n", geo->blocksize, geo->erasesize, geo->neraseblocks);
		}
	}
	break;

	case MTDIOC_BULKERASE: {
		/* Erase the entire device */
		ret = esp32_erase(dev, 0, priv->nsectors);
	}
	break;

	case MTDIOC_XIPBASE:
	default:
		ret = -ENOTTY;			/* Bad command */
		break;
	}

	fvdbg("return %d\n", ret);
	return ret;
}

/************************************************************************************
 * Public Functions
 ************************************************************************************/

/************************************************************************************
 * Name: esp32_initialize
 *
 * Description:
 *   Create an initialize MTD device instance.  MTD devices are not registered
 *   in the file system, but are created as instances that can be bound to
 *   other functions (such as a block or character driver front end).
 *
 ************************************************************************************/

FAR struct mtd_dev_s *up_flashinitialize(void)
{
	FAR struct esp32_dev_s *priv;
	priv = (FAR struct esp32_dev_s *)kmm_zalloc(sizeof(struct esp32_dev_s));
	if (priv) {
		/* Initialize the allocated structure (unsupported methods were
		 * nullified by kmm_zalloc).
		 */
		priv->mtd.erase = esp32_erase;
		priv->mtd.bread = esp32_bread;
		priv->mtd.bwrite = esp32_bwrite;
		priv->mtd.read = esp32_read;
		priv->mtd.ioctl = esp32_ioctl;
#if defined(CONFIG_MTD_BYTE_WRITE)
		priv->mtd.write = esp32_write;
#endif
		priv->nsectors = ESP32_NSECTORS;
		return (FAR struct mtd_dev_s *)priv;
	}
	return NULL;
}
