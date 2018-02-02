/* 
** ATMI level cache, EDB access
**
** @file atmi_cache_edb.c
** 
** -----------------------------------------------------------------------------
** Enduro/X Middleware Platform for Distributed Transaction Processing
** Copyright (C) 2015, Mavimax, Ltd. All Rights Reserved.
** This software is released under one of the following licenses:
** GPL or Mavimax's license for commercial use.
** -----------------------------------------------------------------------------
** GPL license:
** 
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
** PARTICULAR PURPOSE. See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 59 Temple
** Place, Suite 330, Boston, MA 02111-1307 USA
**
** -----------------------------------------------------------------------------
** A commercial use license is available from Mavimax, Ltd
** contact@mavimax.com
** -----------------------------------------------------------------------------
*/

/*---------------------------Includes-----------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ndrstandard.h>
#include <atmi.h>
#include <atmi_tls.h>
#include <typed_buf.h>

#include "thlock.h"
#include "userlog.h"
#include "utlist.h"
#include "exregex.h"
#include <exparson.h>
#include <atmi_cache.h>
#include <Exfields.h>
/*---------------------------Externs------------------------------------*/
/*---------------------------Macros-------------------------------------*/

#define NDRX_CACHE_TPERROR(atmierr, fmt, ...)\
        NDRX_LOG(log_error, fmt, ##__VA_ARGS__);\
        userlog(fmt, ##__VA_ARGS__);\
        ndrx_TPset_error_fmt(atmierr, fmt, ##__VA_ARGS__);

#define NDRX_CACHE_ERROR(fmt, ...)\
        NDRX_LOG(log_error, fmt, ##__VA_ARGS__);\
        userlog(fmt, ##__VA_ARGS__);

/*---------------------------Enums--------------------------------------*/
/*---------------------------Typedefs-----------------------------------*/
/*---------------------------Globals------------------------------------*/
/*---------------------------Statics------------------------------------*/
/*---------------------------Prototypes---------------------------------*/

/**
 * Begin MDB transaction
 * @param db db handler
 * @param txn transaction obj (out)
 * @return EXSUCCEED or edb error
 */
expublic int ndrx_cache_edb_begin(ndrx_tpcache_db_t *db, EDB_txn **txn)
{
    int ret = EXSUCCEED;
    
    if (EXSUCCEED!=(ret=edb_txn_begin(db->env, NULL, 0, txn)))
    {
        NDRX_CACHE_TPERROR(ndrx_cache_maperr(ret), 
                "Failed to begin transaction for [%s]: %s", 
                db->cachedb, edb_strerror(ret));
        
        goto out;
    }
    
out:
    return ret;
}

/**
 * Commit transaction
 * @param db db handler
 * @param txn transaction (in)
 * @return EXSUCCEED or edb error
 */
expublic int ndrx_cache_edb_commit(ndrx_tpcache_db_t *db, EDB_txn *txn)
{
    int ret = EXSUCCEED;
    
    if (EXSUCCEED!=(ret=edb_txn_commit(txn)))
    {
        NDRX_CACHE_TPERROR(ndrx_cache_maperr(ret), 
                "Failed to commit transaction for [%s]: %s", 
                db->cachedb, edb_strerror(ret));
        
        goto out;
    }
    
out:
    return ret;
}

/**
 * Abort transaction
 * @param db db handler
 * @param txn transaction (in)
 * @return EXSUCCEED or edb error
 */
expublic int ndrx_cache_edb_abort(ndrx_tpcache_db_t *db, EDB_txn *txn)
{
    int ret = EXSUCCEED;
    
    edb_txn_abort(txn);
    
out:
    return ret;
}

/**
 * Get data from DB with transaction.
 * @param db db handler
 * @param key string key
 * @param data_out data out obj data is valid till update or end of tran
 * @return EXSUCCEED/edb error
 */
expublic int ndrx_cache_edb_get(ndrx_tpcache_db_t *db, EDB_txn *txn, 
        char *key, EDB_val *data_out)
{
    int ret = EXSUCCEED;
    EDB_val keydb;
    
    keydb.mv_data = key;
    keydb.mv_size = strlen(key);
            
    if (EXSUCCEED!=(ret=edb_get(txn, db->dbi, &keydb, data_out)))
    {
        if (ret!=EDB_NOTFOUND)
        {
            NDRX_CACHE_TPERROR(ndrx_cache_maperr(ret), 
                "Failed to get data from db [%s] for key [%s]: %s", 
                db->cachedb, key, edb_strerror(ret));
            
        }
        else
        {
            NDRX_LOG(log_debug, "Failed to get data from db [%s] for key [%s]: %s", 
                db->cachedb, key, edb_strerror(ret));
        }
    }
    
out:
    return ret;
}

/**
 * Get data for cursor
 * @param db
 * @param cursor
 * @param key
 * @param data_out
 * @param op
 * @return 
 */
expublic int ndrx_cache_edb_cursor_get(ndrx_tpcache_db_t *db, EDB_cursor * cursor,
        char *key, EDB_val *data_out, EDB_cursor_op op)
{
    int ret = EXSUCCEED;
    EDB_val keydb;
    
    keydb.mv_data = key;
    keydb.mv_size = strlen(key);
            
    if (EXSUCCEED!=(ret=edb_cursor_get(cursor, &keydb, data_out, op)))
    {
        if (ret!=EDB_NOTFOUND)
        {
            NDRX_CACHE_TPERROR(ndrx_cache_maperr(ret), 
                "Failed to get data from db [%s] for key [%s]: %s", 
                db->cachedb, key, edb_strerror(ret));
        }
        else
        {
            NDRX_LOG(log_debug, "EOF [%s] for key [%s]: %s", 
                db->cachedb, key, edb_strerror(ret));
        }
    }
    
out:
    return ret;
}


/**
 * Set compare function
 * @param db
 * @param txn
 * @param cmp
 * @return 
 */
expublic int ndrx_cache_edb_set_dupsort(ndrx_tpcache_db_t *db, EDB_txn *txn, 
            EDB_cmp_func *cmp)
{
    int ret = EXSUCCEED;
    
    if (EXSUCCEED!=(ret=edb_set_dupsort(txn, db->dbi, cmp)))
    {
        NDRX_CACHE_TPERROR(ndrx_cache_maperr(ret), 
                "Failed to set dupsort cmp func for db [%s] %p: %s", 
            db->cachedb, cmp, edb_strerror(ret));
    }
    
out:
    return ret;
}

/**
 * Open cursor 
 * @param db
 * @param txn
 * @param cursor cursor out
 * @return 
 */
expublic int ndrx_cache_edb_cursor_open(ndrx_tpcache_db_t *db, EDB_txn *txn, 
            EDB_cursor ** cursor)
{
    int ret = EXSUCCEED;
    
    if (EXSUCCEED!=(ret=edb_cursor_open(txn, db->dbi, cursor)))
    {
        NDRX_CACHE_TPERROR(ndrx_cache_maperr(ret), 
                "Failed to open cursor [%s]: %s", 
                db->cachedb, edb_strerror(ret));
    }
    
out:
    return ret;
}

/**
 * Delete db record full or particular
 * @param db handler
 * @param txn transaction
 * @param key key (string based)
 * @param data data to delete, can be NULL, then full delete. Only for duplicate recs
 * @return EXSUCCEED/EXFAIL/DBERR
 */
expublic int ndrx_cache_edb_del (ndrx_tpcache_db_t *db, EDB_txn *txn, 
        char *key, EDB_val *data)
{
    int ret = EXSUCCEED;
    EDB_val keydb;
    
    keydb.mv_data = key;
    keydb.mv_size = strlen(key);
            
    if (EXSUCCEED!=(ret=edb_del(txn, db->dbi, &keydb, data)))
    {
        if (ret!=EDB_NOTFOUND)
        {
            NDRX_CACHE_TPERROR(ndrx_cache_maperr(ret), 
                "Failed to delete from db [%s] for key [%s], data: %p: %s", 
                db->cachedb, key, data, edb_strerror(ret));
        }
        else
        {
            NDRX_LOG(log_debug, "EOF [%s] for delete of key [%s] data: %p: %s", 
                db->cachedb, key, data, edb_strerror(ret));
        }
    }
out:
    return ret;
}

/**
 * Add data to database
 * @param db db hander
 * @param txn transaction
 * @param key string key
 * @param data data to put
 * @param flags LMDB flags
 * @return EXSUCCEED/LMDB err
 */
expublic int ndrx_cache_edb_put (ndrx_tpcache_db_t *db, EDB_txn *txn, 
        char *key, EDB_val *data, unsigned int flags)
{
    int ret = EXSUCCEED;
    EDB_val keydb;
    
    keydb.mv_data = key;
    keydb.mv_size = strlen(key);
            
    if (EXSUCCEED!=(ret=edb_put(txn, db->dbi, &keydb, data, flags)))
    {
        NDRX_CACHE_TPERROR(ndrx_cache_maperr(ret), 
            "Failed to to put to db [%s] key [%s], data: %p: %s", 
            db->cachedb, key, data, edb_strerror(ret));
    }
out:
    return ret;
}

