#include <iostream>

using namespace std;

// simple unidirectional list
struct MyList{
    int num;
    MyList *next;

    MyList(int i):num(i), next(0){}
    MyList(const MyList& c):num(c.num), next(0){}
    bool operator ==(MyList& l){return l.num == num;}
    ~MyList(){
        if(next) delete next;
    }

    void print(){
        cout << num << " ";
        if(next)
            next->print();
        else
           cout << endl;
    }
};

// rearrange unidirectional list orig to new list
// Time complexity 2N
// Memory complexity 2N

MyList *repack(MyList *orig){
    // backward list
    MyList *back;
    //forward pointer
    MyList *current;
    //backward pointer
    MyList *prev;
    // num elements in list
    int count = 0;

   // create backward list
    current = orig;
    back = new MyList(*current);
    prev = back;
    while (current->next) {
       current = current->next;
       back = new MyList(*current);
       back->next = prev;
       prev = back;
       count++;
    }

    // merge forward with backward
    MyList *result;
    MyList *l;

    current = orig;
    l = new MyList(*current);
    result = l;

    l->next = new MyList(*prev);
    l = l->next;

    current = current->next;
    prev = prev->next;

    for(int i = 0; i < count/2; i++){
        l->next = new MyList(*current);
        l = l->next;

        // prevent copy the same element if N odd number
        if(*l == *prev) break;

        l->next = new MyList(*prev);
        l = l->next;

        current = current->next;
        prev = prev->next;
    }

    delete back;

    return result;
}

int main(int argc, char *argv[])
{   MyList *first;
    MyList *current;

    cout << "Repack List" << endl;

    // init first state
    first = new MyList(1);
    current = first;
    for( int i = 2; i < 22; i++){
        current->next = new MyList(i);
        current = current->next;
    }
    cout << "Initial state " << endl;
    first->print();

    cout << "After repacking " << endl;
    current = repack(first);
    current->print();

    delete first;
    delete current;

    return 0;
}
