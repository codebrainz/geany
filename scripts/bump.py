#!/usr/bin/env python
#
# Copyright 2013 Matthew Brush <mbrush@codebrainz.ca>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.
#

"""
This script updates various files in Geany's source tree depending
which options it is given. An example invocation might look like::

    $ scripts/bump.py --version="1.42.6" \
                      --codename="FooBar" \
                      --api=512 \
                      --abi=256 \
                      --copyright-year

That would replace the version number, codename, api/abi versions and
the copyright years in (mostly) all files.
"""

import os
import re
import sys
from datetime import datetime
from optparse import OptionParser

SRCDIR = os.getcwd()

def file_replace(path, pattern, repl):
    " Replace all occurences of pattern with repl in file at path. "
    text = open(path).read()
    text = re.sub(pattern, repl, text, re.MULTILINE)
    open(path, 'w').write(text)

def src_path(*args):
    " Build a path out of args list under SRCDIR. "
    return os.path.join(SRCDIR, *args)

def update_codename(codename):
    pat = '#\s*define\s+GEANY_CODENAME\s+"[^"]+"'
    repl = '#define GEANY_CODENAME "%s"' %codename
    #src/geany.h
    file_replace(src_path('src', 'geany.h'), pat, repl)

def update_version(version):

    ver_pat = '([0-9]+)\.([0-9]+)(\.([0-9]+))?'
    ver_pat_quad = '([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)'
    ver_com_pat_quad = '([0-9]+),([0-9]+),([0-9]+),([0-9]+)'

    #split out version components
    ver_fields = version.split('.')
    major_ver, minor_ver = ver_fields[0:2]
    micro_ver = ver_fields[2] if len(ver_fields) > 2 else 0
    major_ver = int(major_ver)
    minor_ver = int(minor_ver)
    micro_ver = int(micro_ver)

    #configure.ac
    file_replace(src_path('configure.ac'),
                 r'AC_INIT\(\s*\[\s*Geany\s*\]\s*,\s*\[\s*%s\s*\]\s*,' % ver_pat,
                  'AC_INIT([Geany], [%s],' % version)

    #geany.nsi
    file_replace(src_path('geany.nsi'),
                 r'!define\s+PRODUCT_VERSION\s+"%s"' % ver_pat,
                  '!define PRODUCT_VERSION "%s"' % version)
    file_replace(src_path('geany.nsi'),
                 r'!define\s+PRODUCT_VERSION_ID\s+"%s"' % ver_pat_quad,
                  '!define PRODUCT_VERSION_ID "%d.%d.%d.0"' % (
                    major_ver, minor_ver, micro_ver))

    #geany_private.rc
    file_replace(src_path('geany_private.rc'),
                 r'#define\s+VER_FILEVERSION\s+%s' % ver_com_pat_quad,
                  '#define VER_FILEVERSION %d,%d,%d,0' % (
                    major_ver, minor_ver, micro_ver))
    file_replace(src_path('geany_private.rc'),
                 r'#define\s+VER_FILEVERSION_STR\s+"%s"' % ver_pat,
                  '#define VER_FILEVERSION_STR "%s"' % version)

    #geany.exe.manifest
    file_replace(src_path('geany.exe.manifest'),
                 r'<assemblyIdentity\s+version\s*=\s*"%s"' % ver_pat_quad,
                  '<assemblyIdentity\n\t\tversion="%d.%d.%d.0"' % (
                    major_ver, minor_ver, micro_ver))

    #win32-config.h
    file_replace(src_path('win32-config.h'),
                 r'#define\s+VERSION\s+"%s"' % ver_pat,
                  '#define VERSION "%s"' % version)

    #wscript
    file_replace(src_path('wscript'),
                 r"APPNAME\s*=\s*'geany'\nVERSION\s*=\s*['\"]%s['\"]" % ver_pat,
                  "APPNAME = 'geany'\nVERSION = '%s'" % version)

    #doc/geany.txt
    file_replace(src_path('doc', 'geany.txt'),
                 r'\.\. \|\(version\)\| replace:: %s' % ver_pat,
                  '.. |(version)| replace:: %s' % version)

def update_api_version(api_version):
    api_version = int(api_version)
    #src/plugindata.h
    file_replace(src_path('src', 'plugindata.h'),
                 r'#define\s+GEANY_API_VERSION\s+[0-9]+',
                  '#define GEANY_API_VERSION %d' % api_version)

def update_abi_version(abi_version):
    abi_version = int(abi_version)
    #src/plugindata.h
    file_replace(src_path('src', 'plugindata.h'),
                 r'#define\s+GEANY_ABI_VERSION\s+\(\s*[0-9]+',
                  '#define GEANY_ABI_VERSION (%d' % abi_version)

def update_copyright_year():
    year = datetime.now().year
    # future: 
    #   could support 'Copyright (c)' and pickup more files (mostly non-Geany)
    pat = r'Copyright\s+[0-9]{4}\s*-\s*([0-9]{4})' # year range
    pat2 = r'Copyright\s+([0-9]{4})\s+[^-]+'       # single year
    skip_files = [ src_path('doc', 'geany.html'),
                   src_path('scintilla'),
                   src_path('.git'),
                   src_path('scripts', 'bump.py') ]
    def is_skip_file(fn):
        for sf in skip_files:
            if sf in fn:
                return True
        return False
    for path, dirs, files in os.walk(SRCDIR):
        if os.path.basename(path).startswith('.') or is_skip_file(path):
            continue
        for f in files:
            fpath = os.path.join(path, f)
            if is_skip_file(fpath):
                continue
            text = open(fpath).read()
            # if range, update end year
            for match in re.finditer(pat, text):
                text = text[:match.start(1)] + str(year) + text[match.end(1):]
            # if single year and not current year, make range to current
            for match in re.finditer(pat2, text):
                fyear = int(match.group(1))
                if fyear < year:
                    text = text[:match.start(1)] + str(fyear) + \
                            '-' + str(year) + text[match.end(1):]
            open(fpath, 'w').write(text)

def main(args):

    p = OptionParser(
        usage='Usage: %prog [options] [SRCDIR]',
        description='Use this script to update version-related ' +
                    'information in the source tree. The `SRCDIR\' ' +
                    'option specifies the source directory to work ' +
                    'on, defaulting to the current directory.',
        epilog='Written by Matthew Brush <mbrush@codebrainz.ca>')

    p.add_option('--codename', metavar='CODENAME', dest='codename',
        help='the codename to update')
    p.add_option('--version', metavar='VERSION', dest='version',
        help='the Geany version to update')
    p.add_option('--api', metavar='APIVERSION', dest='api',
        help='the plugin API version to update')
    p.add_option('--abi', metavar='ABIVERSION', dest='abi',
        help='the plugin ABI version to update')
    p.add_option('--copyright-year', dest='copyright', action='store_true',
        help='update the copyright years')

    opts, args = p.parse_args(args)

    if len(args) > 1:
        SRCDIR = args[1]

    if opts.version:
        update_version(opts.version)

    if opts.codename:
        update_codename(opts.codename)

    if opts.api:
        update_api_version(opts.api)

    if opts.abi:
        update_abi_version(opts.abi)

    if opts.copyright:
        update_copyright_year()

    return 0

if __name__ == "__main__": sys.exit(main(sys.argv))
