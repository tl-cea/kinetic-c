/*
* kinetic-c
* Copyright (C) 2014 Seagate Technology.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>

#include "kinetic.h"
#include "noop.h"
#include "put.h"

typedef struct _Arguments {
    char host[HOST_NAME_MAX];
    int port;
    int nonBlocking;
    int useTls;
    int64_t clusterVersion;
    int64_t identity;
    char key[KINETIC_MAX_KEY_LEN];
} Arguments;

int main(int argc, char** argv)
{
    int status = -1;
    int opt;
    int optIndex = 0;

    // Create an ArgP processor to parse arguments
    Arguments cfg = {
        .host = "localhost",
        .port = KINETIC_PORT,
        .nonBlocking = false,
        .useTls = false,
        .clusterVersion = 0,
        .identity = 1,
        .key = "asdfasdf",
    };

    struct option long_options[] =
    {
        {"non-blocking", no_argument,      &cfg.nonBlocking, true},
        {"blocking",    no_argument,       &cfg.nonBlocking, false},
        {"tls",         no_argument,       &cfg.port,        KINETIC_TLS_PORT},
        {"host",        required_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "npgdh:z:", long_options, &optIndex)) != -1)
    {
        // Parse options until we reach the end of the argument list
        if (opt != -1)
        {
            switch (opt)
            {
                case 0:
                    // If this option set a flag, do nothing else now.
                    if (long_options[optIndex].flag != 0)
                        break;
                    printf ("option %s", long_options[optIndex].name);
                    if (optarg)
                        printf (" with arg %s", optarg);
                    printf ("\n");
                    break;
                case 'h':
                    strcpy(cfg.host, optarg);
                    break;
                case '?':
                    // getopt_long already printed an error message.
                    break;
                default:
                    abort ();
            }
        }
    }

    // Execute all specified operations in order
    for (;optind < argc; optind++)
    {
        char* op = argv[optind];

        if (strcmp("noop", op) == 0)
        {
            printf("\n"
                   "Executing NoOp w/configuration:\n"
                   "-------------------------------\n"
                   "  host: %s\n"
                   "  port: %d\n"
                   "  non-blocking: %s\n"
                   "  clusterVersion: %lld\n"
                   "  identity: %lld\n"
                   "  key: '%s'\n",
                cfg.host,
                cfg.port,
                cfg.nonBlocking ? "true" : "false",
                (long long int)cfg.clusterVersion,
                (long long int)cfg.identity,
                cfg.key);
            status = NoOp(cfg.host, cfg.port, cfg.clusterVersion, cfg.identity, cfg.key);
            if (status == 0)
            {
                printf("\nNoOp executed successfully!\n\n");
            }
            else
            {
                printf("\nNoOp operation failed! status=%d\n\n", status);
                return -1;
            }
        }
        else
        {
            printf("\nSpecified operation is invalid!\n");
            return -1;
        }

    }

    return status;
}