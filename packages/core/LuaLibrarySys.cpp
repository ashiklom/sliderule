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

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include "LuaLibrarySys.h"
#include "LuaEngine.h"
#include "core.h"

#include <unistd.h>

/******************************************************************************
 * STATIC DATA
 ******************************************************************************/

const char* LuaLibrarySys::LUA_SYSLIBNAME = "sys";
const struct luaL_Reg LuaLibrarySys::sysLibs [] = {
    {"version",     LuaLibrarySys::lsys_version},
    {"quit",        LuaLibrarySys::lsys_quit},
    {"abort",       LuaLibrarySys::lsys_abort},
    {"alive",       LuaLibrarySys::lsys_alive},
    {"wait",        LuaLibrarySys::lsys_wait},
    {"log",         LuaLibrarySys::lsys_log},
    {"metric",      LuaLibrarySys::lsys_metric},
    {"lsmsgq",      LuaLibrarySys::lsys_lsmsgq},
    {"setenvver",   LuaLibrarySys::lsys_setenvver},
    {"type",        LuaLibrarySys::lsys_type},
    {"setstddepth", LuaLibrarySys::lsys_setstddepth},
    {"setiosz",     LuaLibrarySys::lsys_setiosize},
    {"getiosz",     LuaLibrarySys::lsys_getiosize},
    {"setlvl",      LuaLibrarySys::lsys_seteventlvl},
    {"getlvl",      LuaLibrarySys::lsys_geteventlvl},
    {"healthy",     LuaLibrarySys::lsys_healthy},
    {"ipv4",        LuaLibrarySys::lsys_ipv4},
    {"lsrec",       LuaLibrarySys::lsys_lsrec},
    {"cwd",         LuaLibrarySys::lsys_cwd},
    {"memu",        LuaLibrarySys::lsys_memu},
    {"lsdev",       DeviceObject::luaList},
    {NULL,          NULL}
};

/******************************************************************************
 * SYSTEM LIBRARY EXTENSION METHODS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * lsys_init
 *----------------------------------------------------------------------------*/
void LuaLibrarySys::lsys_init (void)
{
}

/*----------------------------------------------------------------------------
 * luaopen_cmdlib
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::luaopen_syslib (lua_State *L)
{
    luaL_newlib(L, sysLibs);
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_version
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_version (lua_State* L)
{
    /* Get Information */
    int64_t launch_time_gps = TimeLib::sys2gpstime(OsApi::getLaunchTime());
    TimeLib::gmt_time_t timeinfo = TimeLib::gps2gmttime(launch_time_gps);
    TimeLib::date_t dateinfo = TimeLib::gmt2date(timeinfo);
    SafeString timestr("%04d-%02d-%02dT%02d:%02d:%02dZ", timeinfo.year, dateinfo.month, dateinfo.day, timeinfo.hour, timeinfo.minute, timeinfo.second);
    int64_t duration = TimeLib::gpstime() - launch_time_gps;
    const char** pkg_list = LuaEngine::getPkgList();

    /* Display Information on Terminal */
    print2term("SlideRule Version:   %s\n", LIBID);
    print2term("Build Information:   %s\n", BUILDINFO);
    print2term("Environment Version: %s\n", OsApi::getEnvVersion());
    print2term("Launch Time: %s\n", timestr.str());
    print2term("Duration: %.2lf days\n", (double)duration / 1000.0 / 60.0 / 60.0 / 24.0); // milliseconds / seconds / minutes / hours
    print2term("Packages: [ ");
    if(pkg_list)
    {
        int index = 0;
        while(pkg_list[index])
        {
            print2term("%s", pkg_list[index]);
            index++;
            if(pkg_list[index]) print2term(", ");
        }
    }
    print2term(" ]\n");

    /* Return Information to Lua (and clean up package list) */
    lua_pushstring(L, LIBID);
    lua_pushstring(L, BUILDINFO);
    lua_pushstring(L, OsApi::getEnvVersion());
    lua_pushstring(L, timestr.str());
    lua_pushinteger(L, duration);
    lua_newtable(L);
    if(pkg_list)
    {
        int index = 0;
        while(pkg_list[index])
        {
            lua_pushstring(L, pkg_list[index]);
            lua_rawseti(L, -2, index+1);
            delete [] pkg_list[index];
            index++;
        }
        delete [] pkg_list;
    }

    return 6;
}

/*----------------------------------------------------------------------------
 * lsys_quit
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_quit (lua_State* L)
{
    /* Get errors reported by lua app */
    int errors = 0;
    if(lua_isnumber(L, 1))
    {
        errors = lua_tonumber(L, 1);
    }

    setinactive( errors );

    /* Return Status to Lua */
    lua_pushboolean(L, true);
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_abort
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_abort (lua_State* L)
{
    (void)L;

    quick_exit(0);

    return 0;
}

/*----------------------------------------------------------------------------
 * lsys_alive
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_alive (lua_State* L)
{
    lua_pushboolean(L, checkactive());
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_wait
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_wait (lua_State* L)
{
    /* Get Seconds to Wait */
    int secs = 0;
    if(lua_isnumber(L, 1))
    {
        secs = lua_tonumber(L, 1);
    }
    else
    {
        mlog(CRITICAL, "Incorrect parameter type for seconds to wait");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Wait */
    OsApi::sleep(secs);

    /* Return Success */
    lua_pushboolean(L, true);
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_log - .log(<level>, <message>)
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_log (lua_State* L)
{
    if(lua_isinteger(L, 1))
    {
        event_level_t lvl = (event_level_t)lua_tointeger(L, 1);
        if(lua_isstring(L, 2))
        {
            mlog(lvl, "%s", lua_tostring(L, 2));
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------
 * lsys_metric - .metric(<category>) --> table of metric values
 *----------------------------------------------------------------------------*/
static void populate_metric_table (const EventLib::metric_t& metric, int32_t index, void* parm)
{
    (void)index;

    lua_State* L = (lua_State*)parm;
    SafeString metric_full_name("%s.%s", metric.category, metric.name);

    lua_pushstring(L, metric_full_name.str());
    lua_newtable(L);
    {
        lua_pushstring(L, "value");
        lua_pushnumber(L, metric.value);
        lua_settable(L, -3);

        lua_pushstring(L, "type");
        lua_pushstring(L, EventLib::subtype2str(metric.subtype));
        lua_settable(L, -3);
    }
    lua_settable(L, -3);
}

int LuaLibrarySys::lsys_metric (lua_State* L)
{
    /* Get Category Parameter */
    const char* category = NULL;
    if(lua_isstring(L, 1)) // query category
    {
        category = lua_tostring(L, 1);
    }
    else if(!lua_isnil(L, 1) && lua_gettop(L) > 0)
    {
        mlog(CRITICAL, "Invalid parameter supplied to metric, must be nil or string (i.e. metric(\"mycategory\"))");
        return 0;
    }

    /* Populate Metric Table */
    lua_newtable(L);
    EventLib::iterateMetric(category, populate_metric_table, L);
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_lsmsgq
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_lsmsgq (lua_State* L)
{
    (void)L;

    int num_msgqs = MsgQ::numQ();
    if(num_msgqs > 0)
    {
        MsgQ::queueDisplay_t* msgQs = new MsgQ::queueDisplay_t[num_msgqs];
        int numq = MsgQ::listQ(msgQs, num_msgqs);
        print2term("\n");
        for(int i = 0; i < numq; i++)
        {
            print2term("MSGQ: %40s %8d %9s %d\n",
                msgQs[i].name, msgQs[i].len, msgQs[i].state,
                msgQs[i].subscriptions);
        }
        print2term("\n");
        delete [] msgQs;
    }

    return 0;
}

/*----------------------------------------------------------------------------
 * lsys_setenvver
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_setenvver (lua_State* L)
{
    bool status = true;
    const char* version_str = NULL;
    if(lua_isstring(L, 1))
    {
        version_str = lua_tostring(L, 1);
        OsApi::setEnvVersion(version_str);
    }
    else
    {
        mlog(CRITICAL, "Invalid parameter supplied to set environment version, must be a string");
        status = false;
    }

    lua_pushboolean(L, status);
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_type - TODO - needs to handle userdata types
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_type (lua_State* L)
{
    const char* obj_type = NULL;

    if(lua_isstring(L, 1)) // check event level parameter
    {
        const char* obj_name = lua_tostring(L, 1);
        if(MsgQ::existQ(obj_name))
        {
            obj_type = "MsgQ";
        }
        else if(RecordObject::isRecord(obj_name))
        {
            obj_type = "Record";
        }

        if(obj_type == NULL)
        {
            char errstr[MAX_STR_SIZE];
            StringLib::format(errstr, MAX_STR_SIZE, "Object %s not registered, unable to provide type!\n", obj_name);
            return luaL_error(L, errstr);
        }
    }
    else if(lua_isuserdata(L, 1))
    {
        obj_type = "LuaObject";
    }
    else
    {
        obj_type = "Unknown";
    }

    /* Return Status to Lua */
    lua_pushstring(L, obj_type); /* push object type */
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_setstddepth
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_setstddepth (lua_State* L)
{
    int depth = 0;
    if(lua_isnumber(L, 1))
    {
        depth = lua_tonumber(L, 1);
    }
    else
    {
        mlog(CRITICAL, "Standard queue depth must be a number");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Set Standard Depth */
    MsgQ::setStdQDepth(depth);

    /* Return Success */
    lua_pushboolean(L, true);
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_setiosize
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_setiosize (lua_State* L)
{
    bool status = false;

    if(!lua_isnumber(L, 1))
    {
        mlog(CRITICAL, "I/O maximum size must be a number");
    }
    else
    {
        /* Set I/O Size */
        int size = lua_tonumber(L, 1);
        status = OsApi::setIOMaxsize(size);
    }

    /* Return Status */
    lua_pushboolean(L, status);
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_getiosize
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_getiosize (lua_State* L)
{
    lua_pushnumber(L, OsApi::getIOMaxsize());
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_seteventlvl
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_seteventlvl (lua_State* L)
{
    bool status = false;
    int type_mask = 0;

    if(lua_isnumber(L, 1))
    {
        type_mask = lua_tonumber(L, 1);
        if(lua_isnumber(L, 2))
        {
            event_level_t lvl = (event_level_t)lua_tonumber(L, 2);
            if(type_mask & EventLib::LOG) EventLib::setLvl(EventLib::LOG, lvl);
            if(type_mask & EventLib::TRACE) EventLib::setLvl(EventLib::TRACE, lvl);
            if(type_mask & EventLib::METRIC) EventLib::setLvl(EventLib::METRIC, lvl);
            status = true;
        }
        else
        {
            mlog(CRITICAL, "event level must be a number");
        }
    }
    else
    {
        mlog(CRITICAL, "type mask must be a number");
    }

    /* Return Status */
    lua_pushboolean(L, status);
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_geteventlvl
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_geteventlvl (lua_State* L)
{
    lua_pushnumber(L, EventLib::getLvl(EventLib::LOG));
    lua_pushnumber(L, EventLib::getLvl(EventLib::TRACE));
    lua_pushnumber(L, EventLib::getLvl(EventLib::METRIC));
    return 3;
}

/*----------------------------------------------------------------------------
 * lsys_healthy
 *  - this is currently a placeholder for a more sophisticated health check
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_healthy (lua_State* L)
{
    lua_pushboolean(L, true);
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_ipv4
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_ipv4 (lua_State* L)
{
    lua_pushstring(L, SockLib::sockipv4());
    return 1;
}

/*----------------------------------------------------------------------------
 * lsys_lsrec
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_lsrec (lua_State* L)
{
    const char* pattern = NULL;
    if(lua_isstring(L, 1)) pattern = lua_tostring(L, 1);

    print2term("\n%50s %24s %s\n", "Type", "Id", "Size");
    char** rectypes = NULL;
    int numrecs = RecordObject::getRecords(&rectypes);
    for(int i = 0; i < numrecs; i++)
    {
        if(pattern == NULL || StringLib::find(rectypes[i], pattern))
        {
            const char* id_field = RecordObject::getRecordIdField(rectypes[i]);
            int data_size = RecordObject::getRecordDataSize(rectypes[i]);
            print2term("%50s %24s %d\n", rectypes[i], id_field != NULL ? id_field : "NA", data_size);
        }
        delete [] rectypes[i];
    }
    delete [] rectypes;
    return 0;
}

/*----------------------------------------------------------------------------
 * lsys_cwd - current working directory
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_cwd (lua_State* L)
{
    char cwd[MAX_STR_SIZE];
    char* cwd_ptr = getcwd(cwd, MAX_STR_SIZE);

    if(cwd_ptr != NULL)
    {
        lua_pushstring(L, cwd);
        return 1;
    }
    else
    {
        return 0;
    }
}

/*----------------------------------------------------------------------------
 * lsys_memu - memory usage
 *----------------------------------------------------------------------------*/
int LuaLibrarySys::lsys_memu (lua_State* L)
{
    double m = OsApi::memusage();
    lua_pushnumber(L, m);
    return 1;
}
