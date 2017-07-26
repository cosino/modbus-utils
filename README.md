MODBUS utils
============

MODBUS utils are intended to be used for a quick test of a generic
MODBUS slave.

Note: ths projects is still in beta version so refer to the "known bugs"
      section for further info!

Compile
-------

In order to compile the code you need the libmodbus library from
http://libmodbus.org/.

For Debian and Ubuntu users the library can be easily obtained by using the
usual apt-get (or equivalent) command as below:

    $ sudo apt-get install libmodbus-dev

Also you may wish installing also the next packages (if not already present):

    $ sudo apt-get install gcc make pkg-config

Then download the code and enter into the code's directory:

    $ git clone https://github.com/cosino/modbus-utils.git
    $ cd modbus-utils

Now you can compile as usual by using make:

    $ make


Usage
-----

Usage is quite simple, just use command:

    $ ./modbus-dump 10 1 39
    modbus-dump: reg[1]=2562/0x0a02
    modbus-dump: reg[2]=1/0x0001
    modbus-dump: reg[3]=6/0x0006
    modbus-dump: reg[4]=8464/0x2110
    modbus-dump: reg[5]=65344/0xff40
    ...

to dump registers from 1 to 39 at slave address 10.

Use command:

    $ ./modbus-set 10 12 1000 45000

to write 1000 into register 12 and 45000 into register 13 at slave
address 10.

And use command:

    $ ./modbus-get 10 13 12
    modbus-get: reg[13]=45000/0xafc8
    modbus-get: reg[12]=1000/0x03e8

to read registers 13 and 12 at slave address 10.

By default RTU connection has the following options:

    rtu:/dev/ttyUSB0,115200,8,N,1

and you can modify it by using for instance:

    $ ./modbus-get -d rtu:/dev/ttyUSB1,9600,8E1 10 13 12

to specify serial the device /dev/ttyUSB1 set at 9600 baud, 8 data
bits, even parity and 1 bit stop.


Known bugs
----------

* Only RTU connection is supported.
* Only read/write of holding registers are supported
* Broadcast messages are not supported
