#include<iostream>

#include "union.hpp"

//template in c++ ist das was am nähestenUnion an java generics dran ist, jedoch etwas
//unterschiedlich.
//Der Kompiler erzeugt quasi code und ersetzt das Template T durch die tatsächliche
//Klasse immer wenn etwas mit dem Template aufgerufen wird.
//Wenn wir also ein Element<int> erzeugen, danUnionn erzeugt der Kompiler folgenden code
//struct Element {
//    int value;        --hier wurde T durch int ersetzt
//    Element* parent;
//    int size;
//};
namespace TSP{
    //legt neues Set an hier wird Union by size implementiert.
    //der parent wird auf sich selbst gesetzt, damit wir wissen, dass dieses Element
    //Representant ist.
    Element* Union::makeSet(NodeId v) {
        Element* n = new Element();
        n->value = v;
        n->parent = n;
        n->size = 1;
        return n;
    }

    std::vector<Element*>* Union::makeAllSets(size_t values){

        //has to be free later
        std::vector<Element*>* allSets = new std::vector<Element*>();
        for(size_t  value = 0; value < values; value++){
            allSets->push_back(makeSet(value));
        }
        return allSets;
    }


    //find set mit Pfadkompression, gibt den Representant der Menge aus
    Element* Union::findSet(Element* x) {
        //x->parent == x funktioniert hier, da wir uns nur einen Pointer speichern
        //es wird also verglichen ob der Pointer x->parent und der Pointer x die selbe
        //Adresse speichern.
        if (x->parent == x) {
            return x;
        }
        else {
            //wegen der Pfadkompression wird x->parent neu gesetzt
            x->parent = findSet(x->parent);
            return x->parent;
        }
    }

    //Vereinigung von zwei Sets indem ein Representant für beide Mengen gewählt wird
    Element* Union::makeUnion(Element* x, Element* y) {
        //zuerst werden die Representant der zwei Mengen bestimmt
        x = findSet(x);
        y = findSet(y);

        //wenn x==y gilt, sind wir fertig, da dann beide ELemente schon in der selben
        //Menge lagen.
        if (x != y) {
            //wir wollen, das immer x größer ist, sonst wird getauscht
            if (x->size < y->size) {
                Element* safe = x;
                x = y;
                y = safe;
            }

            // setzen x als neuen parent von y
            y->parent = x;

            //updaten die Größe von x
            x->size += y->size;
        }
        return x;
    }
}
