/*
 * Copyright (C) 2024.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

FILE_LICENCE ( GPL2_OR_LATER );

#include <stdio.h>
#include <time.h>
#include <ipxe/command.h>
#include <ipxe/parseopt.h>

/** @file
 *
 * Date and time command
 *
 */

/** "date" options */
struct date_options {};

/** "date" option list */
static struct option_descriptor date_opts[] = {};

/** "date" command descriptor */
static struct command_descriptor date_cmd =
	COMMAND_DESC ( struct date_options, date_opts, 0, 0, "" );

/**
 * "date" command
 *
 * @v argc		Argument count
 * @v argv		Argument list
 * @ret rc		Return status code
 */
static int date_exec ( int argc, char **argv ) {
	struct date_options opts;
	time_t now;
	struct tm *tm;
	int rc;

	/* Parse options */
	if ( ( rc = parse_options ( argc, argv, &date_cmd, &opts ) ) != 0 )
		return rc;

	/* Get current time */
	now = time ( NULL );

	/* Convert to broken-down time */
	tm = gmtime ( &now );
	if ( ! tm ) {
		printf ( "Unable to get current time\n" );
		return -1;
	}

	/* Display date and time */
	printf ( "%04d-%02d-%02d %02d:%02d:%02d UTC\n",
		 ( tm->tm_year + 1900 ), ( tm->tm_mon + 1 ), tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec );

	return 0;
}

/** "date" command */
COMMAND ( date, date_exec );