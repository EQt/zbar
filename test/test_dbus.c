/*------------------------------------------------------------------------
 *  Copyright 2019 (c) Mauro Carvalho Chehab <mchehab+samsung@kernel.org>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <dbus/dbus.h>

#define ZBAR_INTERFACE "org.linuxtv.Zbar1.Code"
#define ZBAR_SIGNAL_CODE "Code"
#define ZBAR_SIGNAL_TYPE "Type"
#define ZBAR_SIGNAL_DATA "Data"

int main(void)
{
    DBusMessage* msg;
    DBusMessageIter args, entry, dict, val;
    DBusConnection* conn;
    DBusError err;
    char *str, *property;

    // initialise the error value
    dbus_error_init(&err);

    // connect to the DBUS system bus, and check for errors
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (!conn) {
        fprintf(stderr, "Connection Null\n");
        return -1;
    }

    dbus_bus_add_match(conn,
		       "type='signal',interface='" ZBAR_INTERFACE "'",
		       &err);
   dbus_connection_flush(conn);
   if (dbus_error_is_set(&err)) {
      fprintf(stderr, "Match Error (%s)\n", err.message);
      exit(1);
   }
// loop listening for signals being emmitted
   printf("Waiting for Zbar events\n");
   while (true) {
      // non blocking read of the next available message
      dbus_connection_read_write(conn, 0);
      msg = dbus_connection_pop_message(conn);

      // loop again if we haven't read a message
      if (NULL == msg) {
         sleep(1);
         continue;
      }

      // check if the message is a signal from the correct interface and with the correct name
      if (dbus_message_is_signal(msg, ZBAR_INTERFACE, ZBAR_SIGNAL_CODE)) {
         // read the parameters
         if (!dbus_message_iter_init(msg, &args))
            fprintf(stderr, "Message has no arguments!\n");
         else if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&args))
            fprintf(stderr, "Argument is not array!\n");
         else {
            while (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_INVALID) {
              dbus_message_iter_recurse(&args, &entry);
              if (DBUS_TYPE_DICT_ENTRY != dbus_message_iter_get_arg_type(&entry)) {
                fprintf(stderr, "Element is not dict entry!\n");
              } else {
                while (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_INVALID) {
                  dbus_message_iter_recurse(&entry, &dict);
                  if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&dict)) {
                    fprintf(stderr, "Dict Entry is not string!\n");
                  } else {
                    dbus_message_iter_get_basic(&dict, &property);
                    if (strcmp(property, ZBAR_SIGNAL_TYPE) == 0) {
                      dbus_message_iter_next(&dict);
                      dbus_message_iter_recurse(&dict, &val);
                      dbus_message_iter_get_basic(&val, &str);
                      printf("Type = %s\n", str);
                    } else if (strcmp(property, ZBAR_SIGNAL_DATA) == 0) {
                      dbus_message_iter_next(&dict);
                      dbus_message_iter_recurse(&dict, &val);
                      dbus_message_iter_get_basic(&val, &str);
                      printf("Value = %s\n", str);
                    }
                  }
                  dbus_message_iter_next(&entry);
                }
              }
              dbus_message_iter_next(&args);
            }
         }
      }


      // free the message
      dbus_message_unref(msg);
   }
   return 0;
}