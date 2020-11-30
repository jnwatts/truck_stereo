#ifndef UGUI_UTILS_H_
#define UGUI_UTILS_H_

#include <assert.h>
#include <vector>

#ifdef NDEBUG
#define UG_ERROR_CHECK(x) do {                                          \
        UG_RESULT __err_rc = (x);                                       \
        (void) sizeof(__err_rc);                                        \
    } while(0)
#else
#define UG_ERROR_CHECK(x) do {                                          \
        assert((x) == UG_RESULT_OK);                                    \
    } while(0)
#endif

namespace UGUI {

template<class P, class M>
size_t _offsetof(const M P::*member)
{
    return (size_t) &( reinterpret_cast<P*>(0)->*member);
}

template<class P, class M>
P* container_of_impl(M* ptr, const M P::*member)
{
    return (P*)( (char*)ptr - _offsetof(member));
}

#define container_of(ptr, type, member) \
     container_of_impl (ptr, &type::member)

static inline void strdup(std::string &s, std::vector<char> &v)
{
    v.resize(s.size() + 1);
    s.copy(&v[0], s.size());
    v[s.size()] = '\0';
}

static inline unsigned actual_char_width(const UG_FONT *font, char c) {
    return (font->widths ? font->widths[static_cast<unsigned>(c) - font->start_char] : font->char_width);
}

static inline unsigned actual_str_width(const UG_FONT *font, std::string s) {
    unsigned w = 0;
    for (char c : s)
        w += actual_char_width(font, c);
    return w;
}

}

#endif