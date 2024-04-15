// Copyright (c) 2015-2021 The Pqcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "policy/policy.h"
#include "arith_uint256.h"
#include "pqcoin.h"
#include "txmempool.h"
#include "util.h"
#include "validation.h"
#include "pqcoin-fees.h"

// wh_include
#include <unordered_map>
#include <string>
#include <fstream>
using namespace std;
vector<uint32_t> btc_nBits;
vector<uint32_t> btc_block_counts;
vector<uint32_t> ltc_nBits;
vector<uint32_t> ltc_block_counts;
unordered_map<string, pair<uint32_t, uint32_t>> hash_to_nBits;

// extern void resetCount();
uint32_t count1;
uint32_t count2;
double change_to_nbits(unsigned int i)
{
    int nShift = (i >> 24) & 0xff;
    double dDiff = (double)0x0000ffff / (double)(i & 0x00ffffff);
    while (nShift < 29)
    {
        dDiff *= 256.0;
        nShift++;
    }
    while (nShift > 29)
    {
        dDiff /= 256.0;
        nShift--;
    }
    return dDiff;
}

int static generateMTRandom(unsigned int s, int range)
{
    boost::mt19937 gen(s);
    boost::uniform_int<> dist(1, range);
    return dist(gen);
}

// Pqcoin: Normally minimum difficulty blocks can only occur in between
// retarget blocks. However, once we introduce Digishield every block is
// a retarget, so we need to handle minimum difficulty on all blocks.
bool AllowDigishieldMinDifficultyForBlock(const CBlockIndex *pindexLast, const CBlockHeader *pblock, const Consensus::Params &params)
{
    // check if the chain allows minimum difficulty blocks
    if (!params.fPowAllowMinDifficultyBlocks)
        return false;

    // check if the chain allows minimum difficulty blocks on recalc blocks
    if (pindexLast->nHeight < 157500)
        // if (!params.fPowAllowDigishieldMinDifficultyBlocks)
        return false;

    // Allow for a minimum block time if the elapsed time > 2*nTargetSpacing
    return (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing * 2);
}

unsigned int CalculatePqcoinNextWorkRequired(const CBlockIndex *pindexLast, int64_t nFirstBlockTime, const Consensus::Params &params)
{
    int nHeight = pindexLast->nHeight + 1;
    const int64_t retargetTimespan = params.nPowTargetTimespan;
    const int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    int64_t nModulatedTimespan = nActualTimespan;
    int64_t nMaxTimespan;
    int64_t nMinTimespan;

    if (params.fDigishieldDifficultyCalculation) // DigiShield implementation - thanks to RealSolid & WDC for this code
    {
        // amplitude filter - thanks to daft27 for this code
        nModulatedTimespan = retargetTimespan + (nModulatedTimespan - retargetTimespan) / 8;

        nMinTimespan = retargetTimespan - (retargetTimespan / 4);
        nMaxTimespan = retargetTimespan + (retargetTimespan / 2);
    }
    else if (nHeight > 10000)
    {
        nMinTimespan = retargetTimespan / 4;
        nMaxTimespan = retargetTimespan * 4;
    }
    else if (nHeight > 5000)
    {
        nMinTimespan = retargetTimespan / 8;
        nMaxTimespan = retargetTimespan * 4;
    }
    else
    {
        nMinTimespan = retargetTimespan / 16;
        nMaxTimespan = retargetTimespan * 4;
    }

    // Limit adjustment step
    if (nModulatedTimespan < nMinTimespan)
        nModulatedTimespan = nMinTimespan;
    else if (nModulatedTimespan > nMaxTimespan)
        nModulatedTimespan = nMaxTimespan;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    arith_uint256 bnOld;
    bnNew.SetCompact(pindexLast->nBits);
    bnOld = bnNew;
    bnNew *= nModulatedTimespan;
    bnNew /= retargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

unsigned int CalculatePqcoinNextWorkRequired_new(const CBlockIndex *pindexLast, int64_t nFirstBlockTime, const Consensus::Params &params)
{
    int nHeight = pindexLast->nHeight + 1;
    const int64_t retargetTimespan = params.nPowTargetTimespan;
    const int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    int64_t nModulatedTimespan = nActualTimespan;
    int64_t nMaxTimespan;
    int64_t nMinTimespan;
    // **jsrchange_240127 Record the difficulty changes of pqcoins
    time_t currentTime = std::time(nullptr);
    tm *localTime = std::localtime(&currentTime);
    char timeString[100];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);
    int year = 1900 + localTime->tm_year;
    string file_name_date = "pqcoin_nbits_" + std::to_string(localTime->tm_mon + 1) + " _ " + std::to_string(localTime->tm_mday) + "_" + std::to_string(year);

    fstream file(file_name_date, ios::app | ios::out);
    file << "-------------------------pqcoins_nBits_240127---------------------------" << std::endl;
    file << "nActualTimespan: ";
    file.width(15);
    file << nActualTimespan;
    file.width(20);
    file << "retargetTimespan: ";
    file.width(15);
    file << retargetTimespan;
    file << std::endl;
    file << nHeight << "   " << timeString << std::endl;
    // pqcoin 和litecoin的难度值一致
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew_pq;
    bnNew_pq.SetCompact(ltc_nBits.back());
    if (bnNew_pq > bnPowLimit)
        bnNew_pq = bnPowLimit;
    file << "pqNew_ltc : " << ltc_nBits.back() << std::endl;
    file << "pqNew_btc : " << bnNew_pq.GetCompact() << std::endl;
    file << "pqNew_btc_nbits : " << change_to_nbits(bnNew_pq.GetCompact()) << std::endl;
    return bnNew_pq.GetCompact();
}

void CalculatecoinNextWorkRequired_only_aux(const CBlockIndex *pindexLast, const Consensus::Params &params)
{
    int nHeight = pindexLast->nHeight + 1;
    const CBlockIndex *pindexFirst;
    if (nHeight > params.DifficultyAdjustmentInterval())
        //consensus.DifficultyAdjustmentInterval()
        pindexFirst = pindexLast->GetAncestor(nHeight - params.DifficultyAdjustmentInterval()); // 追溯720个块
    else
        pindexFirst = pindexLast->GetAncestor(nHeight - 1);

    const int64_t retargetTimespan = params.nPowTargetTimespan;                               // 目标时间间隔
    const int64_t nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime(); // 选择的父链实际的时间跨度

    vector<uint32_t> y_n(4);
    vector<uint32_t> x_n(4);

    time_t currentTime = std::time(nullptr);
    tm *localTime = std::localtime(&currentTime);
    char timeString[100];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);
    int year = 1900 + localTime->tm_year;
    string file_name_date = "nbits" + std::to_string(localTime->tm_mon + 1) + "_" + std::to_string(localTime->tm_mday) + "_" + std::to_string(year);

    fstream file(file_name_date, ios::app | ios::out);
    file << "-------------------------********---------------------------" << std::endl;
    file << "just see    :params.DifficultyAdjustmentInterval() :  " << params.DifficultyAdjustmentInterval()<<std::endl;
    file << "btc_count[]:  ";
    for (int i = 0; i < 4; i++)
    {
        x_n[i] = *(btc_block_counts.end() - i - 1);
        file << x_n[i] << "  ";
    }
    file << std::endl;
    file << "ltc_count[]:  ";
    for (int i = 0; i < 4; i++)
    {
        y_n[i] = *(ltc_block_counts.end() - i - 1);
        file << y_n[i] << "  ";
    }
    file << std::endl;

    file << "nActualTimespan: ";
    file.width(15);
    file << nActualTimespan;
    file.width(20);
    file << "retargetTimespan: ";
    file.width(15);
    file << retargetTimespan;
    file << std::endl;
    double A_T = (double)nActualTimespan / retargetTimespan;
    file << "A/T:  " << A_T << std::endl;

    // Limit adjustment step
    // 用float的精度存储中间变量
    double temp_btc_1;
    double temp_ltc_1;
    double proportion_b_l = 10.0;

    temp_btc_1 = (double)proportion_b_l * y_n[0] / x_n[0];
    file << "10.0*y_n[0]/x_n[0] :      " << temp_btc_1 << "         ";
    double bound_b_1 = 3.0;
    double bound_b_2 = 0.33;
    if (temp_btc_1 > bound_b_1)
        temp_btc_1 = bound_b_1;
    else if (temp_btc_1 < bound_b_2)
        temp_btc_1 = bound_b_2;

    temp_ltc_1 = (double)x_n[0] / (proportion_b_l * y_n[0]);
    file << "x_n[0]/10.0*y_n[0] :      " << temp_ltc_1 << std::endl;
    double bound_l_3 = 4.0;
    double bound_l_4 = 0.25;
    if (temp_ltc_1 > bound_l_3)
        temp_ltc_1 = bound_l_3;
    else if (temp_ltc_1 < bound_l_4)
        temp_ltc_1 = bound_l_4;
    file << "temp_btc_1 :      " << temp_btc_1 << "       "
         << "temp_ltc_1   " << temp_ltc_1 << std::endl;

    double bound_b_3 = 3.0;
    double target_btc = 1833.0;
    double temp_btc_11 = target_btc / x_n[0];
    if (temp_btc_11 > bound_b_3)
        temp_btc_11 = bound_b_3;

    double bound_l_1 = 3.0;
    double bound_l_2 = 0.33;
    double target_ltc = 183.0;
    double temp_ltc_11 = target_ltc / y_n[0];
    if (temp_ltc_11 > bound_l_1)
        temp_ltc_11 = bound_l_1;
    else if (temp_ltc_11 < bound_l_2)
        temp_ltc_11 = bound_l_2;

    double ratio_1 = 0.5;
    double ratio_2 = 0.5;
    temp_btc_1 = temp_btc_1 * ratio_1 + temp_btc_11 * ratio_2;
    temp_ltc_1 = temp_ltc_1 * ratio_1 + temp_ltc_11 * ratio_2;
    file << "temp_btc_1_final :      " << temp_btc_1 << "       "
         << "temp_ltc_1_final   " << temp_ltc_1 << std::endl;
    uint32_t diff_1[3];
    uint32_t diff_2[3];
    for (int i = 0; i < 3; i++)
    {
        diff_1[i] = (x_n[i] > x_n[i + 1]) ? (x_n[i] - x_n[i + 1]) : (x_n[i + 1] - x_n[i]);
        diff_2[i] = (y_n[i] > y_n[i + 1]) ? (y_n[i] - y_n[i + 1]) : (y_n[i + 1] - y_n[i]);
    }

    double temp_x_a = 0.5 * diff_1[0] + 0.3 * diff_1[1] + 0.2 * diff_1[2];
    double temp_y_b = 0.5 * diff_2[0] + 0.3 * diff_2[1] + 0.2 * diff_2[2];
    file << "temp_x_a :  " << temp_x_a << "   temp_y_b   " << temp_y_b << std::endl;

    double temp_btc_2;
    double temp_ltc_2;
    int64_t diff_a;
    if (x_n[1] > x_n[0])
        diff_a = x_n[1] - x_n[0];
    else
        diff_a = (int64_t)(-1) * (x_n[0] - x_n[1]);
    if (temp_x_a == 0.0)
    {
        temp_btc_2 = 1.0;
    }
    else if ((temp_x_a <= 15) && (diff_1[0] <= 20))
    {
        temp_x_a = 56;
        temp_btc_2 = (double)diff_a / temp_x_a;
        file << "not pow(1.2,temp_btc_2):  " << temp_btc_2 << "   ";
        temp_btc_2 = pow(1.2, temp_btc_2);
    }
    else
    {
        temp_btc_2 = (double)diff_a / temp_x_a;
        file << "not pow(1.2,temp_btc_2):  " << temp_btc_2 << "    ";
        temp_btc_2 = pow(1.2, temp_btc_2);
    }

    int64_t diff_b;
    if (y_n[1] > y_n[0])
        diff_b = y_n[1] - y_n[0];
    else
        diff_b = (int64_t)(-1) * (y_n[0] - y_n[1]);
    if (temp_y_b == 0.0)
    {
        temp_ltc_2 = 1.0;
    }
    else if ((temp_y_b <= 8) && (diff_2[0] <= 10))
    {
        temp_y_b = 28;
        temp_ltc_2 = (double)diff_b / temp_y_b;
        file << "not pow(1.2,temp_ltc_2):  " << temp_ltc_2 << std::endl;
        temp_ltc_2 = pow(1.2, temp_ltc_2);
    }
    else
    {
        temp_ltc_2 = (double)diff_b / temp_y_b;
        file << "not pow(1.2,temp_ltc_2):  " << temp_ltc_2 << std::endl;
        temp_ltc_2 = pow(1.2, temp_ltc_2);
    }

    file << "pow(1.2,temp_btc_2):  " << temp_btc_2 << "   pow(1.2,temp_ltc_2)   " << temp_ltc_2 << std::endl;

    if (temp_btc_2 > 1.5)
        temp_btc_2 = 1.5;
    else if (temp_btc_2 < 0.67)
        temp_btc_2 = 0.67;
    if (temp_ltc_2 > 1.5)
        temp_ltc_2 = 1.5;
    else if (temp_ltc_2 < 0.67)
        temp_ltc_2 = 0.67;

    // nActualTimespan != retargetTimespan 的两种情况只有这个不同，为避免冗余合并
    double temp_btc_3;
    double temp_ltc_3;
    if (A_T > 1.05)
    {
        temp_btc_3 = (temp_btc_1 * temp_btc_2 > 1.0) ? (temp_btc_1 * temp_btc_2) : 1.0;
        temp_ltc_3 = (temp_ltc_1 * temp_ltc_2 > 1.0) ? (temp_ltc_1 * temp_ltc_2) : 1.0;
    }
    else if (A_T < 0.95)
    {
        temp_btc_3 = (temp_btc_1 * temp_btc_2 > 1.0) ? 1.0 : (temp_btc_1 * temp_btc_2);
        temp_ltc_3 = (temp_ltc_1 * temp_ltc_2 > 1.0) ? 1.0 : (temp_ltc_1 * temp_ltc_2);
    }
    else
    {
        temp_btc_3 = temp_btc_1 * temp_btc_2;
        temp_ltc_3 = temp_ltc_1 * temp_ltc_2;
    }

    file << "temp_btc_1*temp_btc_2:  " << temp_btc_1 * temp_btc_2 << "     temp_ltc_1*temp_ltc_2   " << temp_ltc_1 * temp_ltc_2 << std::endl;
    file << "temp_btc_3:  " << temp_btc_3 << "         temp_ltc_3 :    " << temp_ltc_3 << std::endl;

    if (A_T > 4.0)
        A_T = 4.0;
    else if (A_T < 0.25)
        A_T = 0.25;

    file << "btc_3  " << temp_btc_3 * A_T << "   ltc_3  " << temp_ltc_3 * A_T << std::endl;
    double btc_4 = temp_btc_1 * temp_btc_2 * A_T;
    double ltc_4 = temp_ltc_1 * temp_ltc_2 * A_T;
    file << "btc_4  " << btc_4 << "   ltc_4  " << ltc_4 << std::endl;
    int count_less6 = 0;
    int count_zero = 0;
    int count_more = 0;
    // int count_710=0;
    for (auto y_i : y_n)
    {
        if (y_i == 0)
            count_zero++;
        else if (y_i <= 6)
            count_less6++;
        else if (y_i >= 500)
        {
            // if(y_i >= 710) count_710++;
            count_more++;
        }
    }

    if (y_n[0] <= 6 && (A_T <= 2 && A_T >= 0.5))
    {
        if ((A_T >= 1) && (count_less6 >= 3))
            ltc_4 = log10(10 + A_T);
        else
            ltc_4 = log10(10 + ltc_4); // 3.5 - 1.130333768495006
        // btc_4 = temp_btc_3*A_T; //0.682 *TBN*A/T
        // temp_btc_3 >=1 蕴含A_T<0.95
        // if(temp_btc_3 >=1) btc_4 = log10(10+btc_4); //0.682 *TBN*A/T  0.9693227061122
        // else btc_4 = log10(10-btc_4);
        btc_4 = temp_btc_3 * A_T;
        if (btc_4 > 1.25)
            btc_4 = 1.25;
        else if (btc_4 < 0.8)
            btc_4 = 0.8;
    }
    else if (y_n[0] >= 500 && (A_T <= 2 && A_T >= 0.5))
    {
        if ((A_T >= 1) && (count_more >= 3))
            btc_4 = log10(10 + A_T);
        else
            btc_4 = log10(10 + btc_4);
        // if(temp_ltc_3 >=1) ltc_4 = log10(10+ltc_4);
        // else ltc_4 = log10(10-ltc_4);
        ltc_4 = temp_ltc_3 * A_T;
        if (ltc_4 > 1.25)
            ltc_4 = 1.25;
        else if (ltc_4 < 0.8)
            ltc_4 = 0.8;
    }

    else
    {
        btc_4 = temp_btc_3 * A_T;
        ltc_4 = temp_ltc_3 * A_T;
    }

    file << "btc_4  " << btc_4 << "   ltc_4  " << ltc_4 << std::endl;

    if (btc_4 > 5)
        btc_4 = 5;
    else if (btc_4 < 0.2)
        btc_4 = 0.2;
    if (ltc_4 > 5)
        ltc_4 = 5;
    else if (ltc_4 < 0.2)
        ltc_4 = 0.2;

    file << "0.2~btc_temp3*A_T~5   " << btc_4 << "   0.2~ltc_temp3*A_T~5  " << ltc_4 << std::endl;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew_btc;
    arith_uint256 bnNew_ltc;
    bnNew_btc.SetCompact(btc_nBits.back());
    bnNew_ltc.SetCompact(ltc_nBits.back());
    file << "before:-----------------------------" << endl;
    file << "bnNew_btc_nbits : " << change_to_nbits(bnNew_btc.GetCompact()) << std::endl;
    file << "bnNew_ltc_nbits : " << change_to_nbits(bnNew_ltc.GetCompact()) << std::endl;

    int64_t accuracy = 1000;
    arith_uint256 bnNew_btc_over = bnNew_btc;
    arith_uint256 bnNew_ltc_over = bnNew_ltc;

    double nModulatedTimespan_btc = accuracy * btc_4;
    double nModulatedTimespan_ltc = accuracy * ltc_4;
    file << "nModulatedTimespan_btc    " << nModulatedTimespan_btc << "     nModulatedTimespan_ltc     " << nModulatedTimespan_ltc << std::endl;

    bnNew_btc_over *= (int64_t)nModulatedTimespan_btc; // 最多5000
    if (bnNew_btc_over > bnPowLimit)
    {
        file << "   !!!!!bnNew_btc_over *= nModulatedTimespan_btc>bnPowLimit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        bnNew_btc /= accuracy;
        bnNew_btc *= (int64_t)nModulatedTimespan_btc;
    }
    else
    {
        bnNew_btc_over /= accuracy;
        bnNew_btc = bnNew_btc_over;
    }

    bnNew_ltc_over *= (int64_t)nModulatedTimespan_ltc;
    if (bnNew_ltc_over > bnPowLimit)
    {
        file << "    !!LTC!!!!!bnNew_ltc_over *= nModulatedTimespan_ltc>bnPowLimit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        bnNew_ltc /= accuracy;
        bnNew_ltc *= (int64_t)nModulatedTimespan_ltc;
    }
    else
    {
        bnNew_ltc_over /= accuracy;
        bnNew_ltc = bnNew_ltc_over;
    }

    //"0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
    // if (bnNew_btc > bnPowLimit) bnNew_btc = bnPowLimit;
    // if (bnNew_ltc > bnPowLimit) bnNew_ltc = bnPowLimit;

    const int64_t btc_low_rate = 1000;
    const int64_t ltc_low_rate = 100;
    const arith_uint256 btc_bnPowLimit_low = bnPowLimit / btc_low_rate;
    const arith_uint256 ltc_bnPowLimit_low = bnPowLimit / ltc_low_rate;
    // 设置难度下限
    //  100倍  4.65654e-08，
    //  500倍  2.32827e-07，
    //  1000倍 4.65650e-07，
    //  5000倍 2.32825e-06.
    if (bnNew_btc > btc_bnPowLimit_low)
        bnNew_btc = btc_bnPowLimit_low;
    if (bnNew_ltc > ltc_bnPowLimit_low)
        bnNew_ltc = ltc_bnPowLimit_low;

    file << "after:-----------------------------" << endl;
    file << "bnNew_btc_nbits : " << change_to_nbits(bnNew_btc.GetCompact()) << std::endl;
    file << "bnNew_ltc_nbits : " << change_to_nbits(bnNew_ltc.GetCompact()) << std::endl;
    file << timeString << std::endl;
    file << "*****************************************************" << std::endl;

    btc_nBits.push_back(bnNew_btc.GetCompact());
    ltc_nBits.push_back(bnNew_ltc.GetCompact());
}


void CalculatecoinNextWorkRequired_only_aux_720blocks(const CBlockIndex *pindexLast, const Consensus::Params &params)
{
    int nHeight = pindexLast->nHeight + 1;
    const CBlockIndex *pindexFirst;
    if (nHeight > 720)
        pindexFirst = pindexLast->GetAncestor(nHeight - 720); // 追溯720个块
    else
        pindexFirst = pindexLast->GetAncestor(nHeight - 1);

    const int64_t retargetTimespan = params.nPowTargetTimespan;                               // 目标时间间隔
    const int64_t nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime(); // 选择的父链实际的时间跨度

    vector<uint32_t> y_n(4);
    vector<uint32_t> x_n(4);

    time_t currentTime = std::time(nullptr);
    tm *localTime = std::localtime(&currentTime);
    char timeString[100];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);
    int year = 1900 + localTime->tm_year;
    string file_name_date = "nbits" + std::to_string(localTime->tm_mon + 1) + "_" + std::to_string(localTime->tm_mday) + "_" + std::to_string(year);

    fstream file(file_name_date, ios::app | ios::out);
    file << "-------------------------********---------------------------" << std::endl;

    file << "btc_count[]:  ";
    for (int i = 0; i < 4; i++)
    {
        x_n[i] = *(btc_block_counts.end() - i - 1);
        file << x_n[i] << "  ";
    }
    file << std::endl;
    file << "ltc_count[]:  ";
    for (int i = 0; i < 4; i++)
    {
        y_n[i] = *(ltc_block_counts.end() - i - 1);
        file << y_n[i] << "  ";
    }
    file << std::endl;

    file << "nActualTimespan: ";
    file.width(15);
    file << nActualTimespan;
    file.width(20);
    file << "retargetTimespan: ";
    file.width(15);
    file << retargetTimespan;
    file << std::endl;
    double A_T = (double)nActualTimespan / retargetTimespan;
    file << "A/T:  " << A_T << std::endl;

    // Limit adjustment step
    // 用float的精度存储中间变量
    double temp_btc_1;
    double temp_ltc_1;
    double proportion_b_l = 10.0;

    temp_btc_1 = (double)proportion_b_l * y_n[0] / x_n[0];
    file << "10.0*y_n[0]/x_n[0] :      " << temp_btc_1 << "         ";
    double bound_b_1 = 3.0;
    double bound_b_2 = 0.33;
    if (temp_btc_1 > bound_b_1)
        temp_btc_1 = bound_b_1;
    else if (temp_btc_1 < bound_b_2)
        temp_btc_1 = bound_b_2;

    temp_ltc_1 = (double)x_n[0] / (proportion_b_l * y_n[0]);
    file << "x_n[0]/10.0*y_n[0] :      " << temp_ltc_1 << std::endl;
    double bound_l_3 = 4.0;
    double bound_l_4 = 0.25;
    if (temp_ltc_1 > bound_l_3)
        temp_ltc_1 = bound_l_3;
    else if (temp_ltc_1 < bound_l_4)
        temp_ltc_1 = bound_l_4;
    file << "temp_btc_1 :      " << temp_btc_1 << "       "
         << "temp_ltc_1   " << temp_ltc_1 << std::endl;

    double bound_b_3 = 3.0;
    double target_btc = 655.0;
    double temp_btc_11 = target_btc / x_n[0];
    if (temp_btc_11 > bound_b_3)
        temp_btc_11 = bound_b_3;

    double bound_l_1 = 3.0;
    double bound_l_2 = 0.33;
    double target_ltc = 65.0;
    double temp_ltc_11 = target_ltc / y_n[0];
    if (temp_ltc_11 > bound_l_1)
        temp_ltc_11 = bound_l_1;
    else if (temp_ltc_11 < bound_l_2)
        temp_ltc_11 = bound_l_2;

    double ratio_1 = 0.5;
    double ratio_2 = 0.5;
    temp_btc_1 = temp_btc_1 * ratio_1 + temp_btc_11 * ratio_2;
    temp_ltc_1 = temp_ltc_1 * ratio_1 + temp_ltc_11 * ratio_2;
    file << "temp_btc_1_final :      " << temp_btc_1 << "       "
         << "temp_ltc_1_final   " << temp_ltc_1 << std::endl;
    uint32_t diff_1[3];
    uint32_t diff_2[3];
    for (int i = 0; i < 3; i++)
    {
        diff_1[i] = (x_n[i] > x_n[i + 1]) ? (x_n[i] - x_n[i + 1]) : (x_n[i + 1] - x_n[i]);
        diff_2[i] = (y_n[i] > y_n[i + 1]) ? (y_n[i] - y_n[i + 1]) : (y_n[i + 1] - y_n[i]);
    }

    double temp_x_a = 0.5 * diff_1[0] + 0.3 * diff_1[1] + 0.2 * diff_1[2];
    double temp_y_b = 0.5 * diff_2[0] + 0.3 * diff_2[1] + 0.2 * diff_2[2];
    file << "temp_x_a :  " << temp_x_a << "   temp_y_b   " << temp_y_b << std::endl;

    double temp_btc_2;
    double temp_ltc_2;
    int64_t diff_a;
    if (x_n[1] > x_n[0])
        diff_a = x_n[1] - x_n[0];
    else
        diff_a = (int64_t)(-1) * (x_n[0] - x_n[1]);
    if (temp_x_a == 0.0)
    {
        temp_btc_2 = 1.0;
    }
    else if ((temp_x_a <= 5) && (diff_1[0] <= 8))
    {
        temp_x_a = 20;
        temp_btc_2 = (double)diff_a / temp_x_a;
        file << "not pow(1.2,temp_btc_2):  " << temp_btc_2 << "   ";
        temp_btc_2 = pow(1.2, temp_btc_2);
    }
    else
    {
        temp_btc_2 = (double)diff_a / temp_x_a;
        file << "not pow(1.2,temp_btc_2):  " << temp_btc_2 << "    ";
        temp_btc_2 = pow(1.2, temp_btc_2);
    }

    int64_t diff_b;
    if (y_n[1] > y_n[0])
        diff_b = y_n[1] - y_n[0];
    else
        diff_b = (int64_t)(-1) * (y_n[0] - y_n[1]);
    if (temp_y_b == 0.0)
    {
        temp_ltc_2 = 1.0;
    }
    else if ((temp_y_b <= 3) && (diff_2[0] <= 4))
    {
        temp_y_b = 10;
        temp_ltc_2 = (double)diff_b / temp_y_b;
        file << "not pow(1.2,temp_ltc_2):  " << temp_ltc_2 << std::endl;
        temp_ltc_2 = pow(1.2, temp_ltc_2);
    }
    else
    {
        temp_ltc_2 = (double)diff_b / temp_y_b;
        file << "not pow(1.2,temp_ltc_2):  " << temp_ltc_2 << std::endl;
        temp_ltc_2 = pow(1.2, temp_ltc_2);
    }

    file << "pow(1.2,temp_btc_2):  " << temp_btc_2 << "   pow(1.2,temp_ltc_2)   " << temp_ltc_2 << std::endl;

    if (temp_btc_2 > 1.5)
        temp_btc_2 = 1.5;
    else if (temp_btc_2 < 0.67)
        temp_btc_2 = 0.67;
    if (temp_ltc_2 > 1.5)
        temp_ltc_2 = 1.5;
    else if (temp_ltc_2 < 0.67)
        temp_ltc_2 = 0.67;

    // nActualTimespan != retargetTimespan 的两种情况只有这个不同，为避免冗余合并
    double temp_btc_3;
    double temp_ltc_3;
    if (A_T > 1.05)
    {
        temp_btc_3 = (temp_btc_1 * temp_btc_2 > 1.0) ? (temp_btc_1 * temp_btc_2) : 1.0;
        temp_ltc_3 = (temp_ltc_1 * temp_ltc_2 > 1.0) ? (temp_ltc_1 * temp_ltc_2) : 1.0;
    }
    else if (A_T < 0.95)
    {
        temp_btc_3 = (temp_btc_1 * temp_btc_2 > 1.0) ? 1.0 : (temp_btc_1 * temp_btc_2);
        temp_ltc_3 = (temp_ltc_1 * temp_ltc_2 > 1.0) ? 1.0 : (temp_ltc_1 * temp_ltc_2);
    }
    else
    {
        temp_btc_3 = temp_btc_1 * temp_btc_2;
        temp_ltc_3 = temp_ltc_1 * temp_ltc_2;
    }

    file << "temp_btc_1*temp_btc_2:  " << temp_btc_1 * temp_btc_2 << "     temp_ltc_1*temp_ltc_2   " << temp_ltc_1 * temp_ltc_2 << std::endl;
    file << "temp_btc_3:  " << temp_btc_3 << "         temp_ltc_3 :    " << temp_ltc_3 << std::endl;

    if (A_T > 4.0)
        A_T = 4.0;
    else if (A_T < 0.25)
        A_T = 0.25;

    file << "btc_3  " << temp_btc_3 * A_T << "   ltc_3  " << temp_ltc_3 * A_T << std::endl;
    double btc_4 = temp_btc_1 * temp_btc_2 * A_T;
    double ltc_4 = temp_ltc_1 * temp_ltc_2 * A_T;
    file << "btc_4  " << btc_4 << "   ltc_4  " << ltc_4 << std::endl;
    int count_less5 = 0;
    int count_zero = 0;
    int count_more = 0;
    // int count_710=0;
    for (auto y_i : y_n)
    {
        if (y_i == 0)
            count_zero++;
        else if (y_i <= 5)
            count_less5++;
        else if (y_i >= 320)
        {
            // if(y_i >= 710) count_710++;
            count_more++;
        }
    }

    if (y_n[0] <= 5 && (A_T <= 2 && A_T >= 0.5))
    {
        if ((A_T >= 1) && (count_less5 >= 3))
            ltc_4 = log10(10 + A_T);
        else
            ltc_4 = log10(10 + ltc_4); // 3.5 - 1.130333768495006
        // btc_4 = temp_btc_3*A_T; //0.682 *TBN*A/T
        // temp_btc_3 >=1 蕴含A_T<0.95
        // if(temp_btc_3 >=1) btc_4 = log10(10+btc_4); //0.682 *TBN*A/T  0.9693227061122
        // else btc_4 = log10(10-btc_4);
        btc_4 = temp_btc_3 * A_T;
        if (btc_4 > 1.25)
            btc_4 = 1.25;
        else if (btc_4 < 0.8)
            btc_4 = 0.8;
    }
    else if (y_n[0] >= 320 && (A_T <= 2 && A_T >= 0.5))
    {
        if ((A_T >= 1) && (count_more >= 3))
            btc_4 = log10(10 + A_T);
        else
            btc_4 = log10(10 + btc_4);
        // if(temp_ltc_3 >=1) ltc_4 = log10(10+ltc_4);
        // else ltc_4 = log10(10-ltc_4);
        ltc_4 = temp_ltc_3 * A_T;
        if (ltc_4 > 1.25)
            ltc_4 = 1.25;
        else if (ltc_4 < 0.8)
            ltc_4 = 0.8;
    }

    else
    {
        btc_4 = temp_btc_3 * A_T;
        ltc_4 = temp_ltc_3 * A_T;
    }

    file << "btc_4  " << btc_4 << "   ltc_4  " << ltc_4 << std::endl;

    if (btc_4 > 5)
        btc_4 = 5;
    else if (btc_4 < 0.2)
        btc_4 = 0.2;
    if (ltc_4 > 5)
        ltc_4 = 5;
    else if (ltc_4 < 0.2)
        ltc_4 = 0.2;

    file << "0.2~btc_temp3*A_T~5   " << btc_4 << "   0.2~ltc_temp3*A_T~5  " << ltc_4 << std::endl;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew_btc;
    arith_uint256 bnNew_ltc;
    bnNew_btc.SetCompact(btc_nBits.back());
    bnNew_ltc.SetCompact(ltc_nBits.back());
    file << "before:-----------------------------" << endl;
    file << "bnNew_btc_nbits : " << change_to_nbits(bnNew_btc.GetCompact()) << std::endl;
    file << "bnNew_ltc_nbits : " << change_to_nbits(bnNew_ltc.GetCompact()) << std::endl;

    int64_t accuracy = 1000;
    arith_uint256 bnNew_btc_over = bnNew_btc;
    arith_uint256 bnNew_ltc_over = bnNew_ltc;

    double nModulatedTimespan_btc = accuracy * btc_4;
    double nModulatedTimespan_ltc = accuracy * ltc_4;
    file << "nModulatedTimespan_btc    " << nModulatedTimespan_btc << "     nModulatedTimespan_ltc     " << nModulatedTimespan_ltc << std::endl;

    bnNew_btc_over *= (int64_t)nModulatedTimespan_btc; // 最多5000
    if (bnNew_btc_over > bnPowLimit)
    {
        file << "   !!!!!bnNew_btc_over *= nModulatedTimespan_btc>bnPowLimit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        bnNew_btc /= accuracy;
        bnNew_btc *= (int64_t)nModulatedTimespan_btc;
    }
    else
    {
        bnNew_btc_over /= accuracy;
        bnNew_btc = bnNew_btc_over;
    }

    bnNew_ltc_over *= (int64_t)nModulatedTimespan_ltc;
    if (bnNew_ltc_over > bnPowLimit)
    {
        file << "    !!LTC!!!!!bnNew_ltc_over *= nModulatedTimespan_ltc>bnPowLimit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        bnNew_ltc /= accuracy;
        bnNew_ltc *= (int64_t)nModulatedTimespan_ltc;
    }
    else
    {
        bnNew_ltc_over /= accuracy;
        bnNew_ltc = bnNew_ltc_over;
    }

    //"0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
    // if (bnNew_btc > bnPowLimit) bnNew_btc = bnPowLimit;
    // if (bnNew_ltc > bnPowLimit) bnNew_ltc = bnPowLimit;

    const int64_t btc_low_rate = 1000;
    const int64_t ltc_low_rate = 100;
    const arith_uint256 btc_bnPowLimit_low = bnPowLimit / btc_low_rate;
    const arith_uint256 ltc_bnPowLimit_low = bnPowLimit / ltc_low_rate;
    // 设置难度下限
    //  100倍  4.65654e-08，
    //  500倍  2.32827e-07，
    //  1000倍 4.65650e-07，
    //  5000倍 2.32825e-06.
    if (bnNew_btc > btc_bnPowLimit_low)
        bnNew_btc = btc_bnPowLimit_low;
    if (bnNew_ltc > ltc_bnPowLimit_low)
        bnNew_ltc = ltc_bnPowLimit_low;

    file << "after:-----------------------------" << endl;
    file << "bnNew_btc_nbits : " << change_to_nbits(bnNew_btc.GetCompact()) << std::endl;
    file << "bnNew_ltc_nbits : " << change_to_nbits(bnNew_ltc.GetCompact()) << std::endl;
    file << timeString << std::endl;
    file << "*****************************************************" << std::endl;

    btc_nBits.push_back(bnNew_btc.GetCompact());
    ltc_nBits.push_back(bnNew_ltc.GetCompact());
}

bool CheckAuxPowProofOfWork(const CBlockHeader &block, const Consensus::Params &params)
{
    /* Except for legacy blocks with full version 1, ensure that
       the chain ID is correct.  Legacy blocks are not allowed since
       the merge-mining start, which is checked in AcceptBlockHeader
       where the height is known.  */
    if (!block.IsLegacy() && params.fStrictChainId && block.GetChainId() != params.nAuxpowChainId)
        return error("%s : block does not have our chain ID"
                     " (got %d, expected %d, full nVersion %d)",
                     __func__, block.GetChainId(),
                     params.nAuxpowChainId, block.nVersion);

    /* If there is no auxpow, just check the block hash.  */
    if (!block.auxpow)
    {
        if (block.IsAuxpow())
            return error("%s : no auxpow on block with auxpow version",
                         __func__);

        if (!CheckProofOfWork(block.GetPoWHash(), block.nBits, params))
            return error("%s : non-AUX proof of work failed", __func__);

        return true;
    }

    /* We have auxpow.  Check it.  */

    // wh_check_auxpow
    if (!block.IsAuxpow())
        return error("%s : auxpow on block with non-auxpow version", __func__);

    if (!block.auxpow->check(block.GetHash(), block.GetChainId(), params))
        return error("%s : AUX POW is not valid", __func__);
    if (block.auxpow->parentBlock.nVersion == 0x10000000)
    {
        if (!CheckProofOfWork(block.auxpow->getParentBlockPoWHash(), hash_to_nBits[block.GetHash().GetHex()].first, params))
            return error("%s : AUX proof of work failed", __func__);
    }
    else if (block.auxpow->parentBlock.nVersion == 0x20000000)
    {
        if (!CheckProofOfWork(block.auxpow->getParentBlockHash(), hash_to_nBits[block.GetHash().GetHex()].second, params))
            return error("%s : AUX proof of work failed", __func__);
    }
    return true;
}

CAmount GetPqcoinBlockSubsidy(int nHeight, const Consensus::Params& consensusParams, uint256 prevHash)
{
    int halvings =( nHeight -26250) / consensusParams.nSubsidyHalvingInterval;

    if (!consensusParams.fSimplifiedRewards){
        // Old-style rewards derived from the previous block hash
        const std::string cseed_str = prevHash.ToString().substr(7, 7);
        const char* cseed = cseed_str.c_str();
        char* endp = NULL;
        long seed = strtol(cseed, &endp, 16);
        CAmount maxReward = (1000000 >> halvings) - 1;
        int rand = generateMTRandom(seed, maxReward);

        return (1 + rand) * COIN;

    }
    else if (nHeight <= 26250)   return (1024*COIN);
    else {
        // New-style constant rewards for each halving interval
        return (168* COIN) >> halvings;
    } 
}
