/*
    Starter code for assignment 5, CSC 254, Fall 2015

    Provides skeleton of code for generic ordered sets.

    Everything but /main/ should be moved to a .h file, which should
    then be #included from here.

    Be sure to read the assignment web page carefully.
    Then search for all-caps comments preceded by '***' for hints as to
    where you should make changes.

    A couple warnings:

    (1) Don't try to modify an oset while you're iterating over it.
        The following may not work:
            for (oset::iter i = begin(); i != end(); ++i) {
                if (!other[*i]) operator-=(*i);
            }

    (2) Don't try to modify an oset in the middle of a string of output:
        The following may not work:
            cout << (S += 9) << " " << S[9] << endl;
        (Because this is all one expression, the compiler is free to
        evaluate S[9] before it evaluates (S += 9)).
*/

#include <iostream>
using std::cout;
using std::endl;
using std::flush;


template <class T>
class oset;
void print(oset& OS);       // for debugging

// Non-generic starter code.
//
// *** THIS SKELETON ENVISIONS INTEGER ELEMENTS.
// *** YOUR VERSION SHOULD BE POLYMORPHIC.
//
template <class T>
class oset {
    template <class T>
    class node {
     public:
        const T val;
        node<T> *next;
        node(T v) : val(v), next(NULL) { }
    };
    node<T> head;
        // NB: _not_ node*.  There's a dummy node here, with garbage val;
        // Existence of this node avoids several special cases in the
        // methods below.
    node<T> beyond;
        // to simplify iterator.

    //--------------------------------------
    // Iterator support

 public:
    template<class T>
    class iter {
        node<T> *pos;          // node _before_ the one with this->operator*
        // constructor is private:
        iter(node<T>* n) : pos(n) { } //!!!!!!! should I add <T> to the node*?????
    friend class oset;      // so oset can call the (private) constructor
    public:   
        const T& operator*() {
            return pos->next->val;
        }
        // support forward iteration.  This is prefix version (++p).
        iter<T>& operator++() {
            //cout<< "\"" << pos->next->val <<"\"" << endl;
            if (pos != NULL) pos = pos->next;
            return *this;
        }
        // and this is the postfix version (p++).
        // Note that it returns a copy, _not_ a reference.
        iter<T> operator++(T) {
            iter<T> rtn = *this;
            if (pos != NULL) pos = pos->next;
            return rtn;
        }
        // maybe we should also check the value of the current ?
        bool operator==(iter<T> other) {return pos->next == other.pos->next  ;}  
        bool operator!=(iter<T> other) {return pos->next != other.pos->next  ;}
    };

 private:
    iter<T> start;         // initialized in the constructors below
    iter<T> finish;        // initialized in the constructors below

 public:
    iter<T> begin() {
        return start;
    }
    iter<T> end() {
        return finish;
    }

    //--------------------------------------
    // Constructors and destructor

    // new empty set:
    oset() : head(0), beyond(0), start(&head), finish(&beyond) {
        head.next = NULL;
    }
    oset() : head(""), beyond(""), start(&head), finish(&beyond) {
        head.next = NULL;
    }

    // new singleton set:
    oset(int v) : head(0), beyond(0), start(&head), finish(&beyond) {
        head.next = new node(v);
    }

    // copy constructor:
    oset(oset& other) : head(0), beyond(0), start(&head), finish(&beyond) {
        node *o = other.head.next;
        node *n = &head;
        while (o) {
            n->next = new node(o->val);
            o = o->next;
            n = n->next;
        }
        n->next = NULL;
    }

 private:
    void clear() {
        node *n = head.next;
        while (n) {
            node* t = n->next;
            delete n;
            n = t;
        }
        head.next = NULL;
    }

 public:
    // destructor -- clean up nodes:
    ~oset() {
        clear();
    }
    // assignment
    oset& operator=(oset& other) {
        clear();
        //cout<< "\"" << other.head.next->val <<"\"" << endl;
        operator+=(other);      // union (see below)

    }

    //--------------------------------------
    // Find, insert, and remove

private:
    // Return pointer to last node with val < v
    //
    // *** THIS CODE IMPLICITLY REQUIRES A >= OPERATOR FOR THE SET
    // *** ELEMENT TYPE.  YOU NEED TO MAKE THAT EXPLICIT
    // *** (IN DIFFERENT WAYS IN DIFFERENT VERSIONS OF THE CODE).
    //
    node* find_prev(const int v) {
        node* p = &head;
        while (true) {
            if (p->next == NULL) return p;
            if (p->next->val >= v) return p;
            p = p->next;
        }
    }
        
public:
    // find -- return true iff present:
    bool operator[](const int v) {
        node* p = find_prev(v);
        return (p->next != NULL && p->next->val == v);
    }

    // insert v if not already present; return ref to self
    oset& operator+=(const int v) {
        node* p = find_prev(v);
        if (p->next == NULL || p->next->val != v) {
            node* n = new node(v);
            n->next = p->next;
            p->next = n;
        }
        return *this;
    }

    // remove v if present; return ref to self
    oset& operator-=(const int v) {
        node* p = find_prev(v);
        node* t;
        if ((t = p->next) != NULL && p->next->val == v) {
            // already present
            p->next = t->next;
            delete t;
        }
        return *this;
    }

    //--------------------------------------
    // Union, intersection and difference.
    // These are operator assignments, to simplify management of space:
    // programmer is forced to manage temporaries explicitly
    // (as illustrated in the code for intersection below).

    // Union.
    //
    // *** THIS CODE HAS COST O(N^2).  IT SHOULD BE O(N).
    //
    oset& operator+=(oset& other) {
        // for (iter i = other.begin(); i != other.end(); ++i) {
        //     operator+=(*i);
        // }
        // return *this;

        iter f1 = begin(); 
        //iter l1 = end();
        iter f2 = other.begin();   //cout<< "\"" << *f2 <<"\"" << endl;
        // iter l2 = other.end(); //cout<< "\"" << l2.pos->val <<"\"" << endl;
        node* result = &head;

        
        //for the case that at the very beginning that one of the set is empty
        // if(f1 == end())// the case set1 reaches the end
        //      {
        //         f2++;
        //         while(f2 != other.end()) // though it's a while loop, it won't make the runtime cost from O(n) to O(n^2)
        //         {   
        //             //cout<< "\"" << *f2 <<"\"" << endl;
        //             result->next = new node(*f2);
        //             f2++;
        //             //if(f2 != l2) cout<< "\"" << "pass!" <<"\"" << endl;
        //             result = result->next;
        //         }
        //         return *this;
        //      }  
        // if(f2 == other.end())// the case set2 reaches the end , simply end the union process and return.
        //      {
        //         f1++;
        //         while(f1 != end()) // though it's a while loop, it won't make the runtime cost from O(n) to O(n^2)
        //         {
        //             result->next = new node(*f1);
        //             f2++;
        //             result = result->next;
        //         }
        //         return *this;
        //      }  

        //      // go after the head which is a dummy node with value zero
        //      f1++;
        //      f2++;


        while(true){

            //cout<< "\"" << result->val <<"\"" << endl;
            //cout<< "\"" << "in!" <<"\"" << endl;


            if(f1 == end())// the case set1 reaches the end
             {
                while(f2 != other.end()) // though it's a while loop, it won't make the runtime cost from O(n) to O(n^2)
                {
                    result->next = new node(*f2);
                    ++f2;
                    result = result->next;
                }
                return *this;
             }   


            
            if(f2 == other.end())// the case set2 reaches the end , simply end the union process and return.
             {
                while(f1 != end()) // though it's a while loop, it won't make the runtime cost from O(n) to O(n^2)
                {
                    result->next = new node(*f1);
                    ++f1;
                    result = result->next;
                }
                return *this;
             }  


            if(*f1 < *f2)// case that current s2 bigger than s1, put in current s1
             {
                cout<< "\"a " << *f1 <<"\"" << endl;
                cout<< "\"b " << *f2 <<"\"" << endl;

                cout<< "\"" << f1.pos->val <<"\"" << endl;
                cout<< "\"" << f1.pos->next->val<<"\"" << endl;
                cout<< "\"" << f1.pos->next->next->val<<"\"" << endl;


                result->next = new node(*f1);
                ++f1;
                result = result->next;
                
             }
             else
             {
                if (*f1 > *f2)
                {
                    result->next = new node(*f2);
                    ++f2;
                    result = result->next;
                }
                else // the case that s1 == s2, so add s1 and increment both f1 and f2
                {
                    result->next = new node(*f1);
                    ++f1;
                    ++f2;
                    result = result->next;
                }
             }   

        } // end of while true loop

    }
    //--------------------------------------//--------------------------------------
    // Set difference.   delete the elements which exists in SET2 from SET1
    //
    // *** THIS CODE HAS COST O(N^2).  IT SHOULD BE O(N).
    //
    oset& operator-=(oset& other) {
        // for (iter i = other.begin(); i != other.end(); ++i) {
        //     operator-=(*i);
        // }
        // return *this;
        iter f1 = begin(); 
        iter f2 = other.begin();   
        node* result = &head;

        while(true){

            if(f1 == end())// the case set1 reaches the end
             {
                return *this;
             }   
            
            if(f2 == other.end()){// the case set2 reaches the end , add the rest of SET1 to result
                while(f1 != end()){
                    result->next = new node(*f1);
                    ++f1;
                    result = result->next;
                }
                return *this;
             }  

            if(*f1 < *f2)// case that current s2 bigger than s1, that means s1 is safe and we won't delete it
             {
                result->next = new node(*f1);
                ++f1; 
                result = result->next;               
             }
             else
             {
                if (*f1 > *f2) // s1 is bigger, then need to check the next s2
                {
                    ++f2;
                }
                else // the case that s1 == s2, we don't want this s1, and since there's no common element in one set, we should also ++f2
                {
                    ++f1;
                    ++f2;
                }
             }   

        } // end of while true loop


    }
   
    //--------------------------------------//--------------------------------------
    // Intersection.
    //
    // *** THIS CODE HAS COST O(N^2).  IT SHOULD BE  O(N).
    //
    oset& operator*=(oset& other) {
        // oset temp;      // empty
        // for (iter i = begin(); i != end(); ++i) {
        //     if (other[*i]) temp+=(*i);
        // }
        // clear();
        // operator+=(temp);   // union
        // // NB: temp is destructed as we leave this scope
        // return *this;
        iter f1 = begin(); 
        iter f2 = other.begin();   
        node* result = &head;


        while(true){

            if(f1 == end())// the case set1 reaches the end
             {
                return *this;
             }   


            
            if(f2 == other.end())// the case set2 reaches the end , simply end the union process and return.
             {
                return *this;
             }  


            if(*f1 < *f2)// case that current s2 bigger than s1, increment f1
             {  
                ++f1;         
             }
             else
             {
                if (*f1 > *f2) // increment f2
                {
                    ++f2;
                }
                else // the case that s1 == s2, so add s1 and increment both f1 and f2
                {
                    result->next = new node(*f1);
                    ++f1;
                    ++f2;
                    result = result->next;
                }
             }   

        } // end of while true loop
    }
};

void print(oset& OS) {
    for (oset::iter i = OS.begin(); i != OS.end(); ++i) {
        cout << *i << " ";
    }
    cout << endl;
}

