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

    $ ./modbus-dump 10 reg 1 39
    modbus-dump: reg[1]=2562/0x0a02
    modbus-dump: reg[2]=1/0x0001
    modbus-dump: reg[3]=6/0x0006
    modbus-dump: reg[4]=8464/0x2110
    modbus-dump: reg[5]=65344/0xff40
    ...

to dump registers from 1 to 39 at slave address 10; or use the following to
do multiple queries on the same slave at address 1:

    $ ./modbus-dump 1 ibits 0 0 obits 0 1
    modbus-dump: ibit[0]=0/0x00
    modbus-dump: obit[0]=0/0x00
    modbus-dump: obit[1]=0/0x00

We got input bits and output bits at once.

Note that you can use the watch command as below to refresh the queries each
second:

    $ watch -n 1 ./modbus-dump -d tcp 1 ibits 0 0 obits 0 1

The output should be something as follow:

    Every 1.0s: ./modbus-dump -d tcp 1 ibits ...  tekkaman: Sat Feb  3 12:50:06 2018

    modbus-dump: ibit[0]=1/0x01
    modbus-dump: obit[0]=1/0x01
    modbus-dump: obit[1]=0/0x00

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

If you need to see each data byte exchanged with the remote device you can
use a double -D option argument as below:

    $ ./modbus-dump -DD -d rtu:/dev/ttymxc1,460800,8N1 1 0x1000 0x1002
    common.c[ 190]: check_common_opts: debug is on
    common.c[ 197]: check_common_opts: modbus rtu:/dev/ttymxc1,460800,8,N,1
    common.c[ 253]: modbus_client_connect: addr=1
    [01][03][10][00][00][03][01][0B]
    Waiting for a confirmation...
    <00><00><00><00><00>
    ERROR CRC received 0 != CRC calculated 71C0
    read error: Invalid CRC

In the above example we can see that we send a valid MODBUS request but the
remote device answered in a wrong way!

Note also that in last example I specified the registers by using hex values.


Known bugs
----------

* Only RTU connection is supported.
* Only read/write of holding registers are supported
* Broadcast messages are not supported
