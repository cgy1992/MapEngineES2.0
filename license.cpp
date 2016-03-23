#include "license.h"
#include "time.h"
int license::EncryptionBase64(std::string theSource, std::string& theDesc)
{
	std::string theCode = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";
	theSource += ".";
	int theSourceLenght = theSource.length();
	// 加字符 构成3的整数倍
	int incIndex = theSource.length() % 3;
	switch(incIndex)
	{
	case 0: 
		break;
	case 1: 
		theSource += theCode[clock() % theCode.length()];
		theSource += theCode[clock() % theCode.length()]; 
		break;
	case 2: 
		theSource += theCode[clock() % theCode.length()];
		break;
	default: 
		break;
	}
	theDesc = "";
	int i = 0;
	for(; i < theSourceLenght; i += 3)
	{
		int temp = theSource[i];
		temp = temp << 8;
		temp += theSource[i + 1];
		temp = temp << 8;
		temp += theSource[i + 2];
		char theChar[4] = {0};
		for(int j = 3; j >= 0; j--)
		{
			theChar[j] = temp & 0x0000003f;
			theChar[j] = theChar[j] | 0xc0;
			temp = temp >> 6;
		}
		for(int i = 0; i < 4; i++)
			theDesc += theChar[i];
	}
	return true;
}

int license::DecryptionBase64(const std::string& theSource, std::string& theDesc)
{
	int theSourceLength = theSource.length();
	theDesc = "";
	if(theSource.length() % 4)
		return false;
	for(int i = 0; i < theSourceLength; i += 4)
	{
		int temp = 0;
		char theChar[3] = {0};
		for(int j = i; j < i + 4; j++)
		{
			temp = temp << 6;
			char tempChar = theSource[j] & 0x3f;
			temp += tempChar;
		}
		for(int j = 2; j >= 0; j--)
		{
			theChar[j] = temp & 0x000000ff;
			temp = temp >> 8;
		}
		for(int j = 0; j < 3; j++)
			theDesc += theChar[j];
	}
	int pos = theDesc.rfind(".");
	theDesc = theDesc.substr(0, pos);
	return true;
}

int license::Encryption(std::string theSource, std::string& theDesc)
{
	return EncryptionBase64(theSource, theDesc);
}

int license::Decryption(const std::string& theSource, std::string& theDesc)
{
	return DecryptionBase64(theSource, theDesc);
}

bool license::stringIsNumber(std::string theString, int& Number)
{
	Number = 0;
	for(int i = 0; i < theString.length(); i++)
	{
		if(theString[i] >= '0' && theString[i] <= '9')
		{
			Number *= 10;
			Number = Number + theString[i] - '0';
		}
		else
			return false;
	}
	return true;
}

bool license::GetTheTime(std::string theDataTime, int& year, int& month, int& day)
{
	int ipos = theDataTime.find(".");
	if(ipos <= 0)
		return false;
	else
	{
		// year
		std::string theTempTime = theDataTime.substr(0, ipos);
		theDataTime = theDataTime.substr(ipos + 1, theDataTime.length());
		if(stringIsNumber(theTempTime, year))
		{
			ipos = theDataTime.find(".");
			if(ipos <= 0)
				return false;
			else
			{
				// month
				theTempTime = theDataTime.substr(0, ipos);
				theDataTime = theDataTime.substr(ipos + 1, theDataTime.length());
				if(stringIsNumber(theTempTime, month))
				{
					// daty
					if(stringIsNumber(theDataTime, day))
					{

					}
					else
						return false;
				}
				else
					return false;
			}
		}
		else
			return false;
	}
	if(month > 12 || day > 31)
		return false;
	return true;
}

//只要 stringCurrent < stringlicense 就表示注册过
bool license::isRegister(std::string stringlicense, std::string stringCurrent)
{
	int year1 = 0;
	int month1 = 0;
	int day1 = 0;		// 授权的
	int year2 = 0;
	int month2 = 0;
	int day2 = 0;
	if(GetTheTime(stringlicense, year2, month2, day2))
	{
		if(GetTheTime(stringCurrent, year1, month1, day1))
		{
			if(year2 - year1 > 10) //10年之内有效
				return false;
			if(year2 < year1)
				return false;
			else if(year2 > year1)
			{
				return true;
			}
			else
			{
				if(month2 < month1)
					return false;
				else if(month2 > month1)
					return true;
				else
				{
					if(day2 < day1)
						return false;
					else
						return true;
				}
			}
		}
	}
	return false;
}