#include <iostream>
#include "NetworkServer.h"
#include "SharedEditor.h"

/*
int main() {
    NetworkServer network;
    SharedEditor ed1(network);
    SharedEditor ed2(network);

    ed1.localInsert(0 , 'c');
    ed1.localInsert(1 , 'a');
    ed1.localInsert(2 , 't');

//    ed1.localInsert(0 , 'a');
//    ed1.localInsert(0 , 'c');
//    ed1.localInsert(2 , 't');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;
    ed1.debug();
//    std::cout << "***********";
//    ed2.debug();

    ed1.localInsert(1, 'h');
    ed1.debug();
    ed2.localErase(1);
    ed2.debug();

    ////////////////////////
    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;
    ed1.debug();

    std::cout << "-------------\n";
    ///////////////////////

    ed1.localInsert(0, 'a');
    ed2.localInsert(0, 'b');
    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;
    ed1.debug();

    for (int i = 0; i < 10; i++) {
        ed2.localInsert(0, 'A');
        network.dispatchMessages();
        ed1.debug();
    }
//    network.dispatchMessages();
//    ed1.debug();
    return 0;
}
*/
int main() {
//    srand(time(NULL));

    NetworkServer network;
    SharedEditor ed1(network);
    SharedEditor ed2(network);

//    // TEST 1
//    ed1.localInsert(0, 'h');
//    ed1.localInsert(1, 'a');
//    ed1.localInsert(2, 't');
//
//    network.dispatchMessages();
//    std::cout << "ed1: " << ed1.to_string() << std::endl;
//    std::cout << "ed2: " << ed2.to_string() << std::endl;
//
//    // commutativity
//    ed1.localInsert(0, 'c');
//    ed2.localErase(0);
//    network.dispatchMessages();
//    std::cout << "ed1: " << ed1.to_string() << std::endl;
//    std::cout << "ed2: " << ed2.to_string() << std::endl;
//
//    // idempotency
//    ed1.localErase(0);
//    ed2.localErase(0);
//
//    network.dispatchMessages();
//    std::cout << "ed1: " << ed1.to_string() << std::endl;
//    std::cout << "ed2: " << ed2.to_string() << std::endl;

//    // TEST 2: forcing the generation of random number by adding the following lines at the beginning of generateRandomNumBetween
//    /*
//     *  static int cnt = 0;
//        cnt++;
//        if (cnt == 1 || cnt == 2)
//            return 1;
//     */
//    ed1.localInsert(0, 'h');
//    ed2.localInsert(0, 'a');
//
//    network.dispatchMessages();
//    std::cout << "ed1: " << ed1.to_string() << std::endl;
//    std::cout << "ed2: " << ed2.to_string() << std::endl;
//
//    ed1.localInsert(1, 'W');
//    network.dispatchMessages();
//    std::cout << "ed1: " << ed1.to_string() << std::endl;
//    std::cout << "ed2: " << ed2.to_string() << std::endl;

    // TEST 3
    ed1.localInsert(0, 'h');
    ed1.localInsert(1, 'a');
    ed1.localInsert(1, 't');

    network.dispatchMessages();
    std::cout << "ed1: " << ed1.to_string() << std::endl;
    std::cout << "ed2: " << ed2.to_string() << std::endl;

    ed2.localInsert(2, 'T');
    network.dispatchMessages();
    std::cout << "ed1: " << ed1.to_string() << std::endl;
    std::cout << "ed2: " << ed2.to_string() << std::endl;

    return 0;
}