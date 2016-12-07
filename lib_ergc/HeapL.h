#ifndef __HEAPL_H_
#define __HEAPL_H_
#include <iostream>

template <class T, typename U>
  class HeapL{
 private:
  /**
   * The tree of elements.
   */
  class Theap {
  public :
    U pkey;
    T element;
      
    Theap() {}
    ~Theap() {}
  };
   
  int _size;
  int _nitem;
  Theap *_data;

 public:

  /**
   * Creates a new <code>HeapL</code> with the specified size.
   * @param n	the maximum size of the heap.
   */
 HeapL( int n ): _size(n), _nitem(0),_data(0) {
    if (n>0) _data=new Theap[n];
    else std::cerr<<"Error: negative heap size...not allocated"<<std::endl;
  }

  ~HeapL(){ delete[] _data; }

  /**
   * Checks whether the heap is empty.
   * @return true if the heap empty.
   */
  bool Empty() { return _nitem==0; }

  /**
   * Checks whether the heap is full.
   * @return true if the heap full.
   */
  bool Full() { return _nitem==_size-1; }

  /**
   * Returns the current number of elements if the heap.
   * @return the current size of the heap
   */
  int Nrank() { return _nitem; }

  /**
   * Resets the heap (-> Nrank() = 0).
   */
  void Reset() { _nitem=0; } 

  /**
   * Inserts a new element in the heap with the specified key.
   * @param item	the element to be inserted.
   * @param pkey	the primary key that identify the element in the heap.
   * @return true if the insertion is ok, false if the heap is full.
   */
  bool Push( const T &item, U pkey ) {
    if (Full()) {
      std::cerr<<"Warning: heap full..." << std::endl;
      return false;
    }
    int child=_nitem++;
    int parent=(child-1)/2;

    while (child>0 && (pkey<_data[parent].pkey || (pkey==_data[parent].pkey ))) {
      _data[child]=_data[parent];
      child=parent;
      parent=(parent-1)/2;
    }
    _data[child].pkey=pkey;
    _data[child].element=item;

    return true;
  } 

  /**
   * Removes and returns the next element with the minimum key value.
   * @param pkey	use to return the value of the primary key of the next element.
   * @return	the element.
   */
  T Pop( U *pkey=NULL ) {
    if (Empty()) {
      std::cerr<<"Warning: heap empty..." << std::endl;
      return _data[0].element;
    }

    // Get top of the heap.
    T item=_data[0].element;
    if (pkey) *pkey=_data[0].pkey;

    --_nitem;
    int parent=0;
    int child=1;

    // Move up the lower branch of the heap.
    while (child<_nitem) {

      if (child+1<_nitem && (_data[child+1].pkey<_data[child].pkey || (_data[child+1].pkey==_data[child].pkey )))
	child++;

      if (_data[_nitem].pkey<_data[child].pkey || (_data[_nitem].pkey==_data[child].pkey ))
	break;
      // Move up child as parent.
      _data[parent]=_data[child];
      parent=child;
      child=child*2+1;
    }
    _data[parent]=_data[_nitem];
    return item;
  }

  /**
   * Removes from the heap the first occurence of the element
   * specified by its value.
   * @param item	the element to be removed.
   * @return	false (Error) or true (Success)
   */
  int Remove( T &item ) {
    bool result=false;
    int i=0;
    
    for (; i<_nitem && !(_data[i].element==item); i++) ;
    if (i>= _nitem) return false;
    
    --_nitem;
    int parent=i;
    int child=i*2+1;
    
    // Move up the lower branch of the heap.
    while (child<_nitem) {
      if (child+1<_nitem &&
	  (_data[child+1].pkey<_data[child].pkey || (_data[child+1].pkey==_data[child].pkey )))
	child++;
      
      if (_data[_nitem].pkey<_data[child].pkey || (_data[_nitem].pkey==_data[child].pkey ))
	break;
      
      // Move up child as parent.
      _data[parent]=_data[child];
      parent=child;
      child=child*2+1;
    }
    _data[parent]=_data[_nitem];
    
    return true;
  }
};



#endif
