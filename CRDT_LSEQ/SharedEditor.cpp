//
// Created by Giuseppe Pastore on 2019-05-06.
//

#include "SharedEditor.h"
#include "Symbol.h"
#include <algorithm>
#include <sstream>
#include <random>
#include <iostream>
int base[10]={32,64,128,256,512,1024,2048,4096,8192,16384};

int SharedEditor::getSiteId() const{
    return _siteId;
}

void SharedEditor::localInsert(int index, char value) {
    int max,min,r=0,i;
    Symbol s;
    std::vector<int> a,b,new_pos;

    //insert in version vector



    if (_symbols.size()>index) {
        if (!index) {
            a = {0};
        } else {
            a = _symbols[index - 1].getPos();
        }
        b = _symbols[index].getPos();


        std::cout<<"a: "<<_symbols[index - 1].getValue()<<std::endl;
        std::cout<<"b: "<<_symbols[index ].getValue()<<std::endl;
        /*

        if (b.size()>a.size())
            std::swap(a,b); //a sarà sempre il più grande
        max=a.size();
        min=b.size();
        for(i=0;i<min;i++){
            r*=10;
            new_pos.push_back((a[i]+b[i]+r)/2);
            r=(a[i]+b[i]+r)%2;
        }
        for (;i<max;i++){
            r*=10;
            new_pos.push_back((a[i]+r)/2);
            r=(a[i]+r)%2;
        }
        if (r) new_pos.push_back(5);*/
        s=Symbol(value,_siteId,_counter++,LSEQ_alloc(a,b));
    }
    else if(_symbols.size()==index){
        if (!index){
            s=Symbol(value,_siteId,_counter++,{1});
        }
        else{
            s=Symbol(value,_siteId,_counter++,{(_symbols[index-1].getPos()[0])+1});
        }
    }
    else{
        //errore
        return;
    }

    _symbols.insert(_symbols.begin()+index,s);
    _server.send(Message(1,_siteId, value,s.getId(),s.getPos()));

}

//size()>index
//  index=0 --> non esiste index-1;
//  devo calcolare index-1 e index;
//size == index
//  size=0 --> inserisco con pos=1;
//  pushback() --> non esiste index; tenere conto solo di index-1

//0 1 2 3 4 5 6 7 8 9
//1 2 3 4 5 6 7 8 9 10

SharedEditor::~SharedEditor(){
    _server.disconnect(this);
}

void SharedEditor::localErase(int index){
    Symbol s=_symbols[index];
    _symbols.erase(_symbols.begin()+index);
    _server.send(Message(0,_siteId, s.getValue(),s.getId(),s.getPos()));
}


void SharedEditor::process(const Message& m) {
    std::vector<int> pos = m.getPos();
    if (m.getType()) {
        auto i = std::stable_partition (_symbols.begin(), _symbols.end(),
                [pos](Symbol sym) ->bool{
            return (sym.getPos()<pos)==1;
        });
        _symbols.insert(i,Symbol(m.getValue(),m.getId().first,m.getId().second,m.getPos()));
    } else {
        auto i = find_if(_symbols.begin(), _symbols.end(),
                                  [pos](Symbol sym) {
                                      return sym.getPos() == pos;
                                  });

        if (i == _symbols.end())
            return;

        if ((*i).getId()==m.getId()){
            //lo devo eliminare dal vettore
            _symbols.erase(i);
    }
}
}

std::string SharedEditor::to_string(){
    std::stringstream s;
    for(auto p: _symbols){
        std::vector<int> c=p.getPos();
        s<<p.getValue()<<"(";
        for(auto i: p.getPos())
            s<<i<<",";
        s<<")";
    }
    return s.str();
}

std::vector<int> SharedEditor::LSEQ_alloc(std::vector<int> pos1,std::vector<int> pos2){
    int depth=0;
    int interval=pos2[0]-pos1[0]-1;
    int boundary=10; //da dichiarare globale
    std::vector<int> newpos;

    while(interval<1){
        depth++;
        int s1,s2;

        if (pos2.size()>depth) {
            s2=pos2[depth];
        }
        else
            s2=base[depth];

        if (pos1.size()>depth) {
            s1=pos1[depth];
        }
        else
            s1=0;
        interval=s2-s1-1;
    }

    int step=std::min(boundary,interval);
    std::cout<<"Step ="<<step<<std::endl;

    //choice of the strategy (true=boundary+,false=boundary-)
    if ( strategy.find(depth) == strategy.end() ) {
        strategy.insert(std::pair<int,bool>(depth,randomBool()));
    }

    if (strategy.at(depth)){
        //boundary++
        newpos=prefix(pos1,depth,1);
        int r=randomNum(1,step);
        std::cout<<"Random+ ="<<r<<std::endl;
        newpos[depth]=newpos[depth]+r;
    }
    else{
        //boundary--
        newpos=prefix(pos2,depth,0);
        int r=randomNum(1,step);
        std::cout<<"Random- ="<<r<<std::endl;
        newpos[depth]=newpos[depth]-r;
    }

    return newpos;

}

std::vector<int> SharedEditor::prefix(std::vector<int> id,int depth,int s){
    std::vector<int> idCopy(id.begin(),id.end());

    for(int i=id.size();i<depth+1;i++){
        if (i==depth){
            if (s==1)
                idCopy.push_back(0);
            else {
                idCopy[depth-1]= idCopy[depth-1]-1;
                idCopy.push_back(base[depth]);
            }
        }
        else
            idCopy.push_back(0);

    }
    return idCopy;
}

bool SharedEditor::randomBool() {
    static auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    return gen();
}

int SharedEditor::randomNum(int n1,int n2){
    return (n1 + ( std::rand() % ( n2 - n1 + 1 ) ));
}



