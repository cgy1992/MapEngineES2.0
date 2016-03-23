#ifndef _LINCESE_H_
#define _LINCESE_H_
#include "stdafx.h"

class license
{
private: 
	int EncryptionBase64(std::string theSource, std::string& theDesc);
	int DecryptionBase64(const std::string& theSource, std::string& theDesc);
public:
	// 加密
	int Encryption(std::string theSource, 				// 原文
					std::string& theDesc						// 密文
					);
	// 解密
	int Decryption(const std::string& theSource, 				// 密文
					std::string& theDesc						// 原文
					);
	bool isRegister(std::string stringlicense, 
					std::string stringCurrent);
private:
	bool GetTheTime(std::string theDataTime, int& year, int& month, int& day);

	bool stringIsNumber(std::string theString, int& Number);
};
#endif