#ifndef _AESCODER_H
#define _AESCODER_H

#include <iostream>
#include <string>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

/*
类名：AESCoder
入口：无
出口：加密解密
*/
class AESCoder
{
private:
    static const char* m_key;
    static const char* m_iv;

private:
    static void char_2_byte(byte* bytes, const char* str);

public:
    static std::string aes_encrypt(const std::string& plaintext);
    static std::string aes_decrypt(const std::string& ciphertext);
};


#endif