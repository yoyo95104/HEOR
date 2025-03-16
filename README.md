# HEOR
a Network Stressing tool , Made for Scientific Research and Testing Servers With Permission
# Legal Notice
This Program Should not be used without Authorization and any Action Taken illegally is not endorsed By the Creator
# Installing 
You will Clone the repositry and enter the Directory
```
  git clone https://github.com/yoyo95104/HEOR/main
  cd HEOR
```
Then make a Build Directory and run cmake
```
  mkdir build
  cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release .
  make
```
You will get a file called HEOR , To run the Program Run this Program
```
  sudo chrt -f 99 ./HEOR
```
# Note 
This Program will not work on Windows \
This Program uses all Threads on the Computer and Creates Four Sockets on Each Thread , So use with care \
This Program is Made purely For Scientific Research
