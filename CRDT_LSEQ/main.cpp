#include <iostream>
#include "NetworkServer.h"
#include "SharedEditor.h"

int main() {
    NetworkServer network;
    SharedEditor ed1(network);
    SharedEditor ed2(network);
    SharedEditor ed3(network);

    ed1.localInsert(0,'c');
    ed1.localInsert(1,'a');
    ed1.localInsert(2,'t');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(1,'h');
    ed2.localErase(1);
    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(1,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(4,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(4,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(4,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(6,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(6,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(6,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(6,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;
    ed1.localInsert(6,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(6,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(6,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;
    ed1.localInsert(6,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;
    ed1.localInsert(7,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(9,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;
    ed1.localInsert(9,'h');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(9,'A');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(9,'A');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(12,'A');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;


    return 0;
}