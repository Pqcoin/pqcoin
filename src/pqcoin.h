// Copyright (c) 2015-2021 The Pqcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "amount.h"
#include "chain.h"
#include "chainparams.h"

//wh_include
#include <vector>
using namespace std;

extern uint32_t count1;
extern uint32_t count2;

bool AllowDigishieldMinDifficultyForBlock(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params);
CAmount GetPqcoinBlockSubsidy(int nHeight, const Consensus::Params& consensusParams, uint256 prevHash);
unsigned int CalculatePqcoinNextWorkRequired(const CBlockIndex* pindexLast, int64_t nLastRetargetTime, const Consensus::Params& params);
// ***jsr_change0127
unsigned int CalculatePqcoinNextWorkRequired_new(const CBlockIndex* pindexLast, int64_t nLastRetargetTime, const Consensus::Params& params);


//wh_decleartion
void CalculatecoinNextWorkRequired_only_aux(const CBlockIndex* pindexLast, const Consensus::Params& params);
double change_to_nbits(unsigned int i);

/**
 * Check proof-of-work of a block header, taking auxpow into account.
 * @param block The block header.
 * @param params Consensus parameters.
 * @return True iff the PoW is correct.
 */
bool CheckAuxPowProofOfWork(const CBlockHeader& block, const Consensus::Params& params);


