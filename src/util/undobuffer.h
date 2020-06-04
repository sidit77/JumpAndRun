#pragma once

#include "noncopyable.h"

namespace jnr {
    template<typename T, size_t N>
    class UndoBuffer : private NonCopyable{
    private:
        T buffer[N];
        size_t start = 0;
        size_t length = 0;
        size_t pos = 0;
    public:
        void clear(){
            start = 0;
            length = 0;
            pos = 0;
        }
        T& add(){
            start = (N + start - (length - pos)) % N;
            T& result = buffer[start];
            start = (start + 1) % N;
            if(pos < N)
                pos++;
            length = pos;
            return result;
        }
        bool canUndo(){
            return pos > 0;
        }
        T& undo(){
            T& result = buffer[(N - 1 + start + pos - length) % N];
            pos--;
            return result;
        }
        bool canRedo(){
            return pos + 1 < length;
        }
        T& redo(){
            pos++;
            T& result = buffer[(N + start + pos - length) % N];
            return result;
        }
    };
}

