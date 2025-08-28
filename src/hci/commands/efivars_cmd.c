/*
 * Copyright (C) 2023 Michael Brown <mbrown@fensystems.co.uk>.
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
 *
 * You can also choose to distribute this program under the terms of
 * the Unmodified Binary Distribution Licence (as given in the file
 * COPYING.UBDL), provided that you have satisfied its requirements.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <ipxe/command.h>
#include <ipxe/parseopt.h>
#include <ipxe/efi/efi.h>
#include <ipxe/efi/efi_strings.h>

FILE_LICENCE ( GPL2_OR_LATER_OR_UBDL );

/* Record errors as though they come from EFI settings */
#undef ERRFILE
#define ERRFILE ERRFILE_efi_settings

/** @file
 *
 * EFI variable enumeration command
 *
 */

/** "efivars" options */
struct efivars_options {};

/** "efivars" option list */
static struct option_descriptor efivars_opts[] = {};

/** "efivars" command descriptor */
static struct command_descriptor efivars_cmd =
	COMMAND_DESC ( struct efivars_options, efivars_opts, 0, 0, NULL );

/**
 * The "efivars" command
 *
 * @v argc		Argument count
 * @v argv		Argument list
 * @ret rc		Return status code
 */
static int efivars_exec ( int argc, char **argv ) {
	struct efivars_options opts;
	EFI_RUNTIME_SERVICES *rs;
	CHAR16 *buf;
	CHAR16 *tmp;
	UINTN size;
	EFI_GUID guid;
	EFI_STATUS efirc;
	int rc;
	int count = 0;

	/* Parse options */
	if ( ( rc = parse_options ( argc, argv, &efivars_cmd, &opts ) ) != 0 )
		return rc;

	/* Check if EFI is available */
	if ( ! efi_systab ) {
		printf ( "EFI system table not available\n" );
		return -ENOTSUP;
	}

	rs = efi_systab->RuntimeServices;

	/* Allocate single wNUL for first call to GetNextVariableName() */
	size = sizeof ( buf[0] );
	buf = zalloc ( size );
	if ( ! buf )
		return -ENOMEM;

	printf ( "EFI Variables:\n" );

	/* Iterate over all variables */
	while ( 1 ) {

		/* Get next variable name */
		efirc = rs->GetNextVariableName ( &size, buf, &guid );
		if ( efirc == EFI_BUFFER_TOO_SMALL ) {
			tmp = realloc ( buf, size );
			if ( ! tmp ) {
				rc = -ENOMEM;
				break;
			}
			buf = tmp;
			efirc = rs->GetNextVariableName ( &size, buf, &guid );
		}
		if ( efirc == EFI_NOT_FOUND ) {
			rc = 0;
			break;
		}
		if ( efirc != 0 ) {
			rc = -EEFI ( efirc );
			printf ( "Error fetching variable name: %s\n", strerror ( rc ) );
			break;
		}

		/* Print variable name and GUID */
		printf ( "%s:%ls\n", efi_guid_ntoa ( &guid ), buf );
		count++;
	}

	printf ( "\nTotal: %d variables\n", count );

	/* Free temporary buffer */
	free ( buf );

	return rc;
}

/** "efivars" command */
COMMAND ( efivars, efivars_exec );