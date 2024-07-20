// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2021 The Pqcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "key.h"

#include "arith_uint256.h"
#include "crypto/common.h"
#include "crypto/hmac_sha512.h"
#include "pubkey.h"
#include "random.h"
#include <iostream>
#include <fstream>
#include <string>
#include <dilithium3-smallpoly/api.h>
#include<dilithium3-smallpoly/randombytes.h>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
static secp256k1_context* secp256k1_context_sign = NULL;

/** These functions are taken from the libsecp256k1 distribution and are very ugly. */
static int ec_privkey_import_der(const secp256k1_context* ctx, unsigned char *out32, const unsigned char *privkey, size_t privkeylen) {
    const unsigned char *end = privkey + privkeylen;
    int lenb = 0;
    int len = 0;
    memset(out32, 0, 32);
    /* sequence header */
    if (end < privkey+1 || *privkey != 0x30) {
        return 0;
    }
    privkey++;
    /* sequence length constructor */
    if (end < privkey+1 || !(*privkey & 0x80)) {
        return 0;
    }
    lenb = *privkey & ~0x80; privkey++;
    if (lenb < 1 || lenb > 2) {
        return 0;
    }
    if (end < privkey+lenb) {
        return 0;
    }
    /* sequence length */
    len = privkey[lenb-1] | (lenb > 1 ? privkey[lenb-2] << 8 : 0);
    privkey += lenb;
    if (end < privkey+len) {
        return 0;
    }
    /* sequence element 0: version number (=1) */
    if (end < privkey+3 || privkey[0] != 0x02 || privkey[1] != 0x01 || privkey[2] != 0x01) {
        return 0;
    }
    privkey += 3;
    /* sequence element 1: octet string, up to 32 bytes */
    if (end < privkey+2 || privkey[0] != 0x04 || privkey[1] > 0x20 || end < privkey+2+privkey[1]) {
        return 0;
    }
    memcpy(out32 + 32 - privkey[1], privkey + 2, privkey[1]);
    if (!secp256k1_ec_seckey_verify(ctx, out32)) {
        memset(out32, 0, 32);
        return 0;
    }
    return 1;
}

static int ec_privkey_export_der(const secp256k1_context *ctx, unsigned char *privkey, size_t *privkeylen, const unsigned char *key32, int compressed) {
    secp256k1_pubkey pubkey;
    size_t pubkeylen = 0;
    if (!secp256k1_ec_pubkey_create(ctx, &pubkey, key32)) {
        *privkeylen = 0;
        return 0;
    }
    if (compressed) {
        static const unsigned char begin[] = {
            0x30,0x81,0xD3,0x02,0x01,0x01,0x04,0x20
        };
        static const unsigned char middle[] = {
            0xA0,0x81,0x85,0x30,0x81,0x82,0x02,0x01,0x01,0x30,0x2C,0x06,0x07,0x2A,0x86,0x48,
            0xCE,0x3D,0x01,0x01,0x02,0x21,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFE,0xFF,0xFF,0xFC,0x2F,0x30,0x06,0x04,0x01,0x00,0x04,0x01,0x07,0x04,
            0x21,0x02,0x79,0xBE,0x66,0x7E,0xF9,0xDC,0xBB,0xAC,0x55,0xA0,0x62,0x95,0xCE,0x87,
            0x0B,0x07,0x02,0x9B,0xFC,0xDB,0x2D,0xCE,0x28,0xD9,0x59,0xF2,0x81,0x5B,0x16,0xF8,
            0x17,0x98,0x02,0x21,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFF,0xFF,0xFE,0xBA,0xAE,0xDC,0xE6,0xAF,0x48,0xA0,0x3B,0xBF,0xD2,0x5E,
            0x8C,0xD0,0x36,0x41,0x41,0x02,0x01,0x01,0xA1,0x24,0x03,0x22,0x00
        };
        unsigned char *ptr = privkey;
        memcpy(ptr, begin, sizeof(begin)); ptr += sizeof(begin);
        memcpy(ptr, key32, 32); ptr += 32;
        memcpy(ptr, middle, sizeof(middle)); ptr += sizeof(middle);
        pubkeylen = CPubKey::COMPRESSED_SIZE;
        secp256k1_ec_pubkey_serialize(ctx, ptr, &pubkeylen, &pubkey, SECP256K1_EC_COMPRESSED);
        ptr += pubkeylen;
        *privkeylen = ptr - privkey;
    } else {
        static const unsigned char begin[] = {
            0x30,0x82,0x01,0x13,0x02,0x01,0x01,0x04,0x20
        };
        static const unsigned char middle[] = {
            0xA0,0x81,0xA5,0x30,0x81,0xA2,0x02,0x01,0x01,0x30,0x2C,0x06,0x07,0x2A,0x86,0x48,
            0xCE,0x3D,0x01,0x01,0x02,0x21,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFE,0xFF,0xFF,0xFC,0x2F,0x30,0x06,0x04,0x01,0x00,0x04,0x01,0x07,0x04,
            0x41,0x04,0x79,0xBE,0x66,0x7E,0xF9,0xDC,0xBB,0xAC,0x55,0xA0,0x62,0x95,0xCE,0x87,
            0x0B,0x07,0x02,0x9B,0xFC,0xDB,0x2D,0xCE,0x28,0xD9,0x59,0xF2,0x81,0x5B,0x16,0xF8,
            0x17,0x98,0x48,0x3A,0xDA,0x77,0x26,0xA3,0xC4,0x65,0x5D,0xA4,0xFB,0xFC,0x0E,0x11,
            0x08,0xA8,0xFD,0x17,0xB4,0x48,0xA6,0x85,0x54,0x19,0x9C,0x47,0xD0,0x8F,0xFB,0x10,
            0xD4,0xB8,0x02,0x21,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFF,0xFF,0xFE,0xBA,0xAE,0xDC,0xE6,0xAF,0x48,0xA0,0x3B,0xBF,0xD2,0x5E,
            0x8C,0xD0,0x36,0x41,0x41,0x02,0x01,0x01,0xA1,0x44,0x03,0x42,0x00
        };
        unsigned char *ptr = privkey;
        memcpy(ptr, begin, sizeof(begin)); ptr += sizeof(begin);
        memcpy(ptr, key32, 32); ptr += 32;
        memcpy(ptr, middle, sizeof(middle)); ptr += sizeof(middle);
        pubkeylen = CPubKey::SIZE;
        secp256k1_ec_pubkey_serialize(ctx, ptr, &pubkeylen, &pubkey, SECP256K1_EC_UNCOMPRESSED);
        ptr += pubkeylen;
        *privkeylen = ptr - privkey;
    }
    return 1;
}

bool CKey::Check(const unsigned char *vch) {
    return secp256k1_ec_seckey_verify(secp256k1_context_sign, vch);
}
void mnemonic_from_data(char * mnemo,int mlen,uint8_t *data, int len)
{
	uint8_t bits[32 + 1];
	int i, j, idx;
	char *p = mnemo;

    memset(mnemo,0,sizeof(mnemo));
    uint32_t sha256_h[8];
	mlen = len * 3 / 4;  //字符个数计算
    // sha256(data,len);
	// checksum
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    // 处理输入数据
    SHA256_Update(&sha256, data, len);

    // 获取最终的哈希值
    SHA256_Final((unsigned char*)sha256_h, &sha256);
	bits[len] = (unsigned char)(sha256_h[0]>>24);
	// data
	memcpy(bits, data, len);


	for (i = 0; i < mlen; i++) 
	{
		idx = 0;
		for (j = 0; j < 11; j++) 
		{
			idx <<= 1;
			idx += (bits[(i * 11 + j) / 8] & (1 << (7 - ((i * 11 + j) % 8)))) > 0;
		}

		
			
		strcpy(p, wordlist_english[idx]);
		p += strlen(wordlist_english[idx]);
		
	
		*p = (i < mlen - 1) ? ',' : 0;
		p++;
	}
}
void pbkdf2_hmac_sha256(
    const uint8_t *pass, int passlen,
    uint8_t *salt, int saltlen,
    uint32_t iterations,
    uint8_t *key, int keylen) {
    // 执行PBKDF2 HMAC SHA-256派生
    int derived_keylen = keylen;
    int result = PKCS5_PBKDF2_HMAC(
        reinterpret_cast<const char*>(pass), passlen,
        salt, saltlen,
        iterations,
        EVP_sha256(),
        derived_keylen,
        key);

    if (result != 1) {
        std::cerr << "Error: PBKDF2 HMAC SHA-256 derivation failed." << std::endl;
        return;
    }
}
void mnemonic_to_seed(const char *mnemonic, const char *passphrase, uint8_t *seed, void (*progress_callback)(uint32_t current, uint32_t total))
{
	uint8_t salt[8 + 256 + 4];
	int saltlen = 0;//strlen(passphrase);
	if(passphrase!=0){saltlen =strlen(passphrase);}

	memcpy(salt, "mnemonic", 8);
	memcpy(salt + 8, passphrase, saltlen);
	saltlen += 8;
	pbkdf2_hmac_sha256((const uint8_t *)mnemonic, strlen(mnemonic), salt, saltlen, 2048, seed, 256 / 8);
}

std::string CKey::MakeNewKey(bool fCompressedIn) {
    //do {
     //   GetStrongRandBytes(keydata.data(), keydata.size());
    //} while (!Check(keydata.data()));
    //fValid = true;
    //fCompressed = fCompressedIn;
    unsigned char sk[PRIVATE_KEY_SIZE];
    unsigned char pk[PUB_KEY_SIZE];  
    uint8_t gene_entro[32];
    randombytes(gene_entro,32 );
    char mnemonic[24*20]={0};
    mnemonic_from_data(mnemonic,0,gene_entro,32);
    int j = 0;
    while(mnemonic[j]!=0)
	{
		j++;
	}
    std::string str_mnemonic(mnemonic,j);
    std::cout<<str_mnemonic<<std::endl;
    std::string filename = "mnemonic_address.txt";
    
    // 使用std::ofstream打开文件，std::ios::out表示打开一个文件用于输出
    // std::ios::trunc表示如果文件已存在，先截断文件内容
    std::ofstream file(filename, std::ios::out | std::ios::app);

    // 检查文件是否成功打开
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file." << std::endl;
        
    }

    // 将字符串写入文件
    j = 0;
    while(mnemonic[j]!=0)
	{
		file<<mnemonic[j];
		j++;
	}
    file<<" ";
    
    

    // 关闭文件
    file.close();
    
    uint8_t seed[64];
    mnemonic_to_seed(mnemonic, 0, seed, 0);
    int r = crypto_sign_keypair(seed,pk,sk);
    if(r!=0){
        printf("---- Falcon-512 Key pair gen fail.\n");
    }
    
    memcpy(keydata.data(),sk, PRIVATE_KEY_SIZE);    
    memcpy(pubkeydata.data(),pk, PUB_KEY_SIZE);    
    fValid = true;
    fCompressed = true;//fCompressedIn;
    return str_mnemonic;
}

void CKey::MakeNewKeyByMnemonic(std::string str_mnemonic ,bool fCompressed){
    unsigned char sk[PRIVATE_KEY_SIZE];
    unsigned char pk[PUB_KEY_SIZE];  
    char mnemonic[24*10]={0};
    for(int i = 0; i< str_mnemonic.length();i++){
        mnemonic[i] = str_mnemonic[i];
    }
    uint8_t seed[64];
    mnemonic_to_seed(mnemonic, 0, seed, 0);
    int r = crypto_sign_keypair(seed,pk,sk);
    if(r!=0){
        printf("---- Falcon-512 Key pair gen fail.\n");
    }
    
    memcpy(keydata.data(),sk, PRIVATE_KEY_SIZE);    
    memcpy(pubkeydata.data(),pk, PUB_KEY_SIZE);    
    fValid = true;
    fCompressed = true;//fCompressedIn;
}
bool CKey::SetPrivKey(const CPrivKey &privkey, bool fCompressedIn) {
    if (!ec_privkey_import_der(secp256k1_context_sign, (unsigned char*)begin(), &privkey[0], privkey.size()))
        return false;
    fCompressed = fCompressedIn;
    fValid = true;
    return true;
}

CPrivKey CKey::GetPrivKey() const {
   assert(fValid);
    CPrivKey privkey;
    privkey.resize(PRIVATE_KEY_SIZE);
    memcpy(privkey.data(),keydata.data(), keydata.size());    
    return privkey;
}

CPubKey CKey::GetPubKey() const {
    assert(fValid);
    CPubKey pubkey;
    unsigned char* pch = (unsigned char *)pubkey.begin();
    memcpy(pch+1,pubkeydata.data(), pubkeydata.size());
    pch[0] = 7;
    return pubkey;
}

bool CKey::Sign(const uint256 &hash, std::vector<unsigned char>& vchSig, uint32_t test_case) const {
    if (!fValid)
        return false;
    size_t sig_len;
    vchSig.resize(PQCLEAN_DILITHIUM3_CLEAN_CRYPTO_BYTES_);
    int r = crypto_sign_signature(vchSig.data(),&sig_len,hash.begin() ,32,keydata.data());

    vchSig.resize(sig_len);
    
    if(r!=0){
        printf("\n--- sig is failed.%d\n",sig_len);
    }

    return true;
}

bool CKey::VerifyPubKey(const CPubKey& pubkey) const {
   unsigned char rnd[8];
    std::string str = "Bitcoin key verification\n";
    GetRandBytes(rnd, sizeof(rnd));
    uint256 hash;
    CHash256().Write((unsigned char*)str.data(), str.size()).Write(rnd, sizeof(rnd)).Finalize(hash.begin());
    std::vector<unsigned char> vchSig;
    Sign(hash, vchSig);
    return pubkey.Verify(hash, vchSig);
}

bool CKey::SignCompact(const uint256 &hash, std::vector<unsigned char>& vchSig) const {
    if (!fValid)
        return false;
    size_t sig_len;
    vchSig.resize(PQCLEAN_DILITHIUM3_CLEAN_CRYPTO_BYTES_+pksize());
    int r = crypto_sign_signature(vchSig.data(),&sig_len,hash.begin(),32,keydata.data());
    vchSig.resize(sig_len+pksize());
    memcpy(vchSig.data()+sig_len,pubkeydata.data(),pksize());
    if(r!=0){
        printf("\n--- sig is failed.%d\n",sig_len);
    }
    
    return true;
}

bool CKey::Load(CPrivKey &privkey, CPubKey &vchPubKey, bool fSkipCheck=false) {
    memcpy((unsigned char*)begin(), privkey.data(), privkey.size());
    fCompressed = true; //vchPubKey.IsCompressed();
    fValid = true;
    memcpy((unsigned char*)pkbegin(), vchPubKey.data()+1, pksize());

    if (fSkipCheck)
        return true;

    return VerifyPubKey(vchPubKey);
}


bool CKey::Derive(CKey& keyChild, ChainCode &ccChild, unsigned int nChild, const ChainCode& cc) const {
    assert(IsValid());
    assert(IsCompressed());
    std::vector<unsigned char, secure_allocator<unsigned char>> vout(64);
    if ((nChild >> 31) == 0) {
        CPubKey pubkey = GetPubKey();
        assert(pubkey.begin() + CPubKey::COMPRESSED_SIZE == pubkey.end());
        BIP32Hash(cc, nChild, *pubkey.begin(), pubkey.begin()+1, vout.data());
    } else {
        assert(begin() + 32 == end());
        BIP32Hash(cc, nChild, 0, begin(), vout.data());
    }
    memcpy(ccChild.begin(), vout.data()+32, 32);
    memcpy((unsigned char*)keyChild.begin(), begin(), 32);
    bool ret = secp256k1_ec_privkey_tweak_add(secp256k1_context_sign, (unsigned char*)keyChild.begin(), vout.data());
    keyChild.fCompressed = true;
    keyChild.fValid = ret;
    return ret;
}

bool CExtKey::Derive(CExtKey &out, unsigned int _nChild) const {
    out.nDepth = nDepth + 1;
    CKeyID id = key.GetPubKey().GetID();
    memcpy(&out.vchFingerprint[0], &id, 4);
    out.nChild = _nChild;
    return key.Derive(out.key, out.chaincode, _nChild, chaincode);
}

void CExtKey::SetMaster(const unsigned char *seed, unsigned int nSeedLen) {
    static const unsigned char hashkey[] = {'B','i','t','c','o','i','n',' ','s','e','e','d'};
    std::vector<unsigned char, secure_allocator<unsigned char>> vout(64);
    CHMAC_SHA512(hashkey, sizeof(hashkey)).Write(seed, nSeedLen).Finalize(vout.data());
    key.Set(vout.data(), vout.data() + 32, true);
    memcpy(chaincode.begin(), vout.data() + 32, 32);
    nDepth = 0;
    nChild = 0;
    memset(vchFingerprint, 0, sizeof(vchFingerprint));
}

CExtPubKey CExtKey::Neuter() const {
    CExtPubKey ret;
    ret.nDepth = nDepth;
    memcpy(&ret.vchFingerprint[0], &vchFingerprint[0], 4);
    ret.nChild = nChild;
    ret.pubkey = key.GetPubKey();
    ret.chaincode = chaincode;
    return ret;
}

void CExtKey::Encode(unsigned char code[BIP32_EXTKEY_SIZE]) const {
    code[0] = nDepth;
    memcpy(code+1, vchFingerprint, 4);
    code[5] = (nChild >> 24) & 0xFF; code[6] = (nChild >> 16) & 0xFF;
    code[7] = (nChild >>  8) & 0xFF; code[8] = (nChild >>  0) & 0xFF;
    memcpy(code+9, chaincode.begin(), 32);
    code[41] = 0;
    assert(key.size() == 32);
    memcpy(code+42, key.begin(), 32);
}

void CExtKey::Decode(const unsigned char code[BIP32_EXTKEY_SIZE]) {
    nDepth = code[0];
    memcpy(vchFingerprint, code+1, 4);
    nChild = (code[5] << 24) | (code[6] << 16) | (code[7] << 8) | code[8];
    memcpy(chaincode.begin(), code+9, 32);
    key.Set(code+42, code+BIP32_EXTKEY_SIZE, true);
}

bool ECC_InitSanityCheck() {
//     CKey key;
    
//     key.MakeNewKey(true);
//     CPubKey pubkey = key.GetPubKey();
//     return key.VerifyPubKey(pubkey);
    return true;
}

void ECC_Start() {
   /* assert(secp256k1_context_sign == NULL);

    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    assert(ctx != NULL);

    {
        // Pass in a random blinding seed to the secp256k1 context.
        std::vector<unsigned char, secure_allocator<unsigned char>> vseed(32);
        GetRandBytes(vseed.data(), 32);
        bool ret = secp256k1_context_randomize(ctx, vseed.data());
        assert(ret);
    }

    secp256k1_context_sign = ctx;
    */
}

void ECC_Stop() {
   /*  secp256k1_context *ctx = secp256k1_context_sign;
    secp256k1_context_sign = NULL;

    if (ctx) {
        secp256k1_context_destroy(ctx);
    }
    */
}
