/*
 * Copyright © 2018 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stddef.h>
#ifdef __APPLE__
#include <stdlib.h>
#else
#include <malloc.h>  // malloc
#endif

#include <string.h>
#include <errno.h>

#include "request.h"


 /*
 * allocate the memory of a new request an initialize according to the user request
 */
dbBE_Redis_request_t* dbBE_Redis_request_allocate( dbBE_Request_t *user )
{
  if( user == NULL )
    return NULL;

  dbBE_Redis_request_t *request = (dbBE_Redis_request_t*)malloc( sizeof( dbBE_Redis_request_t ) );
  if( request == NULL )
    return NULL;

  memset( request, 0, sizeof( dbBE_Redis_request_t ) );

  request->_status.generic = NULL;
  request->_user = user;
  request->_next = NULL;
  request->_conn_index = 0;
  request->_step = &gRedis_command_spec[ user->_opcode * DBBE_REDIS_COMMAND_STAGE_MAX ];

  return request;
}

void dbBE_Redis_request_destroy( dbBE_Redis_request_t *request )
{
  if( request == NULL )
    return;

  memset( request, 0, sizeof( dbBE_Redis_request_t ) );
  free( request );
}

int dbBE_Redis_request_stage_transition( dbBE_Redis_request_t *request )
{
  if( request == NULL )
    return -EINVAL;

  int stage = request->_step->_stage;
  if( request->_step->_final == 1 )
    return -EALREADY;

  ++stage;
  request->_step = &gRedis_command_spec[ request->_user->_opcode * DBBE_REDIS_COMMAND_STAGE_MAX + stage ];
  return 0;
}
