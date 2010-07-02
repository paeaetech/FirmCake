/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2009      */
/*-----------------------------------------------------------------------*/
#include <inttypes.h>
#include "diskio.h"
#include "mmc/mmc.h"


uint8_t buf[512];
uint32_t writeSector;
uint16_t writePtr;
uint32_t readSector;
bool isWriting;
bool readBufValid;

void bufCopy(void *dest,uint8_t* src,uint16_t size)
{
	uint8_t* d = (uint8_t*)dest;
	while(size--)
	{
		*d++ = *src++;
	}
}
/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/



DSTATUS disk_initialize (void)
{
	return mmcReset();
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	void* dest,			/* Pointer to the destination object */
	DWORD sector,		/* Sector number (LBA) */
	WORD sofs,			/* Offset in the sector */
	WORD count			/* Byte count (bit15:destination) */
)
{
	if (isWriting)
		return RES_NOTRDY;
	
	if (sector == readSector && readBufValid)
	{
		bufCopy(dest,buf+sofs,count);
		return RES_OK;
	}
	
	if (!mmcRead(sector,buf))
	{
		readSector = sector;
		readBufValid = true;
		bufCopy(dest,buf+sofs,count);
		return RES_OK;
	}
	
	return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (
	BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
)
{
	DRESULT res=RES_OK;


	if (!buff) {
		if (sc) {
			
			writePtr=0;
			writeSector = sc;
			isWriting=true;
			// Initiate write process

		} else {

			// Finalize write process
			isWriting=false;
			return mmcWrite(writeSector,buf) == 0 ? RES_OK : RES_ERROR;
		}
	} else {
		bufCopy(&buf[writePtr],buff,sc);
		writePtr+=sc;
		readBufValid = false;
		// Send data to the disk

	}

	return res;
}

