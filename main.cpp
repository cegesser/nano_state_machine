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

struct Noop
{
    template<typename I>
    void operator()(I) const {  } 
};

template<typename S, typename P>
Transition<S,P,Noop> make_transition(S state, S next,  P predicate)
{
    return { state, next, predicate, Noop() };
}

template<typename S, typename I, typename T, typename ...Tn>
inline S next(S state, I input, S err, T t1, Tn ...tn)
{
    if (t1.state == state && t1.predicate(*input))
    {
        t1.action(input);
        return t1.next;
    }
    return next(state, input, err, tn...);
}

template<typename S, typename I>
inline S next(S, I, S err)
{
    return err;
}

template<typename Iter, typename State, typename Validation, typename ...Transitions>
State run(Iter begin, Iter end, State start, State error, Validation validation, Transitions ...transitions )
{
    while ( validation(start) && begin != end)
    {
        auto curr = begin;
        auto c = *begin++;
        start = next(start, curr, error, transitions...);
    }
    
    return start;
}

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <string>

bool simple_http_request_line_test(const char *begin, const char *end)
{
    enum State { Method, FirstSpace, RequestTarget, SecondSpace, HttpVersion, CR, LF, Error };
    
    const char *start_method = begin;
    const char *start_request_target;
    const char *start_http_version;
    
    auto result = run(begin, end, Method, Error, [](State s) { return s != Error; },
        make_transition(Method,        Method,        [](char c){ return c != ' '; } ),
        make_transition(Method,        FirstSpace,    [](char c){ return c == ' '; }, [&](const char *i){ std::cout << "Method: " << std::string(start_method, i) << std::endl; }  ),
        make_transition(FirstSpace,    RequestTarget, [](char c){ return c != ' '; }, [&](const char *i){ start_request_target = i; }  ),
        make_transition(RequestTarget, RequestTarget, [](char c){ return c != ' '; } ),
        make_transition(RequestTarget, SecondSpace,   [](char c){ return c == ' '; }, [&](const char *i){ std::cout << "Request Target: " << std::string(start_request_target, i) << std::endl; }  ),
        make_transition(SecondSpace,   HttpVersion,   [](char c){ return c != ' '; }, [&](const char *i){ start_http_version = i; }  ),
        make_transition(HttpVersion,   HttpVersion,   [](char c){ return c != '\r'; } ),
        make_transition(HttpVersion,   CR,            [](char c){ return c == '\r'; }, [&](const char *i){ std::cout << "HTTP Version: " << std::string(start_http_version, i) << std::endl; } ),
        make_transition(CR,            LF,            [](char c){ return c == '\n'; } )
    );
    std::cout << "Last State: " << result << std::endl;
    return result == LF;
}

int main(int argc, char * argv[])
{
    
    char txt[] = "GET /index.html HTTP/1.1\r\n";
    std::cout << std::boolalpha << simple_http_request_line_test(txt, txt+sizeof(txt)-1) << std::endl;
    return 0;
    
}