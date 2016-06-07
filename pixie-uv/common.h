//
//  common.h
//  pixie-uv
//
//  Created by ROBERT BLACKWELL on 6/6/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef common_h
#define common_h
#include <uv.h>
#include <logger/logger.h>
#include "simple_buffer.h"
#include "blk_message.h"
#include "blk_parser.h"

struct blk_connection_s;
typedef struct blk_connection_s blk_connection_t;

//
// need this little bit of forward decleration
//
#include "blk_connection.h"
typedef void (blk_handler_function)(blk_message_t* msg, blk_connection_t* conn);

#include "connection_table.h"

#include "server.h"

#endif /* common_h */
