# OSproject3

===========================
Project Team member's names
===========================
1. Andrew Florial
2. Phalguna Rupanagudi
3. Eduardo Alvarez

========================
Contents of Tar Archive:
========================
1. Makefile         
2. fat32.c     
3. fat32.h    
4. info.c        
5. misc.c
6. part1.c
7. part2.c
8. part3.c           

=====================
Makefile instructions
=====================

To build: 
make

To clean: 
make clean

To run:
./a.out fat32.img

===========
Known bugs: 
===========
1. All creations of files and directories are converted to lower case before storage. So if you tried to input "creat AAA" and then "creat aaa", the program would say that the file already exists.

2. Preexisting files with extensions are not handled correctly. For example, if you tried "rm fatspec.pdf", it will not remove. But, if you created a file with an extension and then removed it, the program handles it correctly. "creat hello.pdf" and then "rm hello.pdf" works correctly. 

====================
Additional comments:
==================== 
N/A

==================
Problem statement:
==================
For this project, you will design and implement a simple, user-space, shell-like utility that is capable of
interpreting a FAT32 file system image. The program must understand the basic commands to
manipulate the given file system image, must not corrupt the file system image, and should be robust.
You may not reuse kernel file system code and you may not copy code from other file system utilities.

=============================
Steps taken to solve problem:
=============================
GitHub repository log:
1. Initial commit
2. layout
3. .h 
4. main
5. changes
6. start
7. implementing ls
8. hot fixed ls
9. formatted ls
10. moved format to misc
11. ls
12. ls
13. updated cd
14. cd updates
15. some helper functions
16. stuff
17. finished part 1
18. part 1 complete
19. helper functions
20. changes
21. read
22. mkdir working
23. part2
24. part 2 additions
25. part 3
26. merged part 2 and part 3
27. successfully merged and ran part 2 and 3
28. problems with read
29. finish write
30. write fixes
31. added error messages
32. linprog compatible
33. more comment fixes
34. cd fixes
35. blue directories
36. more fixes



============
Assumptions:
============
1. File and directory names will not contain spaces
2. File and directory names will be names (not paths) within the current working directory
3. STRING will always be contained within " characters

==================
Division of Labor:
==================
Andrew Florial
- Part 1
- Part 2
- Part 3
- Troubleshooting
- Documentation

Phalguna Rupanagudi
- Part 1
- Part 2
- Part 3
- Troubleshooting
- Documentation

Eduardo Alvarez
- Part 1
- Part 2
- Part 3
- Troubleshooting
- Documentation
