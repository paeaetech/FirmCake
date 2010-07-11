#ifndef GCODEPARSER_H_MO8BM1WI
#define GCODEPARSER_H_MO8BM1WI

#include <inttypes.h>
#include "Packet.h"
#include "owntypes.h"

#define GCODE_BUFFER_SIZE 256

class GCodeParser
{
public:
	GCodeParser();
	
	//returns true if end of line received
	bool processByte(uint8_t b);
	
	bool getPacket(Packet& rPacket);
	
	void reset() { mCodePos=0; mbMetric = true; mpParsePos = 0; mPreviousFeedrate = 2500; mPreviousPoint.zero(); }
	
protected:
	
	uint16_t mCodePos;
	uint8_t mCodeBuffer[GCODE_BUFFER_SIZE];
	
	void getCoordinates(Point& rPoint);
	
	Point mPreviousPoint;
	uint32_t mPreviousFeedrate;
	
	void initToolCommand(Packet& rPacket,uint8_t cmd);
	
	bool mbMetric;
	
	float getValue();
	int getCodeInt();
	float getCodeFloat();
	bool hasCode(char c);
	
	char* mpParsePos;
};

extern GCodeParser gcodeParser;

#endif /* end of include guard: GCODEPARSER_H_MO8BM1WI */
