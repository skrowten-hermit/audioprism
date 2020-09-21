# Essentia build scripts

Use this wrapper to compile programs that link against essentia.

## Usage

Edit the `wscript` file and set `APP_NAME` at the top of the file.
Edit the `src/wscript` file and fill in the `sources` variable with
the source files making up 

Run

    ./waf configure

to configure the application. You must have essentia installed.
To compile your program and link it against essentia, run

    ./waf

you can optionally install the program using

    ./waf install


## Licence

This project is Copyright (c) 2016 Music Technology Group - 
Universitat Pompeu Fabra.
It can be used under the terms of the GNU Affero General
Public Licence, version 3.
See the COPYING.txt file for more details
