
#if !defined(__LIST_H__)

#define __LIST_H__

#include <stdbool.h>

#include <cli_mutex.h>

#if defined(__cplusplus)
#include <iostream>
extern "C" {
#endif 
    
struct ListItem;

typedef struct ListItem *pList;

typedef pList* (*pnext)(pList item);

void list_push(pList *head, pList w, pnext next_fn, struct Mutex *mutex);
void list_append(pList *head, pList w, pnext next_fn, struct Mutex *mutex);
bool list_remove(pList *head, pList w, pnext next_fn, struct Mutex *mutex);
int list_size(pList *head, pnext next_fn, struct Mutex *mutex);

pList list_pop(pList *head, pnext next_fn, struct Mutex *mutex);

typedef int (*cmp_fn)(const pList w1, const pList w2);

void list_add_sorted(pList *head, pList w, pnext next_fn, cmp_fn cmp, struct Mutex *mutex);

bool list_has(pList *head, pList w, pnext next_fn, struct Mutex *mutex);

typedef int (*visitor)(pList w, void *arg);

pList  list_find(pList *head, pnext next_fn, visitor fn, void *arg, struct Mutex *mutex);
void list_visit(pList *head, pnext next_fn, visitor fn, void *arg, struct Mutex *mutex);

#if defined(__cplusplus)
}
#endif 

#if defined(__cplusplus)

    /**
     *
     */

template <class Item, Item *next(Item)>
static ListItem ** make_item_next(ListItem *item)
{
    return (ListItem **)next((Item)item); 
}

template <class T>
class List
{
public:
    typedef T* (*fn)(T item);

    T head;
    //fn next_fn;
    pnext next_fn;

    List(fn _fn) : head(0)
    { 
        std::cout << "type of make_item_next=" << typeid(make_item_next<T,(*_fn)>).name() << std::endl;
        //std::cout << "type of make=" << typeid(make_item_next<T,_fn>).name() << std::endl;
        //next_fn = &make_item_next<T,_fn>;
    }
    
    List(pnext _fn) : head(0), next_fn(_fn) 
    { 
        std::cout << "type of pnext=" << typeid(_fn).name() << std::endl;  
        //next_fn = make_item_next<T,_fn>;
    }

    /*
    List(fn _fn) : head(0), next_fn(_fn) 
    { 
        //next_fn = make_item_next<T,_fn>;
    }
    */

    ListItem ** my_item_next(ListItem *i)
    {
        return (ListItem **)next_fn((T)i); 
    }

    void push(T w, Mutex *mutex)
    {
        list_push((pList*) & head, (pList) w, next_fn, mutex);
        //list_push((pList*) & head, (pList) w, &, mutex);
    } 

    void append(T w, Mutex *mutex)
    {
        list_append((pList *) & head, (pList) w, next_fn, mutex);
    }

    bool remove(T w, Mutex *mutex)
    {
        return list_remove((pList *) & head, (pList) w, next_fn, mutex);
    }

    int size(Mutex *mutex)
    {
        return list_size((pList *) & head, next_fn, mutex);
    }

    bool empty()
    {
        return !head;
    }

    T pop(Mutex *mutex)
    {
        return (T) list_pop((pList *) & head, next_fn, mutex);
    }

    void add_sorted(T w, int (*cmp)(T a, T b), Mutex *mutex)
    {
        list_add_sorted((pList *) & head, (pList) w, next_fn, (cmp_fn) cmp, mutex);
    }

    T find(int (*fn)(T a, void *arg), void *arg, Mutex *mutex)
    {
        return (T) list_find((pList *) & head, next_fn, (visitor) fn, arg, mutex);
    }

    bool has(T w, Mutex *mutex)
    {
        return list_has((pList *) & head, (pList) w, next_fn, mutex);
    }

    void visit(int (*fn)(T a, void *arg), void *arg, Mutex *mutex)
    {
        list_visit((pList *) & head, next_fn, (visitor) fn, arg, mutex);
    }
};

#endif // __cplusplus

#endif // __LIST_H__

//  FIN
