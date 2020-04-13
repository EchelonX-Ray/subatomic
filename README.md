IMPORTANT: This program is pre-release.  It is under heavy development with many incomplete and broken parts.  This includes this README file.

Read this file for important information on the Subatomic project

File Structure:

/README.md - This file, where summary documentation is to be provied
/LICENSE.0BSD.txt – A License under which this software is released.  I retain ownership of the copywrite.
/Contribution_Policy.md – My policy for accepting code contributions to this project
/devel/ – Development tools
/src/ – Source Code
/.git/ – Git files
/.gitignore – Git ignore files
/local/– A place to do local work.  This directory is not tracked by Git.

Build Instructions:

As of writing this, only a development build script exists.  
Only execute it from the directory “/devel/”.  
The script is located at “/devel/dev_build.sh”.  
So it may be executed by the command “./dev_build.sh”.  
Upon the successful completion of building the program, the program binary will be placed at: devel/subatomic.out

1) cd devel # Move the correct directory
2) ./dev_build.sh # Run the build script
3) ./subatomic.out # Run the built program

Execute the program with ./subatomic.out
