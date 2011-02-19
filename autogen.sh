#!/bin/sh

echo "Detecting autotools..."
#AM_VERSION="1.11.1"
if ! type aclocal-$AM_VERSION 1>/dev/null 2>&1; then
	AUTOMAKE=automake
	ACLOCAL=aclocal
	AUTOHEADER=autoheader
	AUTOCONF=autoconf
else
	ACLOCAL=aclocal-${AM_VERSION}
	AUTOMAKE=automake-${AM_VERSION}
	AUTOHEADER=autoheader-${AM_VERSION}
	AUTOCONF=autoconf-${AM_VERSION}
fi

LIBTOOLIZE=libtoolize

echo "Generating build scripts..."

$LIBTOOLIZE --copy --force
$ACLOCAL
#$AUTOHEADER
$AUTOMAKE --add-missing --copy
$AUTOCONF
rm -f config.cache
