
#include "config.h"
#include "GCodeParser.h"
#include "Commands.h"
#include <stdlib.h>

namespace {
	
	int8_t strcmp(const char* s1,const char* s2)
	{
		while(*s1 && *s2)
		{
			if (*s1 != *s2)
				return *s1 - *s2;
			s1++;
			s2++;
		}
		return 0;
	}
	
	char* strstr(const char* s1,const char* s2)
	{
		while(*s1)
		{
			if (*s1 == *s2 && !strcmp(s1,s2))
				return (char*)s1;
				
			s1++;
		}
		return 0;
	}


	char* strchr(const char* s1,char c)
	{
		while(*s1)
		{
			if (*s1 == c)
				return (char*)s1;
				
			s1++;
		}
		return 0;
	}
	
	int32_t inchesToMM(int32_t i) 
	{ 
		return (int32_t)(i*25.4f); 
	}

	char toupper(char c)
	{
		return c <= 'z' ? 'A'+(c-'a') : c;
	}
};

#define ISWHITE(x) (x == ' ' || x == '\t')
#define ISALPHA(x) ((x >= 'a' && x<='z') || (x >= 'A' && x <= 'Z'))
#define ISDIGIT(x) (x >= '0' && x <= '9')


GCodeParser gcodeParser;



bool GCodeParser::hasCode(char c)
{
	mpParsePos = strchr((const char*)mCodeBuffer,c);
	return mpParsePos != 0;
}


int GCodeParser::getCodeInt()
{
	return (uint8_t)strtol(mpParsePos,NULL,10);
}

float GCodeParser::getCodeFloat()
{
	return (float)strtod(mpParsePos,NULL);
}

GCodeParser::GCodeParser()
: mPreviousPoint(0,0,0)
{
	reset();
}

bool GCodeParser::processByte(uint8_t b)
{
	switch(b)
	{
		case '\r':
		case '\n':
			if (mCodePos == 0)
				return false;
			mCodeBuffer[mCodePos]='\0';
			return true;
		default:
			if (mCodePos < GCODE_BUFFER_SIZE-1)
				mCodeBuffer[mCodePos++]=b;
			break;
	}
	return false;
}

bool GCodeParser::getPacket(Packet& rPacket)
{
	mpParsePos=(char*)mCodeBuffer;

	if (hasCode('G'))
	{
		switch(getCodeInt())
		{
			case 0:
			case 1: //linear motion
			{
				Point p;
				getCoordinates(p);
				uint32_t feedRate = mPreviousFeedrate;
				if (hasCode('F'))
				{
					feedRate = getCodeInt();
					if (!mbMetric)
						feedRate = inchesToMM(feedRate);
					mPreviousFeedrate = feedRate;
				}
				
				rPacket.put8(HOST_CMD_QUEUE_POINT_ABS);
				rPacket.putPoint(p);
				rPacket.put32(feedRate);
				return true;
			}
			case 2: //clockwise arc
			case 3: //counterclockwise arc
				break;
			case 20: //set imperial units
			case 70:
				mbMetric = false;
				break;
			case 21:
			case 71:
				mbMetric = true;
				break;
			
			case 92: //set position
			{
				Point p;
				getCoordinates(p);
				rPacket.put8(HOST_CMD_SET_POSITION);
				rPacket.putPoint(p);
				return true;
			}
		}
	}
	else if (hasCode('M'))
	{
		switch(getCodeInt())
		{
			case 17: //enable drivers
				rPacket.put8(HOST_CMD_ENABLE_AXES);
				rPacket.put8(0x80|_BV(1)|_BV(2)|_BV(3));
				return true;
			case 18: //disable drivers
				rPacket.put8(HOST_CMD_ENABLE_AXES);
				rPacket.put8(_BV(1)|_BV(2)|_BV(3));
				return true;
			case 101: //extruder on forward
				initToolCommand(rPacket,SLAVE_CMD_TOGGLE_MOTOR_1);
				rPacket.put8(_BV(1)|_BV(2));
				return true;
			case 102: //extruder on backwards
				initToolCommand(rPacket,SLAVE_CMD_TOGGLE_MOTOR_1);
				rPacket.put8(_BV(1));
				return true;
			case 103: //extruder off
				initToolCommand(rPacket,SLAVE_CMD_TOGGLE_MOTOR_1);
				rPacket.put8(0);
				return true;
			case 104: //set heater temp
			{
				uint32_t t = 0;
				
				if (hasCode('S'))
					t = (uint32_t)getCodeFloat();
				
				initToolCommand(rPacket,SLAVE_CMD_SET_TEMP);
				rPacket.put16((uint16_t)t);
				return true;
			}
			case 106: //fan on
				initToolCommand(rPacket,SLAVE_CMD_TOGGLE_FAN);
				rPacket.put8(_BV(0));
				return true;
			case 107: //fan off
				initToolCommand(rPacket,SLAVE_CMD_TOGGLE_FAN);
				rPacket.put8(0);
				return true;
				
			case 108: //extruder speed
			{
				uint32_t t = 0;
				
				if (hasCode('S'))
					t = (uint32_t)getCodeFloat();

				initToolCommand(rPacket,SLAVE_CMD_SET_MOTOR_1_PWM);
				rPacket.put32(t);
				return true;
			}
			case 109: //set hbp temp
			{
				uint32_t t = 0;
				
				if (hasCode('S'))
					t = (uint32_t)getCodeFloat();
				
				initToolCommand(rPacket,SLAVE_CMD_SET_PLATFORM_TEMP);
				rPacket.put16((uint16_t)t);
				return true;
			}
			
			
			
		}
	}
	return false;
}

void GCodeParser::getCoordinates(Point& rPoint)
{
	if (hasCode('X'))
		rPoint.x = getCodeFloat();
	else
		rPoint.x = mPreviousPoint.x;

	if (hasCode('Y'))
		rPoint.y = getCodeFloat();
	else
		rPoint.y = mPreviousPoint.y;
		
	if (hasCode('Z'))
		rPoint.z = getCodeFloat();
	else
		rPoint.z = mPreviousPoint.z;
	
	mPreviousPoint.set(rPoint);
	
	if (!mbMetric)
	{
		rPoint.x = inchesToMM(rPoint.x);
		rPoint.y = inchesToMM(rPoint.y);
		rPoint.z = inchesToMM(rPoint.z);
	}
}

void GCodeParser::initToolCommand(Packet& rPacket,uint8_t cmd)
{
	rPacket.put8(HOST_CMD_TOOL_COMMAND);
	rPacket.put8(0);
	rPacket.put8(cmd);
}
