To build and install file_crawler, after cloning the repository, cd into repositories directory and run the following commands:  
```
$ autoreconf -if
$ ./configure
$ make
$ sudo make install
``` 
Note that you need to have the following packages installed:  
autoconf  
automake  

If you're on mac use homebrew:
```
$ brew install autoconf
$ brew install automake
```
If on linux:
```
$ sudo apt-get install autoconf
$ sudo apt-get install automake
```
or similar package managers.
# Synopsis
```
Usage: file_crawler pattern [directory] ...
```
N.B. the pattern must be enclosed in '' (a pair of single quotations), otherwise bash will interpret it.
# File Crawler
Implementaion by Parsa Bagheri  
Design by Prof. Joe Sventek  

Disks attached to laptops and desktops have become absurdly large. This generally leads users to  
accumulate large numbers of files in a large number of directories – i.e. rather than manage the  
use of disk space, it is easier to simply create more directories, retaining older versions of files.  
Some operating systems have addressed this by continuously indexing the names of files in the  
file system, and then providing the ability to query the index – e.g. Google desktop. Server  
systems generally do not continuously index file names, instead providing one or more  
applications for searching the file system for files whose names match the desired patterns.  
Linux, for example, provides the *find* application for general traversal of the file system – see the  
man page for more details.  
The syntax for the find application is extremely difficult to master; it is also single threaded. For  
very large software systems, a singly-threaded application to crawl the directories may take a very   
long time.  
The aim of *file_crawler* is to be a high-performance, multi-threaded application, with easier syntax  
than *find*, that will recursively crawl through a set of specified directories, looking for filenames   
that match a specified bash pattern.    
# Specifications

This program understands the following arguments:  
__pattern__:     indicates a bash pattern that filenames must match  
__directory__:     directory to be recursively crawled for matching filenames  
The usage string is:     file_crawler pattern [directory] …  
If a directory is not specified, the application searches in the current directory, “.”.  
The application uses the following environment variable when it runs:
CRAWLER_THREADS – if this is defined, it specifies the number of worker threads that the  
application creates; if it is not defined, then two (2) worker threads should be created.  
# Credit

Created by Parsa Bagheri
This piece of software uses open source libraries,
I am grateful and appreciate the developers of theses libraries, and their contribution to open source.
The link to the Github repository of these libraries is provided below.
The LICENSE and Copyright information can be found in ./FileCrawler/src/LICENSE

files: iterator.c iterator.h linkedlist.c linkedlist.h orderedset.c orderedset.h
       tsiterator.c tsiterator.h tslinkedlist.c tslinkedlist.h tsorderedset.c tsorderedset.h
The link to the GitHub repository: https://github.com/jsventek/ADTsv2/{files}
