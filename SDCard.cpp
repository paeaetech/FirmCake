
#include "SDCard.h"
#include "fatfs/pff.h"

#ifdef USE_SDCARD

namespace {
	FATFS fatFs;
	DIR dir;
	
	SDCardResponse fresultToResponse(FRESULT res)
	{
		SDCardResponse ret = SD_SUCCESS;
		switch(res)
		{
			case FR_OK:
				break;
			case FR_DISK_ERR:
				ret = SD_ERR_GENERIC;
				break;
			case FR_NOT_READY:
				ret = SD_ERR_NO_CARD_PRESENT;
				break;
			case FR_NO_FILE:
			case FR_NO_PATH:
				ret = SD_ERR_FILE_NOT_FOUND;
				break;
			case FR_NOT_OPENED:
				ret = SD_ERR_NO_ROOT;
				break;
			case FR_NOT_ENABLED:
				ret = SD_ERR_CARD_LOCKED;
				break;
			case FR_NO_FILESYSTEM:
				ret = SD_ERR_PARTITION_READ;
				break;
		}
		return ret;
	}
};


SDCard sdCard;


SDCard::SDCard()
: mbRecord(false), mbPlayback(false)
{
	
}

SDCardResponse SDCard::init()
{
	return fresultToResponse(pf_mount(&fatFs));
}

SDCardResponse SDCard::startRecord(const char* pFilename)
{
	mRecordedBytes = 0;
	mOpenFilesize = 0;
	FRESULT res = pf_open(pFilename);
	if (res != FR_OK)
		return fresultToResponse(res);
	
	
}

void SDCard::recordPacket(const Packet& rPacket)
{
	
}

uint32_t SDCard::endRecord()
{
	return mRecordedBytes;
}

SDCardResponse SDCard::startPlayback(const char* pFilename)
{
	SDCardResponse ret = init();
	if (ret != SD_SUCCESS)
		return ret;
	
	FRESULT res = pf_open(pFilename);
	if (res != FR_OK)
		return fresultToResponse(res);
	
	mbPlayback = true;
	return SD_SUCCESS;	
}

bool SDCard::playbackDataAvailable()
{
	
}

uint8_t SDCard::playbackRead8()
{
	uint8_t b = 0;
	uint16_t readSize;
	
	FRESULT res = pf_read(&b,1,(WORD*)&readSize);
	return b;
}

void SDCard::endPlayback()
{
	mbPlayback = false;
}


SDCardResponse SDCard::resetScan()
{
	return fresultToResponse(pf_opendir(&dir,0));
}

SDCardResponse SDCard::getNextFile(uint8_t* pBuffer,uint8_t size)
{
	FILINFO fileinfo;
	FRESULT ret = pf_readdir(&dir,&fileinfo);
	if (ret != FR_OK)
		return fresultToResponse(ret);
		
	uint8_t b=0;
	while(fileinfo.fname[b] && b < size-1)
	{
		pBuffer[b] = fileinfo.fname[b];
		b++;
	}
	return SD_SUCCESS;
}



#endif