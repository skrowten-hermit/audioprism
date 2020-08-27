#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import platform

APPNAME = 'essentia_sample'

top = '.'
out = 'build'


def options(ctx):
    ctx.load('compiler_cxx compiler_c python')
    ctx.recurse('src')

    ctx.add_option('--mode', action='store',
                   dest='MODE', default="release",
                   help='debug or release')

def configure(ctx):
    print('→ configuring the project in ' + ctx.path.abspath())

    # compiler flags
    ctx.env.CXXFLAGS = [ '-pipe', '-Wall' ]

    # force using SSE floating point (default for 64bit in gcc) instead of
    # 387 floating point (used for 32bit in gcc) to avoid numerical differences
    # between 32 and 64bit builds (see https://github.com/MTG/essentia/issues/179)
    ctx.env.CXXFLAGS += [ '-msse', '-msse2', '-mfpmath=sse' ]

    # define this to be stricter, but sometimes some libraries can give problems...
    #ctx.env.CXXFLAGS += [ '-Werror' ]

    if ctx.options.MODE == 'debug':
        print ('→ Building in debug mode')
        ctx.env.CXXFLAGS += [ '-g' ]

    elif ctx.options.MODE == 'release':
        print ('→ Building in release mode')
        ctx.env.CXXFLAGS += [ '-O2' ]

    else:
        raise ValueError('mode should be either "debug" or "release"')

    if sys.platform.startswith('linux'):
        # include -pthread flag because not all versions of gcc provide it automatically
        ctx.env.CXXFLAGS += [ '-pthread' ]

    ctx.load('compiler_cxx compiler_c')

    ctx.recurse('src')


def build(ctx):
    print('→ building from ' + ctx.path.abspath())

    # missing -lpthread flag on Ubuntu
    if platform.dist()[0] == 'Ubuntu':
        ext_paths = ['/usr/lib/i386-linux-gnu', '/usr/lib/x86_64-linux-gnu']
        ctx.read_shlib('pthread', paths=ext_paths)
        ctx.env.USES += ' pthread'

    ctx.recurse('src')

