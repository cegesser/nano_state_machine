template<typename S, typename P, typename A>
struct Transition
{
    S state;
    S next;
    P predicate;
    A action;
    
    template<typename Input>
    bool operator()(Input input) 
    {
        return predicate(input);
    }
    
    void operator()() 
    {
        action();
    }
};

template<typename S, typename P, typename A>
Transition<S,P,A> make_transition(S state, S next,  P predicate, A action)
{
    return { state, next, predicate, action };
}

template<typename S, typename I, typename T, typename ...Tn>
S next(S state, I input, S err, T t1, Tn ...tn)
{
    if (t1.state == state && t1(input))
    {
        t1();
        return t1.next;
    }
    return next(state, input, err, tn...);
}

template<typename S, typename I, typename T>
S next(S state, I input, S err, T t1)
{
    if (t1.state == state && t1(input))
    {
        t1();
        return t1.next;
    }
    return err;
}

template<typename Iter, typename State, typename ...Transitions>
State run(Iter begin, Iter end, State start, State error, Transitions ...transitions )
{
    while (begin != end && start != error)
    {
        start = next(start, *begin++, error, transitions...);
    }
    
    return start;
}

#include <iostream>

bool test(const char *begin, const char *end)
{
    enum State { A, B, E };
    
    return run(begin, end, A, E,
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