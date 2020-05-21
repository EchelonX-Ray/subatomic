**IMPORTANT: This program is pre-release.  It is under heavy development with many incomplete and broken parts.  This includes this README file.**  
  
# Read this file for important information on the Subatomic project  
  
**File Structure:**  
  
/README.md - This file, where summary documentation is to be provied  
/LICENSE – The license under which this software is released.  I retain ownership of the copywrite.  
/CONTRIBUTING.md – My policy for accepting code contributions to this project  
/devel/ – Development tools  
/src/ – Source code  
/.git/ – Git files  
/.gitignore – Git ignore files  
/local/– A place to do local work.  This directory is not tracked by Git.  
  
**Build Instructions:**  
  
As of writing this, only a development build script exists.  
It can be used to build the program, but portability may be an issue.  
The script is located at “/devel/dev_build.sh”.  
Upon the successful completion of the script, the program  
binary will be placed at: /devel/subatomic.out  
  
1) cd devel &#35; Move the correct directory  
2) &#35; In the build script file ./dev_build.sh, there is a configuration section  
   &#35; This can be used to configure the build.  Adjust the parameters as needed.  
3) ./dev_build.sh &#35; Run the build script  
4) ./subatomic.out &#35; Run the built program  
