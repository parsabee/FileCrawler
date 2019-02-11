To build and install file_crawler, after cloning the repository, run the following commands in shell:  
```
$ autoreconf -if
$ ./configure
$ make
$ make install
``` 
Note that you need to have the following packages installed:  
autotools-dev  
automake  

If you're on mac use homebrew:
```
$ brew install autotools-dev
$ brew install automake
```
If on linux:
```
$ sudo apt-get install autotools-dev
$ sudo apt-get install automake
```
or similar package managers.
# Synopsis
```
Usage: file_crawler pattern [directory] ...
```
N.B. the pattern must be enclosed in '' (a pair of single quotations), otherwise bash will interpret it.
# File Crawler

Disks attached to laptops and desktops have become absurdly large. This generally leads users to  
accumulate large numbers of files in a large number of directories – i.e. rather than manage the  
use of disk space, it is easier to simply create more directories, retaining older versions of files.  
Some operating systems have addressed this by continuously indexing the names of files in the  
file system, and then providing the ability to query the index – e.g. Google desktop. Server  
systems generally do not continuously index file names, instead providing one or more  
applications for searching the file system for files whose names match the desired patterns.  
Linux, for example, provides the find application for general traversal of the file system – see the  
man page for more details.  
The syntax for the find application is extremely difficult to master; it is also single threaded. The aim of this  
tool is to be a high-performance application that will recursively crawl  
through a set of specified directories, looking for filenames that match a specified bash pattern.  
For very large software systems, a singly-threaded application to crawl the directories may take a  
very long time.  
# Specifications

This program understands the following arguments:  
__pattern__     indicates a bash pattern that filenames must match  
__directory__     directory to be recursively crawled for matching filenames  
The usage string is:     file_crawler pattern [directory] …  
If a directory is not specified, the application searches in the current directory, “.”.  
The application uses the following environment variable when it runs:
CRAWLER_THREADS – if this is defined, it specifies the number of worker threads that the  
application creates; if it is not defined, then two (2) worker threads should be created.  
# Credit

Created by Parsa Bagheri  
This software uses open source material.  
I am grateful to the developers of these material for their contirbutions to open source     
The LICENSE and Copyright information can be found in ./FileCrawler/src/LICENSE  
The link to the GitHub repository: https://github.com/jsventek/ADTsv2
