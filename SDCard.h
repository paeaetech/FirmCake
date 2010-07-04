#ifndef SDCARD_H_EXIGGX8O
#define SDCARD_H_EXIGGX8O

#include <inttypes.h>
#include "config.h"
#include "Packet.h"

#ifdef USE_SDCARD

enum SDCardResponse {
  SD_SUCCESS              = 0,  // Operation succeeded
  SD_ERR_NO_CARD_PRESENT  = 1,  // No SD card is inserted in the slot
  SD_ERR_INIT_FAILED      = 2,  // SD card initialization failed
  SD_ERR_PARTITION_READ   = 3,  // Couldn't read the card's partition table
  SD_ERR_OPEN_FILESYSTEM  = 4,  // Couldn't open the FAT16 filesystem -- 
                                //  check that it's real FAT16
  SD_ERR_NO_ROOT          = 5,  // No root directory found
  SD_ERR_CARD_LOCKED      = 6,  // Card is locked, writing forbidden
  SD_ERR_FILE_NOT_FOUND   = 7,  // Could not find specific file
  SD_ERR_GENERIC          = 8   // General error
};


class SDCard
{
public:
	SDCard();
	
	bool isRecording() const { return mbRecord; }
	bool isPlayback() const { return mbPlayback; }
	
	SDCardResponse startRecord(const char* pFilename);
	void recordPacket(const Packet& rPacket);
	uint32_t endRecord();
	
	SDCardResponse startPlayback(const char* pFilename);
	bool playbackDataAvailable();
	uint8_t playbackRead8();
	void endPlayback();
	
	SDCardResponse resetScan();
	SDCardResponse getNextFile(uint8_t* pBuffer,uint8_t size);
	
	
protected:	
	SDCardResponse init();
	
	uint32_t getFilesize(const char* pFilename);
	
	bool mbRecord;
	bool mbPlayback;
	uint32_t mRecordedBytes;
	uint32_t mOpenFilesize;
	
};


extern SDCard sdCard;

#endif


#endif /* end of include guard: SDCARD_H_EXIGGX8O */
