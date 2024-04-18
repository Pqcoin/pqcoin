// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2022 The Dogecoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"
#include "arith_uint256.h"
static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    // printf("%d\n",genesis.nTime);
    // printf("%d\n",genesis.nBits);
    // printf("%d\n",genesis.nNonce);
    // printf("%d\n",genesis.nVersion);
    // printf("%s\n",genesis.hashMerkleRoot.ToString().c_str());
    // LogPrintf("hashMerkleRoot******** %s\n", genesis.hashMerkleRoot);
    // LogPrint(genesis.nTime);
    // LogPrint(genesis.nBits);
    // LogPrint(genesis.nNonce);
    // LogPrint(genesis.nVersion);
    // LogPrint(genesis.hashMerkleRoot);

    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1386325540, nBits=0x1e0ffff0, nNonce=99943, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "pqc nb";
//    CBitcoinAddress coinbaseaddress = "DR3MvNbWaKUQ6gN2XTGxbXvtzr795e5ugx";
//    const CScript genesisOutputScript = GetScriptForDestination(coinbaseaddress);
    const CScript genesisOutputScript = CScript() << ParseHex("07df3e810da0ad06f03e83f460bd938173de04bda27b967e7e43e541ef3db056c06ff0e0e9531a9a74385bd50e3c824871f0a3a4d357582eff7b4e3f172a87f36e795a87c5b89dd0ec708159b17b6ba71682023e7ab99a04db6b4ccb65c75d3fa070ea573fb8cd9cc89499fd005b00495d5176bcec25a7ecbaa7bed4fbeaef1814cc4158f16c0042c0e7740537db7cf33458ae919da567a101d1446ea3732aed9652d12de47e4573e85a03506f6d3e40b446d9d66953c04a77d568d69599456f64d0970b9daa413b18a32b4bb3f21e22895c44397d516e8d920b77875d5241e79e6b8260bd0452e19c1db626672beac27d93d954e33bbc07de20b1ad3eb15e61e3b383b9cd6d088f702232d669faba84e4f88ce85f9291ecfaf33d9a4fdc97434f9cfe4f112b54383a0d11e3aacea67740e01c0685d696e1b91d4bf55e62615acaab96c7ddcb87716ce8903dba227f00c2fd31b0c7dcdd4d835ea98975d455392b0c0de16d5c3e3e5610e9f01eae90478d9d0020a2d8e992c7ee03fd7c6d21898a142082a47ba8f285729826b7c0832571e2cbc0998154c7750d613a4a93e952f98c5fdd4a43389f6fcb5c964e5d02332041e62e3339d3fefca82a84025d6e76996d572d8faba9e1d38e40adbf23685efc393c109517918ee9972d4f8b32f72b77fb17d6da465d87ea26d0aed74af98d482ebc5ffd8a1d121b5f43340be20fe36528bd78425c0c0e48e9841078d97e9596fc04ae41d64e7a106e60202a752d8df0b623cdb06f19a223e90cbfb80f94e9a023d1d2eb2d4769ece83f53c3d36ecbf4a1c95b3e18e294d3912680ebe43a977068b20d128e335c6a1d6b50760a74591a9490f6384c3e81ffb23c8d7cff55a6a6dd23bafc7ff6a244b6d10682b69413c69c827ae63134377b94413897706788e33005e3789685f95aea44a0b4b817a33895b42ba763e7b587bdfb0dfc069aec109d11cfd5aac40b9517752cebcf350137145ad90473ea55472c204901c008bcd97c547bee2891a690941b5cffe9e6d7d53cfcb48dcb1bf3ac3cda4f1dd18d5d9684bd1ea02f86de1530a3721477b993312f2859e316346a38f65b06752de1ae3dc6e1d2775c11dedc10e09579f87d1149f5b900db0ba010a65b6bdec232f1353b2b80fac84e3784fe044cecd8d07e02856599bb69d3b9d75d3ae21ede3ddbb2d4468c3dc3348e9f8de63958ddae36f09e3d8316460e4e3310830fcf0b96be44077fb27d8751b0aa46432174f62ed55f18b6a3c5c7be61c1190849bc497b75631668274f687cfedcceb5e4c3383be23731cda8251bf797f505b5e75bdf63e8ae4d58e34d78f3347fd035a8fda55ee2d7ff896aa7cee812da0c8708cbaf495863d9d73e70274a21b3e64c136a8c4a2c3331d7c453e3d2c235467d684644a58aee3b127b559bcef717fe3c3d1a54117e3cffd64f4c11e738d9bb564390726a051bb9cd87e9e4e33475f81ff1aeae6166a8084a45b6fa0ed3d80cba372bf4b7b135688e977e3814fcc202299a9e7bc38f61a8a2fde06504830007bb95f3888cb0f7be8dafcd040425ac0717426d81d26b0fe3c812efd0b32fb20d06331dd0d0eb18d43c2a365de841d8e7b65effa7e8ded61861d55414b98b11080955d5a4a04f8460129bd191945d8a7aa0cbc50f50dd5a17de6feeb8cd16f4e3a052f5ea246119de808a0ce85816ee1d4f49b175f2fd91cae83f81f07294d1b8b61c40200f764ce93f57260de8f68798b3f37c389d1e04e866e06d5849c8edd7f72def1e16813fc5d175fbae67f4aafce898ef49d77cf0b74c31a9e66cb0c69cf3cdd93065e47865f84ced19fdbcb4fff2aa02ef26db2459dec561e93e9ae8d2268f163db8727b71e2f39d56cce43d532660e273b7f99fb49fb36bd66bbc5c00d4dac8adf977c69bdac20c247a5484964e65bed52d6d460fc5df0eda028785d0c0bfdb165f6f108e7d45504bc6a7884d84835b23d049dba91b114f0d776f704f370f0ffb8fab4af65b3f36d867350fd87ade554166b0d62cee5fc2dd17d38c7d8285206a89ed6bd9ce330ff7cbbcdb3764889bd7ca190f51570b057f3b5813483a8476876c43b9743d2b875a11456aa527af50c6da4d926db5dae2f5902f9af9335a26417e9c1655370f389061612d803279a5f17f24e92a3993f94bfedc480b32ac093aa1baf53ace94ac048e3560251c87cd1b42f2b654d17fa5dee613172b81676108fc4570addc5f2d7f352058221075f833027b5a5194be097ec4f59d4b84cb2a69fd68ffae6ff49f3a74ece6b46cfa0a60adc829752d479cecac1d17b7dda2e6741dd2d4045e5d421a8969c5fa615570026e88354cdbe116b5af3dd0b9c07965307b0091bef3c09986c76e79ea77c9edb8fe39b39a0cd4d3d3587bafa7b2468d55e6e360744b8d1c660889dfd2378e3589872be850c713194680ddb978331244dde41f5695fafa91808a82cbbf47b1f0f3c073c37787ced860365420d3d5ea4acfe17539dadad56d8300f98c88c9af65944efe3a8879dafec1f6b2f74152b45b0a2fef29b08469779ab30dd148301d67225d5717f8f774c16340a4794629a2ee1b7fb56bef94357bdd4c14ce58f2ee79ebc3c6c809b5eb8f85559d5832ce29612f62a224ce782bc2f34670db9b14819aa115a33a390eed79ac1d27ab18a7be0b62ebdf031efaecd49e86313e7ab2de22be80e09c810978c681a88322774f3ffdf0a91dece6e719b31842f65ba4fce4a6f95bf3bc72") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
private:
    Consensus::Params digishieldConsensus;
    Consensus::Params auxpowConsensus;
public:
    CMainParams() {
        strNetworkID = "main";

        // Blocks 0 - 144999 are conventional difficulty calculation
        consensus.nSubsidyHalvingInterval = 420000;
        consensus.nMajorityEnforceBlockUpgrade = 1500;
        consensus.nMajorityRejectBlockOutdated = 1900;
        consensus.nMajorityWindow = 2000;
        // BIP34 is never enforced in Dogecoin v2 blocks, so we enforce from v3
        consensus.BIP34Height = 1034383;
        consensus.BIP34Hash = uint256S("0x80d1364201e5df97e696c03bdd24dc885e8617b9de51e453c10a4f629b1e797a");
        consensus.BIP65Height = 3464751; // 34cd2cbba4ba366f47e5aa0db5f02c19eba2adf679ceb6653ac003bdc9a0ef1f - first v4 block after the last v3 block
        consensus.BIP66Height = 1034383; // 80d1364201e5df97e696c03bdd24dc885e8617b9de51e453c10a4f629b1e797a - this is the last block that could be v2, 1900 blocks past the last v2 block
        consensus.powLimit = uint256S("0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20;
        consensus.initPowDifficulty = uint256S("0x000010ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

        //wh_difficuly
        // consensus.nPowTargetTimespan = 4 * 60; // 
        // consensus.nPowTargetSpacing = 60; // 
        consensus.nPowTargetTimespan = 7*24*60*60;   // 2016块调整一次
        consensus.nPowTargetSpacing = 300;   // 期望300秒出一个块

        //std::cout << "aaa" << std::endl;

        consensus.fDigishieldDifficultyCalculation = false;
        consensus.nCoinbaseMaturity = 30;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowAllowDigishieldMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 9576; // 95% of 10,080
        consensus.nMinerConfirmationWindow = 10080; // 60 * 24 * 7 = 10,080 blocks, or one week

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        // XXX: BIP heights and hashes all need to be updated to Dogecoin values
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1462060800; // May 1st, 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1493596800; // May 1st, 2017

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1479168000; // November 15th, 2016.
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 0; // Disabled

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00"); // 4,303,965

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S(""); // 4,303,965

        // AuxPoW parameters
        consensus.nAuxpowChainId = 0x0062; // 98 - Josh Wise!
        consensus.fStrictChainId = true;
        consensus.fAllowLegacyBlocks = true;
        consensus.nHeightEffective = 0;

        // Blocks 145000 - 371336 are Digishield without AuxPoW
        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 0;
        digishieldConsensus.fSimplifiedRewards = true;
        digishieldConsensus.fDigishieldDifficultyCalculation = true;

        //wh_nPowTargetTimespan
        digishieldConsensus.nPowTargetTimespan =  7*24*60*60; // 真实的nPowTargetTimespan
        digishieldConsensus.nCoinbaseMaturity = 240;

        // Blocks 371337+ are AuxPoW
        auxpowConsensus = digishieldConsensus;
        auxpowConsensus.nHeightEffective = 0;
        auxpowConsensus.fAllowLegacyBlocks = false;

        // Assemble the binary search tree of consensus parameters
        pConsensusRoot = &digishieldConsensus;
        digishieldConsensus.pLeft = &consensus;
        digishieldConsensus.pRight = &auxpowConsensus;

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xc1;
        pchMessageStart[1] = 0xc2;
        pchMessageStart[2] = 0xc3;
        pchMessageStart[3] = 0xc4;
        nDefaultPort = 7332;
        nPruneAfterHeight = 100000;
        genesis = CreateGenesisBlock(1681098301, 1, 0x207fffff, 1, 12000000 * COIN);
//        std::cout<<genesis.GetHash().ToString()<<std::endl;

        //printf("%s\n",genesis.GetHash().ToString().c_str());
        // printf("%d\n",genesis.GetHash());
        // LogPrint(genesis);
        //  LogPrint(genesis.nTime);
        //  LogPrint(consensus.hashGenesisBlock);
    //       CBlock genesis;
    // genesis.nTime    = nTime;
    // genesis.nBits    = nBits;
    // genesis.nNonce   = nNonce;
    // genesis.nVersion = nVersion;
    // genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    // genesis.hashPrevBlock.SetNull();
    // genesis.hashMerkleRoot = BlockMerkleRoot(genesis);


        consensus.hashGenesisBlock = genesis.GetHash();
        digishieldConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        auxpowConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        assert(consensus.hashGenesisBlock == uint256S("0xa5720dd5637aed5167ccc084c19020751624e8d801ceb41d54f4477c887812b9"));
//        assert(genesis.hashMerkleRoot == uint256S("0x3e8d1c67030da19c37775237b858508f53b7867eb0b444adae944cbe0d30520f"));

        // Note that of those with the service bits flag, most only support a subset of possible options
        // vSeeds.push_back(CDNSSeedData("multidoge.org", "seed.multidoge.org", true));
        // vSeeds.push_back(CDNSSeedData("multidoge.org", "seed2.multidoge.org"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,30);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,22);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,158);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x02)(0xfa)(0xca)(0xfd).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x02)(0xfa)(0xc3)(0x98).convert_to_container<std::vector<unsigned char> >();

        // vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));
        vFixedSeeds.clear();

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = (CCheckpointData) {
        //     {

        // }
            boost::assign::map_list_of
            (      0, uint256S("0xd1fe73b87f5b2263faf24bbda99eec05b35d5d6d4eec83a9e6a87acbdf1e84e1"))
            // ( 104679, uint256S("0x35eb87ae90d44b98898fec8c39577b76cb1eb08e1261cfc10706c8ce9a1d01cf"))
            // ( 145000, uint256S("0xcc47cae70d7c5c92828d3214a266331dde59087d4a39071fa76ddfff9b7bde72"))
            // ( 371337, uint256S("0x60323982f9c5ff1b5a954eac9dc1269352835f47c2c5222691d80f0d50dcf053"))
            // ( 450000, uint256S("0xd279277f8f846a224d776450aa04da3cf978991a182c6f3075db4c48b173bbd7"))
            // ( 771275, uint256S("0x1b7d789ed82cbdc640952e7e7a54966c6488a32eaad54fc39dff83f310dbaaed"))
            // ( 1000000, uint256S("0x6aae55bea74235f0c80bd066349d4440c31f2d0f27d54265ecd484d8c1d11b47"))
            // ( 1250000, uint256S("0x00c7a442055c1a990e11eea5371ca5c1c02a0677b33cc88ec728c45edc4ec060"))
            // ( 1500000, uint256S("0xf1d32d6920de7b617d51e74bdf4e58adccaa582ffdc8657464454f16a952fca6"))
            // ( 1750000, uint256S("0x5c8e7327984f0d6f59447d89d143e5f6eafc524c82ad95d176c5cec082ae2001"))
            // ( 2000000, uint256S("0x9914f0e82e39bbf21950792e8816620d71b9965bdbbc14e72a95e3ab9618fea8"))
            // ( 2031142, uint256S("0x893297d89afb7599a3c571ca31a3b80e8353f4cf39872400ad0f57d26c4c5d42"))
            // ( 2250000, uint256S("0x0a87a8d4e40dca52763f93812a288741806380cd569537039ee927045c6bc338"))
            // ( 2510150, uint256S("0x77e3f4a4bcb4a2c15e8015525e3d15b466f6c022f6ca82698f329edef7d9777e"))
            // ( 2750000, uint256S("0xd4f8abb835930d3c4f92ca718aaa09bef545076bd872354e0b2b85deefacf2e3"))
            // ( 3000000, uint256S("0x195a83b091fb3ee7ecb56f2e63d01709293f57f971ccf373d93890c8dc1033db"))
            // ( 3250000, uint256S("0x7f3e28bf9e309c4b57a4b70aa64d3b2ea5250ae797af84976ddc420d49684034"))
            // ( 3500000, uint256S("0xeaa303b93c1c64d2b3a2cdcf6ccf21b10cc36626965cc2619661e8e1879abdfb"))
            // ( 3606083, uint256S("0x954c7c66dee51f0a3fb1edb26200b735f5275fe54d9505c76ebd2bcabac36f1e"))
            // ( 3854173, uint256S("0xe4b4ecda4c022406c502a247c0525480268ce7abbbef632796e8ca1646425e75"))
            // ( 3963597, uint256S("0x2b6927cfaa5e82353d45f02be8aadd3bfd165ece5ce24b9bfa4db20432befb5d"))
            // ( 4303965, uint256S("0xed7d266dcbd8bb8af80f9ccb8deb3e18f9cc3f6972912680feeb37b090f8cee0"))
        };

        chainTxData = ChainTxData{
            // Data as of block ed7d266dcbd8bb8af80f9ccb8deb3e18f9cc3f6972912680feeb37b090f8cee0 (height 4303965).
            // Tx estimate based on average between 2021-07-01 (3793538) and 2022-07-01 (4288126)
            1681098301, // * UNIX timestamp of last checkpoint block
            0,   // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.29        // * estimated number of transactions per second after checkpoint
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
private:
    Consensus::Params digishieldConsensus;
    Consensus::Params auxpowConsensus;
    Consensus::Params minDifficultyConsensus;
public:
    CTestNetParams() {
        strNetworkID = "test";

        // Blocks 0 - 144999 are pre-Digishield
        consensus.nHeightEffective = 0;
        consensus.nPowTargetTimespan = 4 * 60 * 60; // pre-digishield: 4 hours
        consensus.fDigishieldDifficultyCalculation = false;
        consensus.nCoinbaseMaturity = 30;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowAllowDigishieldMinDifficultyBlocks = false;
        consensus.nSubsidyHalvingInterval = 100000;
        consensus.nMajorityEnforceBlockUpgrade = 501;
        consensus.nMajorityRejectBlockOutdated = 750;
        consensus.nMajorityWindow = 1000;
        // BIP34 is never enforced in Dogecoin v2 blocks, so we enforce from v3
        consensus.BIP34Height = 708658;
        consensus.BIP34Hash = uint256S("0x21b8b97dcdb94caa67c7f8f6dbf22e61e0cfe0e46e1fff3528b22864659e9b38");
        consensus.BIP65Height = 1854705; // 955bd496d23790aba1ecfacb722b089a6ae7ddabaedf7d8fb0878f48308a71f9
        consensus.BIP66Height = 708658; // 21b8b97dcdb94caa67c7f8f6dbf22e61e0cfe0e46e1fff3528b22864659e9b38 - this is the last block that could be v2, 1900 blocks past the last v2 block
        consensus.powLimit = uint256S("0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20;
        consensus.nPowTargetTimespan = 4 * 60 * 60; // pre-digishield: 4 hours
        consensus.nPowTargetSpacing = 60; // 1 minute
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 2880; // 2 days (note this is significantly lower than Bitcoin standard)
        consensus.nMinerConfirmationWindow = 10080; // 60 * 24 * 7 = 10,080 blocks, or one week
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        // XXX: BIP heights and hashes all need to be updated to Dogecoin values
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1456790400; // March 1st, 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1493596800; // May 1st, 2017

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1462060800; // May 1st 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 0; // Disabled

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00"); // 3,976,284

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S(""); // 3,976,284

        // AuxPoW parameters
        consensus.nAuxpowChainId = 0x0062; // 98 - Josh Wise!
        consensus.fStrictChainId = false;
        consensus.nHeightEffective = 0;
        consensus.fAllowLegacyBlocks = true;

        // Blocks 145000 - 157499 are Digishield without minimum difficulty on all blocks
        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 0;
        digishieldConsensus.nPowTargetTimespan = 60; // post-digishield: 1 minute
        digishieldConsensus.fDigishieldDifficultyCalculation = true;
        digishieldConsensus.fSimplifiedRewards = true;
        digishieldConsensus.fPowAllowMinDifficultyBlocks = false;
        digishieldConsensus.nCoinbaseMaturity = 240;

        // Blocks 157500 - 158099 are Digishield with minimum difficulty on all blocks
        minDifficultyConsensus = digishieldConsensus;
        minDifficultyConsensus.nHeightEffective = 0;
        minDifficultyConsensus.fPowAllowDigishieldMinDifficultyBlocks = true;
        minDifficultyConsensus.fPowAllowMinDifficultyBlocks = true;

        // Enable AuxPoW at 158100
        auxpowConsensus = minDifficultyConsensus;
        auxpowConsensus.nHeightEffective = 0;
        auxpowConsensus.fPowAllowDigishieldMinDifficultyBlocks = true;
        auxpowConsensus.fAllowLegacyBlocks = false;

        // Assemble the binary search tree of parameters
        pConsensusRoot = &digishieldConsensus;
        digishieldConsensus.pLeft = &consensus;
        digishieldConsensus.pRight = &minDifficultyConsensus;
        minDifficultyConsensus.pRight = &auxpowConsensus;

        pchMessageStart[0] = 0xfd;
        pchMessageStart[1] = 0xc2;
        pchMessageStart[2] = 0xb8;
        pchMessageStart[3] = 0xdd;
        nDefaultPort = 18686;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1681098301, 0, 0x207fffff, 1, 88 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        digishieldConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        minDifficultyConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        auxpowConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
//        assert(consensus.hashGenesisBlock == uint256S("0xd1fe73b87f5b2263faf24bbda99eec05b35d5d6d4eec83a9e6a87acbdf1e84e1"));
//        assert(genesis.hashMerkleRoot == uint256S("0x3e8d1c67030da19c37775237b858508f53b7867eb0b444adae944cbe0d30520f"));

        // vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        // vSeeds.push_back(CDNSSeedData("jrn.me.uk", "testseed.jrn.me.uk"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,113); // 0x71
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196); // 0xc4
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,241); // 0xf1
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xcf).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;

        checkpointData = (CCheckpointData) {
            {

            }
            // boost::assign::map_list_of
            // ( 0, uint256S("0x001"))
            // ( 483173, uint256S("0xa804201ca0aceb7e937ef7a3c613a9b7589245b10cc095148c4ce4965b0b73b5"))
            // ( 591117, uint256S("0x5f6b93b2c28cedf32467d900369b8be6700f0649388a7dbfd3ebd4a01b1ffad8"))
            // ( 658924, uint256S("0xed6c8324d9a77195ee080f225a0fca6346495e08ded99bcda47a8eea5a8a620b"))
            // ( 703635, uint256S("0x839fa54617adcd582d53030a37455c14a87a806f6615aa8213f13e196230ff7f"))
            // ( 1000000, uint256S("0x1fe4d44ea4d1edb031f52f0d7c635db8190dc871a190654c41d2450086b8ef0e"))
            // ( 1202214, uint256S("0xa2179767a87ee4e95944703976fee63578ec04fa3ac2fc1c9c2c83587d096977"))
            // ( 1250000, uint256S("0xb46affb421872ca8efa30366b09694e2f9bf077f7258213be14adb05a9f41883"))
            // ( 1500000, uint256S("0x0caa041b47b4d18a4f44bdc05cef1a96d5196ce7b2e32ad3e4eb9ba505144917"))
            // ( 1750000, uint256S("0x8042462366d854ad39b8b95ed2ca12e89a526ceee5a90042d55ebb24d5aab7e9"))
            // ( 2000000, uint256S("0xd6acde73e1b42fc17f29dcc76f63946d378ae1bd4eafab44d801a25be784103c"))
            // ( 2250000, uint256S("0xc4342ae6d9a522a02e5607411df1b00e9329563ef844a758d762d601d42c86dc"))
            // ( 2500000, uint256S("0x3a66ec4933fbb348c9b1889aaf2f732fe429fd9a8f74fee6895eae061ac897e2"))
            // ( 2750000, uint256S("0x473ea9f625d59f534ffcc9738ffc58f7b7b1e0e993078614f5484a9505885563"))
            // ( 3062910, uint256S("0x113c41c00934f940a41f99d18b2ad9aefd183a4b7fe80527e1e6c12779bd0246"))
            // ( 3286675, uint256S("0x07fef07a255d510297c9189dc96da5f4e41a8184bc979df8294487f07fee1cf3"))
            // ( 3445426, uint256S("0x70574db7856bd685abe7b0a8a3e79b29882620645bd763b01459176bceb58cd1"))
            // ( 3976284, uint256S("0xaf23c3e750bb4f2ce091235f006e7e4e2af453d4c866282e7870471dcfeb4382"))
        };

        chainTxData = ChainTxData{
            // Data as of block af23c3e750bb4f2ce091235f006e7e4e2af453d4c866282e7870471dcfeb4382 (height 3976284)
            // 1681098301, // * UNIX timestamp of last checkpoint block
            // 10,    // * total number of transactions between genesis and last checkpoint
            // 0.02        // * estimated number of transactions per second after that timestamp
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
private:
    Consensus::Params digishieldConsensus;
    Consensus::Params auxpowConsensus;
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.powLimit = uint256S("0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1;
        consensus.nPowTargetTimespan = 4 * 60 * 60; // pre-digishield: 4 hours
        consensus.nPowTargetSpacing = 1; // regtest: 1 second blocks
        consensus.fDigishieldDifficultyCalculation = false;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 540; // 75% for testchains
        consensus.nMinerConfirmationWindow = 720; // Faster than normal for regtest (2,520 instead of 10,080)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        // AuxPow parameters
        consensus.nAuxpowChainId = 0x0062; // 98 - Josh Wise!
        consensus.fStrictChainId = true;
        consensus.fAllowLegacyBlocks = true;

        // Dogecoin parameters
        consensus.fSimplifiedRewards = true;
        consensus.nCoinbaseMaturity = 60; // For easier testability in RPC tests

        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 0;
        digishieldConsensus.nPowTargetTimespan = 1; // regtest: also retarget every second in digishield mode, for conformity
        digishieldConsensus.fDigishieldDifficultyCalculation = true;

        auxpowConsensus = digishieldConsensus;
        auxpowConsensus.fAllowLegacyBlocks = false;
        auxpowConsensus.nHeightEffective = 0;

        // Assemble the binary search tree of parameters
        digishieldConsensus.pLeft = &consensus;
        digishieldConsensus.pRight = &auxpowConsensus;
        pConsensusRoot = &digishieldConsensus;

        pchMessageStart[0] = 0xfb;
        pchMessageStart[1] = 0xbe;
        pchMessageStart[2] = 0xb4;
        pchMessageStart[3] = 0xdb;
        nDefaultPort = 18778;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1681098301, 0, 0x207fffff, 1, 88 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        digishieldConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        auxpowConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
//        assert(consensus.hashGenesisBlock == uint256S("0xd1fe73b87f5b2263faf24bbda99eec05b35d5d6d4eec83a9e6a87acbdf1e84e1"));
//        assert(genesis.hashMerkleRoot == uint256S("0x3e8d1c67030da19c37775237b858508f53b7867eb0b444adae944cbe0d30520f"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = (CCheckpointData){
            {

            }
            // boost::assign::map_list_of
            // ( 0, uint256S("0x001"))
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);  // 0x6f
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);  // 0xc4
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);  // 0xef
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

const Consensus::Params *Consensus::Params::GetConsensus(uint32_t nTargetHeight) const {
    if (nTargetHeight < this -> nHeightEffective && this -> pLeft != NULL) {
        return this -> pLeft -> GetConsensus(nTargetHeight);
    } else if (nTargetHeight > this -> nHeightEffective && this -> pRight != NULL) {
        const Consensus::Params *pCandidate = this -> pRight -> GetConsensus(nTargetHeight);
        if (pCandidate->nHeightEffective <= nTargetHeight) {
            return pCandidate;
        }
    }

    // No better match below the target height
    return this;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}
