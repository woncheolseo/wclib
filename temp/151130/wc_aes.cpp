/**
	@file wc_aes.cpp
   	@brief WCAes 소스
*/

#include "wc_lib.h"

WCAes::WCAes()
{
	WCAes("coderay");
	/*unsigned int salt[] = {12345, 54321};
	
    const unsigned char *key_data = (const unsigned char *)key.c_str();
    int key_data_len = strlen(key.c_str());

    if (!aes_init(key_data, key_data_len, (unsigned char *)&salt)) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"Couldn't initialize AES cipher\n");
	}*/
}

WCAes::WCAes(const WCAes &Other) : WCCryptograph(Other)
{
}

WCAes& WCAes::operator=(const WCAes &Other)
{
	if (this != &Other) {
		WCCryptograph::operator =(Other);
	}
	return *this;
}

WCAes::WCAes(const string& key)
{
	unsigned int salt[] = {12345, 54321};
	
    const unsigned char *key_data = (const unsigned char *)key.c_str();
    int key_data_len = strlen(key.c_str());

    if (!aes_init(key_data, key_data_len, (unsigned char *)&salt)) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"Couldn't initialize AES cipher\n");
	}
}

WCAes::~WCAes() 
{
	EVP_CIPHER_CTX_cleanup(&m_evpEnc);
	EVP_CIPHER_CTX_cleanup(&m_evpDec);
}

bool WCAes::aes_init(const unsigned char *key_data, int key_data_len, unsigned char *salt)
{
        int i, nrounds = 5;
        unsigned char key[32], iv[32];

        i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv);
        if (i != 32) {
                printf("Key size is %d bits - should be 256 bits\n", i);
                return false;
        }

        EVP_CIPHER_CTX_init(&m_evpEnc);
        EVP_EncryptInit_ex(&m_evpEnc, EVP_aes_256_cbc(), NULL, key, iv);
        EVP_CIPHER_CTX_init(&m_evpDec);
        EVP_DecryptInit_ex(&m_evpDec, EVP_aes_256_cbc(), NULL, key, iv);

        return true;
}


unsigned char * WCAes::Encrypt(const unsigned char *pcPlain, int *pnSize)
{ 
	int nLen1, nLen2;
	unsigned char *pcCipher;

	nLen1 = *pnSize + AES_BLOCK_SIZE;
	nLen2 = 0;
	pcCipher = (unsigned char*)malloc(nLen1);

	if (pcCipher)
	{
		EVP_EncryptInit_ex(&m_evpEnc, NULL, NULL, NULL, NULL);
		EVP_EncryptUpdate(&m_evpEnc, pcCipher, &nLen1, pcPlain, *pnSize);
	    EVP_EncryptFinal_ex(&m_evpEnc, pcCipher+nLen1, &nLen2);
		*pnSize = nLen1 + nLen2;
	}
    return pcCipher;
}

unsigned char * WCAes::Decrypt(const unsigned char *ciphertext, int *len)
{
        int p_len = *len, f_len = 0;
        unsigned char *plaintext = (unsigned char*)malloc(p_len);

        EVP_DecryptInit_ex(&m_evpDec, NULL, NULL, NULL, NULL);
        EVP_DecryptUpdate(&m_evpDec, plaintext, &p_len, ciphertext, *len);
		EVP_DecryptFinal_ex(&m_evpDec, plaintext+p_len, &f_len);

        *len = p_len + f_len;
        return plaintext;
}
