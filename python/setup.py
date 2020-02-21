#!/usr/bin/env python
from setuptools import setup
from setuptools.extension import Extension
import sys
from os import path


setup(
    name = 'zbar',
    version = '0.22.2',
    author = 'Jeff Brown',
    author_email = 'spadix@users.sourceforge.net',
    url = 'http://zbar.sourceforge.net',
    description = 'read barcodes from images or video',
    license = 'LGPL',
    long_description = open('README').read(),
    classifiers = [
        'License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)',
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Environment :: Console',
        'Environment :: X11 Applications',
        'Environment :: Win32 (MS Windows)',
        'Operating System :: POSIX',
        'Operating System :: Unix',
        'Operating System :: Microsoft :: Windows',
        'Topic :: Communications',
        'Topic :: Multimedia :: Graphics',
        'Topic :: Software Development :: Libraries',
    ],
    ext_modules = [
        Extension('zbar', [
                'zbarmodule.c',
                'enum.c',
                'exception.c',
                'symbol.c',
                'symbolset.c',
                'symboliter.c',
                'image.c',
                'processor.c',
                'imagescanner.c',
                'decoder.c',
                'scanner.c',
                ],
            libraries = [ 'zbar' ],
            include_dirs=['../include'],
            library_dirs=['../zbar/.libs'],
            extra_link_args=['-Wl,-rpath=' + path.abspath(
                path.join(path.dirname(__file__), '..', 'zbar', '.libs')
            )] if sys.platform != 'win32' else [],
        ),
    ],
)
