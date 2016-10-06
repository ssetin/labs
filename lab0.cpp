#include <atomic>

using namespace std;

template<typename T>
struct TagPointer{
    long counter;
    T *ptr;
    TagPointer() noexcept:counter(0),ptr(nullptr){}
    TagPointer(T *newptr) noexcept:counter(0),ptr(newptr){}
};


//============================================================================
template<typename T>
class lfQueue
{
    struct Node;
    typedef TagPointer<Node> tgPointer;
    struct Node{
        T *value;
        tgPointer next;
        bool dummy{false};
        Node()noexcept:value(nullptr),next(nullptr){}
        Node(T* val)noexcept:value(val),next(nullptr){}
    };
    atomic<tgPointer> last,first;
public:
    atomic<int> size;
    lfQueue();
    ~lfQueue();
    void Append(T* data);
    T* Extract();

};

template<typename T>
lfQueue<T>::lfQueue()
{
    Node *dummy=new Node();
    dummy->dummy=true;
    last.store(dummy);
    first.store(dummy);
    size.store(0);
}

template<typename T>
lfQueue<T>::~lfQueue()
{

}

template<typename T>
void lfQueue<T>::Append(T* data)
{
    Node *newnode=new Node(data);
    tgPointer oldlast=last.load();
    tgPointer newlast;
    newlast.ptr=newnode;
    newlast.counter=0;
    newlast.ptr->next=nullptr;

    do{
        oldlast.ptr->next=newlast;
        newlast.counter++;
    }while(!last.compare_exchange_weak(oldlast,newnode));

    size.store(size.load()+1);
}


template<typename T>
T* lfQueue<T>::Extract()
{
    T *result=nullptr;
    tgPointer oldfirst=first.load();
    tgPointer newfirst;
    newfirst.counter=0;
    newfirst.ptr=nullptr;

    if(oldfirst.ptr->next.ptr!=nullptr && oldfirst.ptr->dummy){
        tgPointer dummy=first.load();
        while(!first.compare_exchange_weak(dummy,oldfirst.ptr->next));
        //delete dummy.ptr->value;
        //delete dummy.ptr;
    }

    do{
        result=oldfirst.ptr->value;
        newfirst.counter++;
        size.store(size.load()-1);
        if(oldfirst.ptr->next.ptr!=nullptr && !oldfirst.ptr->dummy)
            newfirst=oldfirst.ptr->next;
        else{
            newfirst=oldfirst;
            newfirst.ptr->dummy=1;
            newfirst.ptr->next=nullptr;
            newfirst.ptr->value=nullptr;
            //size.store(0);
            return nullptr;
        }

    }while(!first.compare_exchange_weak(oldfirst,newfirst));


    return result;
}
