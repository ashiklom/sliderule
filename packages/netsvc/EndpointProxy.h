/*
 * Copyright (c) 2021, University of Washington
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the University of Washington nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF WASHINGTON AND CONTRIBUTORS
 * “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OF WASHINGTON OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __endpoint_proxy__
#define __endpoint_proxy__

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include "LuaObject.h"
#include "MsgQ.h"
#include "OsApi.h"
#include "OrchestratorLib.h"

/******************************************************************************
 * ATL03 READER
 ******************************************************************************/

class EndpointProxy: public LuaObject
{
    public:

        /*--------------------------------------------------------------------
         * Constants
         *--------------------------------------------------------------------*/

        static const int MAX_REQUEST_PARAMETER_SIZE = 0x2000000; // 32MB
        static const int CPU_LOAD_FACTOR = 10; // number of concurrent requests per cpu
        static const int COLLATOR_POLL_RATE = 1000; // milliseconds
        static const int DEFAULT_PROXY_QUEUE_DEPTH = 1000;
        static const int MAX_PROXY_THREADS = 1000;
        static const int DEFAULT_TIMEOUT = 600; // seconds

        static const char* SERVICE;

        static const char* OBJECT_TYPE;
        static const char* LuaMetaName;
        static const struct luaL_Reg LuaMetaTable[];

        /*--------------------------------------------------------------------
         * Methods
         *--------------------------------------------------------------------*/

        static int luaCreate (lua_State* L);

    private:

        /*--------------------------------------------------------------------
         * Data
         *--------------------------------------------------------------------*/

        bool                    active;
        Publisher*              rqstPub;
        Subscriber*             rqstSub;
        Thread**                proxyPids;
        Thread*                 collatorPid;
        const char**            resources;
        OrchestratorLib::Node** nodes;
        int                     numResources;
        int                     numResourcesComplete;
        Cond                    completion;
        const char*             endpoint;
        const char*             asset;
        const char*             parameters;
        int                     timeout;
        Publisher*              outQ;
        int                     numProxyThreads;
        int                     rqstQDepth;
        bool                    sendTerminator;

        /*--------------------------------------------------------------------
         * Methods
         *--------------------------------------------------------------------*/

                            EndpointProxy           (lua_State* L, const char* _endpoint, const char** _resources, int _num_resources,
                                                     const char* _parameters, int _timeout_secs, const char* _outq_name, bool _send_terminator,
                                                     int _num_threads, int _rqst_queue_depth);
                            ~EndpointProxy          (void);

        static void*        collatorThread          (void* parm);
        static void*        proxyThread             (void* parm);
};

#endif  /* __endpoint_proxy__ */
