// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "chainparams.h"
#include "clientversion.h"
#include "compat.h"
#include "rpc/server.h"
#include "init.h"
#include "noui.h"
#include "scheduler.h"
#include "util.h"
#include "httpserver.h"
#include "httprpc.h"
#include "utilstrencodings.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include <stdio.h>

//wh_include
#include <pthread.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <deque>
#include <vector>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pqcoin.h>
#include <unordered_map>
#include <string>
#include <fstream>
#include <ctime>
using namespace std;
extern vector<uint32_t> btc_nBits;
extern vector<uint32_t> btc_block_counts;
extern vector<uint32_t> ltc_nBits;
extern vector<uint32_t> ltc_block_counts;
extern unordered_map<string, pair<uint32_t, uint32_t>> hash_to_nBits;



//wh_getAuxBlockThread_define
extern UniValue createauxblock(const JSONRPCRequest& request);
extern UniValue SubmitAuxBlock(JSONRPCRequest& request);
extern bool createauxblock(const std::string& address, uint256 &hash, uint32_t &nbits, int id);
extern bool SubmitAuxBlock(const char buf[]);
void* getAuxBlockThread(void* arg) {
    cout<<"create thread starts"<<endl;
    int skfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (skfd == -1) {
        perror("socket failed in getAuxBlockthread!");
        exit(-1);
    }
    //wh_ip_getAuxBlockThread
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9988); 
    addr.sin_addr.s_addr = htonl(INADDR_ANY); 
 

    int ret = bind(skfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        perror("bind failed in getAuxBlockthread!");
        exit(-1);
    }

    char recvbuf[1024] = {0}; 
    char address[35] = {0}; 

    struct sockaddr_in client; 
    socklen_t len = sizeof(client);
    int id; 

    
    uint256 hash; 
    uint32_t nbits;
    while (1) {
        //memset(recvbuf, 0, sizeof(recvbuf));

        
        std::cout<<"create start"<<endl;
        int ret = recvfrom(skfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&client, &len);
        if (ret != sizeof(recvbuf)) {
            perror("recvfrom failed in getAuxBlockthread!\n");
            exit(-1);
        }


        memcpy(&id, recvbuf, sizeof(id));
        memcpy(address, recvbuf + sizeof(id), sizeof(address));

        UniValue Uad(address);
        UniValue Uid(id);

        JSONRPCRequest test;
        
    
        
        test.params.setArray();
        test.params.push_back(Uad);
        test.params.push_back(Uid);
        std::cout<<"created block address:"<<test.params[0].get_str()<<endl;
        std::cout<<"created block id:"<<test.params[1].get_int()<<endl;
        UniValue re = createauxblock(test);
        std::cout<<"create aux block success"<<endl;
        hash = uint256S(re[0].get_str());
        
        nbits = std::stoul(re[4].get_str(), nullptr, 16);
        if (id == 1) {
            hash_to_nBits[hash.GetHex()].first = nbits;
        } else {
            hash_to_nBits[hash.GetHex()].second = nbits;
        }
        


        memcpy(recvbuf, hash.begin(), 32);
        memcpy(recvbuf + 32, &nbits, sizeof(nbits));
   

        ret = sendto(skfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&client, sizeof(client));
        if (ret != sizeof(recvbuf)) {
            perror("sendto failed!\n");
            exit(-1);
        }
   
    }
    close(skfd);
}

void* submitAuxBlockThread(void* arg) __attribute__((optimize("O0")));
void* submitAuxBlockThread(void* arg) {
    std::cout<<"submit thread starts"<<endl;

    int skfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (skfd == -1) {
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);

    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(skfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }    
    char recvbuf[1024] = {0}; 
    sockaddr_in client;
    socklen_t len = sizeof(client);

    int accept = 0; 
    while (1) {
        std::cout<<"submit start"<<endl;
        memset(recvbuf, 0, sizeof(recvbuf));
        
        int num = recvfrom(skfd, recvbuf, sizeof(recvbuf), 0, (sockaddr*)&client, &len);
        JSONRPCRequest request;
        uint256 Uhash;
        const char *ptr = recvbuf;
        
        
        memcpy(Uhash.begin(), ptr, 32);
        
        string str_hash = Uhash.ToString();
        
        string str_auxpow(ptr+32,1024-32);

        UniValue  hash(str_hash);
        UniValue auxpow(str_auxpow);
        request.params.setArray();
        request.params.push_back(hash);
        request.params.push_back(auxpow);

        
        UniValue Uaccept = SubmitAuxBlock(request);
        
        accept = Uaccept.get_int();
        

        if (accept==1||accept==2) {
            std::cout<<"Uaccept success"<<accept<<endl;
            std::cout<<"SubmitAuxBlock success"<<accept<<endl;
            recvbuf[0] = 1;
            sendto(skfd, recvbuf, sizeof(recvbuf), 0, (sockaddr*)&client, sizeof(client));
        }
    
        // if (accept == 1) {
        //     ++count1;
        // } else if (accept == 2) {
        //     ++count2;
        // }
    }
    close(skfd);
}

/* Introduction text for doxygen: */

/*! \mainpage Developer documentation
 *a
 * \section intro_sec Introduction
 *
 * This is the developer documentation of the reference client for an experimental new digital currency called Bitcoin (https://www.bitcoin.org/),
 * which enables instant payments to anyone, anywhere in the world. Bitcoin uses peer-to-peer technology to operate
 * with no central authority: managing transactions and issuing money are carried out collectively by the network.
 *
 * The software is a community-driven open source project, released under the MIT license.
 *
 * \section Navigation
 * Use the buttons <code>Namespaces</code>, <code>Classes</code> or <code>Files</code> at the top of the page to start navigating the code.
 */

void WaitForShutdown(boost::thread_group* threadGroup)
{
    bool fShutdown = ShutdownRequested();
    // Tell the main threads to shutdown.
    while (!fShutdown)
    {
        MilliSleep(200);
        fShutdown = ShutdownRequested();
    }
    if (threadGroup)
    {
        Interrupt(*threadGroup);
        threadGroup->join_all();
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Start
//
bool AppInit(int argc, char* argv[])
{
    //wh_AppInit
    pthread_t tid, tid2;
    pthread_create(&tid, NULL, getAuxBlockThread, NULL);
    pthread_detach(tid);
    pthread_create(&tid2, NULL, submitAuxBlockThread, NULL);
    pthread_detach(tid2);

    boost::thread_group threadGroup;
    CScheduler scheduler;

    bool fRet = false;

    //
    // Parameters
    //
    // If Qt is used, parameters/bitcoin.conf are parsed in qt/bitcoin.cpp's main()
    ParseParameters(argc, argv);

    // Process help and version before taking care about datadir
    if (IsArgSet("-?") || IsArgSet("-h") ||  IsArgSet("-help") || IsArgSet("-version"))
    {
        std::string strUsage = strprintf(_("%s Daemon"), _(PACKAGE_NAME)) + " " + _("version") + " " + FormatFullVersion() + "\n";

        if (IsArgSet("-version"))
        {
            strUsage += FormatParagraph(LicenseInfo());
        }
        else
        {
            strUsage += "\n" + _("Usage:") + "\n" +
                  "  pqcoind [options]                     " + strprintf(_("Start %s Daemon"), _(PACKAGE_NAME)) + "\n";

            strUsage += "\n" + HelpMessage(HMM_BITCOIND);
        }

        fprintf(stdout, "%s", strUsage.c_str());
        return true;
    }

    try
    {
        if (!boost::filesystem::is_directory(GetDataDir(false)))
        {
            fprintf(stderr, "Error: Specified data directory \"%s\" does not exist.\n", GetArg("-datadir", "").c_str());
            return false;
        }
        try
        {
            ReadConfigFile(GetArg("-conf", BITCOIN_CONF_FILENAME));
        } catch (const std::exception& e) {
            fprintf(stderr,"Error reading configuration file: %s\n", e.what());
            return false;
        }
        // Check for -testnet or -regtest parameter (Params() calls are only valid after this clause)
        try {
            SelectParams(ChainNameFromCommandLine());
        } catch (const std::exception& e) {
            fprintf(stderr, "Error: %s\n", e.what());
            return false;
        }

        // Command-line RPC
        bool fCommandLine = false;
        for (int i = 1; i < argc; i++)
            if (!IsSwitchChar(argv[i][0]) && !boost::algorithm::istarts_with(argv[i], "pqcoin:"))
                fCommandLine = true;

        if (fCommandLine)
        {
            fprintf(stderr, "Error: There is no RPC client functionality in pqcoind anymore. Use the pqcoin-cli utility instead.\n");
            exit(EXIT_FAILURE);
        }
        // -server defaults to true for bitcoind but not for the GUI so do this here
        SoftSetBoolArg("-server", true);
        // Set this early so that parameter interactions go to console
        InitLogging();
        InitParameterInteraction();
        if (!AppInitBasicSetup())
        {
            // InitError will have been called with detailed error, which ends up on console
            exit(1);
        }
        if (!AppInitParameterInteraction())
        {
            // InitError will have been called with detailed error, which ends up on console
            exit(1);
        }
        if (!AppInitSanityChecks())
        {
            // InitError will have been called with detailed error, which ends up on console
            exit(1);
        }
        if (GetBoolArg("-daemon", false))
        {
#if HAVE_DECL_DAEMON
            fprintf(stdout, "Pqcoin server starting\n");

            // Daemonize
            if (daemon(1, 0)) { // don't chdir (1), do close FDs (0)
                fprintf(stderr, "Error: daemon() failed: %s\n", strerror(errno));
                return false;
            }
#else
            fprintf(stderr, "Error: -daemon is not supported on this operating system\n");
            return false;
#endif // HAVE_DECL_DAEMON
        }

        fRet = AppInitMain(threadGroup, scheduler);
    }
    catch (const std::exception& e) {
        PrintExceptionContinue(&e, "AppInit()");
    } catch (...) {
        PrintExceptionContinue(NULL, "AppInit()");
    }

    if (!fRet)
    {
        Interrupt(threadGroup);
        // threadGroup.join_all(); was left out intentionally here, because we didn't re-test all of
        // the startup-failure cases to make sure they don't result in a hang due to some
        // thread-blocking-waiting-for-another-thread-during-startup case
    } else {
        WaitForShutdown(&threadGroup);
    }
    Shutdown();

    return fRet;
}

int main(int argc, char* argv[])
{
    SetupEnvironment();
    // Connect bitcoind signal handlers
    noui_connect();
    return (AppInit(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE);
}
