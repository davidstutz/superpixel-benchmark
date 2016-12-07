#pragma once

#include <vector>
#include <unordered_set>
#include <mutex>


// ParallelDeque 
///////////////////////////////////////////////////////////////////////////////

// - Thread safe deque based on vector ("circular list" representation)
// - Supports forbidden elements (excluded from PopFront operation)
// - Operations: 
//    * PopAndLock pops an "unforbidden" element from the front of the deque
//      and locks it
//    * Release releases element (removes it from forbidden list)
//    * PushBack adds an element to the end of the deque.


template <typename T, typename S> class ParallelDeque {
private:
    typedef std::unordered_set<S> LockSetType;
    typedef std::unordered_map<T, LockSetType> LockMapType;

    std::vector<T> vector;
    size_t start;
    size_t end;
    size_t listSize;
    size_t popCount;
    T emptyVal;
    std::function<void(T&, LockSetType&)> lockFun;
    LockMapType lockMap;
    LockSetType locked;
    std::mutex m;
public:
    ParallelDeque(size_t maxSize) :
        start(0), end(0), listSize(0), emptyVal(T()),
        popCount(0)
    {
        vector.resize(maxSize);
    }

    size_t GetPopCount() { return popCount; }

    void SetEmptyVal(const T& ev) { emptyVal = ev; }

    void SetLockFunction(const std::function<void(T&, LockSetType&)>& lf)
    {
        lockFun = lf;
    }

    size_t Size() const { return listSize; }

    bool Empty() const {
        return listSize == 0;
    }

    void Clear() { start = end = listSize = 0; }

    void PushBack(const T& value)
    {
        std::unique_lock<std::mutex> lck(m);

        if ((end + 1) % vector.size() == start) resize(2 * vector.size());
        vector[end] = value;
        end = (end + 1) % vector.size();
        listSize++;
    }

    T PopAndLock()
    {
        std::unique_lock<std::mutex> lck(m);
        LockSetType lockSet;

        if (Empty()) return emptyVal;
        for (int i = start; i != end; i = (i + 1) % vector.size()) {
            T result = vector[i];
            if (result != emptyVal) {
                lockSet.clear();
                lockFun(result, lockSet);
                if (find_if(lockSet.begin(), lockSet.end(), [&](const S& s) { return locked.find(s) != locked.end(); }) == lockSet.end()) {
                    vector[i] = emptyVal;
                    if (i == start) {
                        do {
                            start = (start + 1) % vector.size();
                        } while (start != end && vector[start] == emptyVal);
                    }
                    locked.insert(lockSet.begin(), lockSet.end());
                    lockMap[result] = std::move(lockSet);
                    listSize--;
                    popCount++;
                    return result;
                }
            }
        }
        return emptyVal;
    }

    void Release(const T& e) {
        std::unique_lock<std::mutex> lck(m);

        typename LockMapType::iterator fiter = lockMap.find(e);
        if (fiter != lockMap.end()) {
            for (const S& s : fiter->second)
                locked.erase(s);
            lockMap.erase(e);
        }
    }

private:
    void resize(size_t newSize)
    {
        std::vector<T> newVector;

        assert(newSize > vector.size());
        newVector.resize(newSize);
        if (start <= end) std::copy(vector.begin() + start, vector.begin() + end, newVector.begin());
        else {
            std::copy(vector.begin() + start, vector.end(), newVector.begin());
            std::copy(vector.begin(), vector.begin() + end, newVector.begin() + vector.size() - start);
        }
        std::swap(vector, newVector);
        start = 0;
        end = listSize;
    }
};

template <typename T> class Deque {
private:
    std::vector<T> vector;
    size_t start;
    size_t end;
    size_t listSize;
public:
    Deque(size_t maxSize) :
        start(0), end(0), listSize(0)
    {
        vector.resize(maxSize);
    }

    size_t Size() const { return listSize; }

    bool Empty() const { return listSize == 0; }

    void Clear() { start = end = listSize = 0; }

    void PushBack(const T& value)
    {
        if ((end + 1) % vector.size() == start) resize(2 * vector.size());
        vector[end] = value;
        end = (end + 1) % vector.size();
        listSize++;
    }

    const T& Front() const { return vector[start]; }

    T PopFront() 
    { 
        size_t retIndex = start;
        start = (start + 1) % vector.size(); 
        listSize--; 
        return vector[retIndex];
    }

private:
    void resize(size_t newSize)
    {
        std::vector<T> newVector;

        assert(newSize > vector.size());
        newVector.resize(newSize);
        if (start <= end) std::copy(vector.begin() + start, vector.begin() + end, newVector.begin());
        else {
            std::copy(vector.begin() + start, vector.end(), newVector.begin());
            std::copy(vector.begin(), vector.begin() + end, newVector.begin() + vector.size() - start);
        }
        std::swap(vector, newVector);
        start = 0;
        end = listSize;
    }
};


