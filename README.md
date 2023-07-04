# scansample

File scanner with file IO limit.

How to build:
1. install python
2. install pip
3. install conan ( pip install conan )
4. generate ( cmake -DCMAKE_BUILD_TYPE=Debug .. )
5. build ( cmake --build . )

How to use:

scansample.exe SCAN_ROOT BYTES_PER_SEC

SCAN_ROOT has to be exist.

BYTES_PER_SEC can't be 0.

