Omegle MITM
===========

This is a program that sniffs conversations between random strangers on the 'Omegle' <http://omegle.com> random-pairing instant messaging service. It does this by establishing two connections, and then routing their messages to one another transparently, all the while recording them to log files (log.\*\*\*\*\*.txt) for generating a corpus for later analysis.

Omegle MITM logs only one conversation at a time for bot-detection avoidance and can therefore take a considerable amount of time to acquire logs. It is designed to be ran from multiple servers, although there is no intercommunication implemented natively yet. Log files must therefore be transferred manually.

#### Build

Omegle MITM depends upon boost, including boost-regex and boost-filesystem. These can be installed (Ubuntu 11.10) with `sudo apt-get install libboost1.46-dev libboost-regex1.46-dev libboost-filesystem1.46-dev libboost-system1.46-dev`.

Additionally, the libomegle library is required. This can be acquired from <https://github.com/personalcomputer/libomegle>. The makefile expects libomegle.so to be in the root Omegle-MITM directory, and Omegle.h (With directory Omegle/\*) to be in Omegle-MITM/src/include/. This is just a simple configuration, you'll probably want to quickly replace the relevant flags in the makefile for your own setup.

Once the dependencies are satisfied, Omegle-MITM can be compiled by running `make` from the src directory.
