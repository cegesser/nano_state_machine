template<typename S, typename P, typename A>
struct Transition
{
    S state;
    S next;
    P predicate;
    A action;
};

template<typename S, typename P, typename A>
Transition<S,P,A> make_transition(S state, S next,  P predicate, A action)
{
    return { state, next, predicate, action };
}

template<typename S, typename I, typename T, typename ...Tn>
inline S next(S state, I input, S err, T t1, Tn ...tn)
{
    if (t1.state == state && t1.predicate(input))
    {
        t1.action();
        return t1.next;
    }
    return next(state, input, err, tn...);
}

template<typename S, typename I>
inline S next(S state, I input, S err)
{
    return err;
}

template<typename Iter, typename State, typename Validation, typename ...Transitions>
State run(Iter begin, Iter end, State start, State error, Validation validation, Transitions ...transitions )
{
    while ( validation(start) && begin != end)
    {
        auto c = *begin++;
        start = next(start, c, error, transitions...);
    }
    
    return start;
}

//--------------------------------------------------------------------------------------------------------------------

#include <iostream>

auto test(const char *begin, const char *end)
{
    enum State { A, B, E };
    
    return run(begin, end, A, E, [](State s) { return s != E; },
                 make_transition(A, B, [](char c){ return c == 'a'; }, [](){ std::cout << "A->B" << std::endl; }),
                 make_transition(B, B, [](char c){ return c == 'b'; }, [](){ std::cout << "B->B" << std::endl; }),
                 make_transition(B, A, [](char c){ return c != 'b'; }, [](){ std::cout << "B->A" << std::endl; })
                );
}

int main(int argc, char * argv[])
{
    
    char txt[] = "abbca";
    std::cout << test(txt, txt+sizeof(txt)-1) << std::endl;
    return 0;
    
}