Introduction
============

A simple syslog exerciser, much like tools such as 'Postal' for SMTP email.

Ever wondered just how many syslog messages your syslog infrastructure can really handle? Well now you can find out. logsmash is intended to be used as a debugging and testing tool to generate syslog data as quickly as possible, allowing you to find out where there might be bottlenecks, and what might happen if parts of your syslog infrastructure fail.

Building
--------

logsmash is written in C for maximum throughput. A Makefile is included: just run "make" to build it.

logsmash should build and run on any vaguely recent POSIX-ish OS, although I've only tried it on fairly recent versions of Linux.

Usage
-----

**WARNING** logsmash can generate thousands of syslog messages **PER SECOND** when it's not rate limited. Make sure you have permission before you run it for real. If syslog is configured to log to local disk, you may find you fill up /var/log very quickly. If syslog is configured to log via. a network, you may find your sysadmin and/or network admin take a sudden dislike to you.

Logsmash can take a few simple options

* -s : Output generated messages to stdout as well as sending them to syslog.
* -c : Write the current message count to stdout. You can not use -s & -c together.
* -q : Quite mode. Do not send the messages to syslog. Useful with -c to see just how quickly logsmash can generate messages before you try it for real.
* -l [n] : Rate limit to 'n' messages per. second. Useful for creating slightly more realistic scenarios.

