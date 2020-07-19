 <img height="150" src="https://github.com/pastaalforno/SharedEditor/blob/master/Client/images/scriba_logo_cropped.png">

# Scriba: Jot down your ideas, share them with the world
A **collaborative real-time editor** written in C++ using Qt GUI framework.  
It is based on a distributed data structure called Conflict-free Replicated Data Type (**CRDT**) and implements, in particular, [LSEQ](https://hal.archives-ouvertes.fr/hal-00921633/document) strategy.

<p align="center">
 <img src="https://github.com/pastaalforno/SharedEditor/blob/master/Resources/demo.gif">
</p>

## Motivation
Project developed in the context of *Programmazione di Sistema* (Operating System Design and Programming) course at Politecnico di Torino. Main features include:

* **Reliability**
  * Open files are saved every 5 seconds on the database to avoid file loss in case of server crash
  * If the server crashes, the user is kicked out of the application, no risk of losing work done
* **Scalability**
  * No CRDT data structure in the server, symbols stored unsorted to avoid overload of CRDT algorithm (performed only on clients)
  * Data (user information, images, files) stored in *MongoDB*: easy replication and sharding
* **Security**
  * TLS employed through *QSslSocket*
  * Users’ password hashed before being stored in database using *argon2* hash function (through *Libsodium* library)
* **Deployability**
  * *Docker* used to easily deploy server and database, without manual dependencies handling
  * Separate containers for server and database, to allow independent replication and balancing of resources

Read [project_requirements.pdf](project_requirements.pdf) (Italian only) for a detailed description of the requirements.

## Installation
Qt libraries need to be installed locally (take a look at https://doc.qt.io/qt-5/gettingstarted.html) to build and run the client application. 
Two options are available:
* Starting from the root folder of the project and using ```make```. In Linux it can be achieved as follows:
```
mkdir build && cd build
qmake ../Client/Client.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug
/usr/bin/make -f Makefile qmake_all
/usr/bin/make
```
* Using Qt Creator: just import the project and run it.  

Nothing is required for the server since it can be launched directly in the Docker container.

## Usage

### Server
To start the server and MongoDB containers, that will be listening respectively on port ```1500``` and ```27017```:

```
bash docker-start.sh
```

### Client
The client application, that will attempt to connect to ```localhost:1500```, can be run with the command:
```
./build/Client
```
To use a custom address and port:
```
./build/Client <ip_addr> <port_number>
```
## Authors

* **Enrico Loparco** - *enrico.loparco@studenti.polito.it*
* **Giuseppe Pastore** - *s257649@studenti.polito.it*

## License
[MIT](https://github.com/pastaalforno/SharedEditor/blob/master/LICENSE.md)
