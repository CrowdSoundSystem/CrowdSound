# Crowdsound
Seven for the She-lord in her hall of stone (Michaela)<br />
Nine for the Moral Men doomed to die, (Bros)<br />
One for the Pi on his dark thrown<br />
In the Land of Waterloo where the Shadows lie.<br />
One Sound to rule them all, One Sound to find them,<br />
One Sound to bring them all and in the crowd bind them<br />
In the Land of Waterloo where the Shadows lie

## Setup
The crowdsound repo uses submodules for the specific components.
Thus, to initialize the repo:
```
$ git submodule update --init
```

Additionally, crowdsound depends on gRPC which is not always present
in a distributions package manager. First, we will want to get and install
protobuf3:

```
$ cd [projects / folder of your preference]
$ git clone git@github.com:google/protobuf
$ git checkout tags/v3.0.0-beta-1
$ ./autogen.sh
$ ./configure
$ make
$ sudo make install
```

Next, we'll want to install gRPC:

```
$ cd [projects / folder of your preference]
$ git clone git@github.com:grpc/grpc
$ get checkout tags/release-0_11_1
$ git submodule update --init
$ make
$ sudo make install
```

You may receive a warning about no protobuf being installed. This is okay,
as long as `which protoc` yields the location of protoc.

If your system does not have boost installed, then you should be able to
retrieve it from your package manager:
```
$ [Ubuntu | Debian] sudo apt-get install libboost-dev
$ [Fedora | CentOS] sudo yum install -y boost-devel
$ [Arch]            pacman -S boost
```

## Building
Building is as simple as running `CMake`. However, it is recommended
to create a seperate directory for building. For example, from the project root:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Testing
There isn't really any testing done yet (if ever?) in an automated way.

## Running
To run the server, simple build the project (as shown above), and `bin/crowdsound`
For example from the build directory:

```
$ make
$ bin/crowdsound
```
