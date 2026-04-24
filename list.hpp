#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;
        node(): data(nullptr), prev(nullptr), next(nullptr) {}
        explicit node(const T &value): data(new T(value)), prev(nullptr), next(nullptr) {}
        ~node(){ if(data) delete data; }

    };

protected:
    // sentinel head and tail, size counter
    node *head;
    node *tail;
    size_t n;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        // insert cur before pos
        cur->next = pos;
        cur->prev = pos->prev;
        pos->prev->next = cur;
        pos->prev = cur;
        ++n;
        return cur;
    }
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        node *ret = pos->next;
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        pos->prev = pos->next = nullptr;
        --n;
        return ret;
    }

public:
    class const_iterator;
    class iterator {
    private:
        node *ptr;
        const list<T> *owner;

    public:
        iterator(): ptr(nullptr), owner(nullptr) {}
        iterator(node *p, const list<T> *o): ptr(p), owner(o) {}
        friend class list<T>;
        friend class const_iterator;
        /**
         * iter++
         */
        iterator operator++(int) {
            if(owner==nullptr || ptr==nullptr || ptr==owner->tail) throw invalid_iterator();
            iterator tmp=*this; ptr=ptr->next; return tmp;
        }
        /**
         * ++iter
         */
        iterator & operator++() {
            if(owner==nullptr || ptr==nullptr || ptr==owner->tail) throw invalid_iterator();
            ptr=ptr->next; return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int) {
            if(owner==nullptr || ptr==nullptr) throw invalid_iterator();
            if(ptr==owner->head->next) throw invalid_iterator();
            if(ptr==owner->tail){ if(owner->n==0) throw invalid_iterator(); iterator tmp=*this; ptr=ptr->prev; return tmp; }
            iterator tmp=*this; ptr=ptr->prev; return tmp;
        }
        /**
         * --iter
         */
        iterator & operator--() {
            if(owner==nullptr || ptr==nullptr) throw invalid_iterator();
            if(ptr==owner->head->next) throw invalid_iterator();
            if(ptr==owner->tail){ if(owner->n==0) throw invalid_iterator(); ptr=ptr->prev; return *this; }
            ptr=ptr->prev; return *this;
        }
        /**
         * TODO *it
         * remember to throw if iterator is invalid
         */
        T & operator *() const {
            if(owner==nullptr || ptr==nullptr || ptr==owner->tail || ptr->data==nullptr) throw invalid_iterator();
            return *(ptr->data);
        }
        /**
         * TODO it->field
         * remember to throw if iterator is invalid
         */
        T * operator ->() const {
            if(owner==nullptr || ptr==nullptr || ptr==owner->tail || ptr->data==nullptr) throw invalid_iterator();
            return ptr->data;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const { return ptr==rhs.ptr && owner==rhs.owner; }
        bool operator==(const const_iterator &rhs) const { return ptr==rhs.ptr && owner==rhs.owner; }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const { return !(*this==rhs); }
        bool operator!=(const const_iterator &rhs) const { return !(*this==rhs); }
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     * should be able to construct from an iterator.
     */
    class const_iterator {
    private:
        const node *ptr;
        const list<T> *owner;
    public:
        const_iterator(): ptr(nullptr), owner(nullptr) {}
        const_iterator(const node *p, const list<T> *o): ptr(p), owner(o) {}
        const_iterator(const iterator &it): ptr(it.ptr), owner(it.owner) {}
        friend class list<T>;
        /** postfix ++ */
        const_iterator operator++(int){ if(owner==nullptr || ptr==nullptr || ptr==owner->tail) throw invalid_iterator(); const_iterator tmp=*this; ptr=ptr->next; return tmp; }
        /** prefix ++ */
        const_iterator & operator++(){ if(owner==nullptr || ptr==nullptr || ptr==owner->tail) throw invalid_iterator(); ptr=ptr->next; return *this; }
        /** postfix -- */
        const_iterator operator--(int){ if(owner==nullptr || ptr==nullptr) throw invalid_iterator(); if(ptr==owner->head->next) throw invalid_iterator(); if(ptr==owner->tail){ if(owner->n==0) throw invalid_iterator(); const_iterator tmp=*this; ptr=ptr->prev; return tmp; } const_iterator tmp=*this; ptr=ptr->prev; return tmp; }
        /** prefix -- */
        const_iterator & operator--(){ if(owner==nullptr || ptr==nullptr) throw invalid_iterator(); if(ptr==owner->head->next) throw invalid_iterator(); if(ptr==owner->tail){ if(owner->n==0) throw invalid_iterator(); ptr=ptr->prev; return *this; } ptr=ptr->prev; return *this; }
        const T & operator*() const { if(owner==nullptr || ptr==nullptr || ptr==owner->tail || ptr->data==nullptr) throw invalid_iterator(); return *(ptr->data); }
        const T * operator->() const { if(owner==nullptr || ptr==nullptr || ptr==owner->tail || ptr->data==nullptr) throw invalid_iterator(); return ptr->data; }
        bool operator==(const const_iterator &rhs) const { return ptr==rhs.ptr && owner==rhs.owner; }
        bool operator!=(const const_iterator &rhs) const { return !(*this==rhs); }
        bool operator==(const iterator &rhs) const { return ptr==rhs.ptr && owner==rhs.owner; }
        bool operator!=(const iterator &rhs) const { return !(*this==rhs); }
    };

    inline bool is_iterator_valid(const iterator &it) const { return it.owner==this && it.ptr!=nullptr; }
    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    list() {
        head = new node();
        tail = new node();
        head->next = tail; tail->prev = head; head->prev=nullptr; tail->next=nullptr;
        n = 0;
    }
    list(const list &other) {
        head = new node(); tail = new node(); head->next=tail; tail->prev=head; head->prev=nullptr; tail->next=nullptr; n=0;
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) push_back(*it);
    }
    /**
     * TODO Destructor
     */
    virtual ~list() { clear(); delete head; delete tail; }
    /**
     * TODO Assignment operator
     */
    list &operator=(const list &other) {
        if (this==&other) return *this;
        clear();
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) push_back(*it);
        return *this;
    }
    /**
     * access the first / last element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const { if (empty()) throw container_is_empty(); return *(head->next->data); }
    const T & back() const { if (empty()) throw container_is_empty(); return *(tail->prev->data); }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() { return iterator(head->next, this); }
    const_iterator cbegin() const { return const_iterator(head->next, this); }
    /**
     * returns an iterator to the end.
     */
    iterator end() { return iterator(tail, this); }
    const_iterator cend() const { return const_iterator(tail, this); }
    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const { return n==0; }
    /**
     * returns the number of elements
     */
    virtual size_t size() const { return n; }

    /**
     * clears the contents
     */
    virtual void clear() {
        node *p = head->next;
        while (p != tail) {
            node *nx = p->next;
            delete p;
            p = nx;
        }
        head->next = tail; tail->prev = head; n = 0;
    }
    /**
     * insert value before pos (pos may be the end() iterator)
     * return an iterator pointing to the inserted value
     * throw if the iterator is invalid
     */
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.owner != this || pos.ptr==nullptr) throw invalid_iterator();
        node *cur = new node(value);
        node *res = insert(pos.ptr, cur);
        return iterator(res, this);
    }
    /**
     * remove the element at pos (the end() iterator is invalid)
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid
     */
    virtual iterator erase(iterator pos) {
        if (pos.owner != this || pos.ptr==nullptr || pos.ptr==tail) throw invalid_iterator();
        if (empty()) throw container_is_empty();
        node *next = erase(pos.ptr);
        delete pos.ptr;
        return iterator(next, this);
    }
    /**
     * adds an element to the end
     */
    void push_back(const T &value) { insert(iterator(tail, this), value); }
    /**
     * removes the last element
     * throw when the container is empty.
     */
    void pop_back() { if (empty()) throw container_is_empty(); iterator it = iterator(tail->prev, this); erase(it); }
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) { insert(iterator(head->next, this), value); }
    /**
     * removes the first element.
     * throw when the container is empty.
     */
    void pop_front() { if (empty()) throw container_is_empty(); iterator it = iterator(head->next, this); erase(it); }
    /**
     * sort the values in ascending order with operator< of T
     */
    void sort() {
        if (n <= 1) return;
        // Allocate raw buffer without default-constructing T
        void *mem = ::operator new(sizeof(T) * n);
        T *arr = static_cast<T *>(mem);
        size_t i = 0;
        // placement-new copy construct into buffer
        for (node *p = head->next; p != tail; p = p->next) new (arr + (i++)) T(*(p->data));
        sjtu::sort<T>(arr, arr + n, [](const T &a, const T &b){ return a < b; });
        i = 0;
        for (node *p = head->next; p != tail; p = p->next) *(p->data) = arr[i++];
        // destroy and free
        for (size_t k = 0; k < n; ++k) arr[k].~T();
        ::operator delete(mem);
    }
    /**
     * merge two sorted lists into one (both in ascending order)
     * compare with operator< of T
     * container other becomes empty after the operation
     * for equivalent elements in the two lists, the elements from *this shall always precede the elements from other
     * the order of equivalent elements of *this and other does not change.
     * no elements are copied or moved
     */
    void merge(list &other) {
        if (&other == this) return; // merging with self does nothing
        node *a = head->next; node *b = other.head->next;
        node *pos = head->next; // we'll build by splicing from other into this in order
        // Standard merge: iterate through this and other, splicing nodes from other
        while (b != other.tail) {
            // find place in this where b should go, maintaining stability with strict < (this precedes on equal)
            while (a != tail && !(*(b->data) < *(a->data))) {
                a = a->next;
            }
            // splice b before a
            node *bn = b->next;
            // unlink b from other
            b->prev->next = b->next;
            b->next->prev = b->prev;
            // link into this before a
            b->prev = a->prev; b->next = a;
            a->prev->next = b; a->prev = b;
            ++n; --other.n;
            b = bn;
        }
        // other now empty
    }
    /**
     * reverse the order of the elements
     * no elements are copied or moved
     */
    void reverse() {
        if (n<=1) return;
        // swap next/prev along the list including sentinels
        node *p = head;
        while (p) { node *tmp = p->next; p->next = p->prev; p->prev = tmp; p = tmp; }
        // swap head and tail pointers
        node *tmp = head; head = tail; tail = tmp;
    }
    /**
     * remove all consecutive duplicate elements from the container
     * only the first element in each group of equal elements is left
     * use operator== of T to compare the elements.
     */
    void unique() {
        if (n<=1) return;
        node *p = head->next;
        while (p != tail) {
            node *q = p->next;
            while (q != tail && *(q->data) == *(p->data)) {
                node *qn = q->next;
                // erase q
                q->prev->next = q->next; q->next->prev = q->prev; delete q; --n; q = qn;
            }
            p = q;
        }
    }
};

}

#endif //SJTU_LIST_HPP
