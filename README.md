# UDP downloader

A project for Computer Networks at university.

To compile, type `make`, to remove temporary `.o` files, type `make clean`, to remove `.o` files and the program itself, type `make distclean`.

To run the program, type `./transport IP PORT FILENAME FILESIZE`, e.g. `./transport 156.17.4.30 40001 test 2137`.

As of 19.05.2019, the dedicated server runs at `156.17.4.30` on ports `40001`, `40002`, `40003`, but I recommend to test the program running the `server` with the following syntax: `./server PORT`, e.g. `./server 40001` and using local IP (`127.0.0.1`).

Unfortunately I don't have the source of the `server`, because it was provided by the lecturer.

Full specification (in polish) is in `p3.pdf`.