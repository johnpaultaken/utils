//----------------------------------------------------------------------------
// year   : 2019
// author : John Paul
// email  : johnpaultaken@gmail.com
// source : https://github.com/johnpaultaken
// description :
// Using stream operator with container inserter.
// For example stream input directly to a vector.
//----------------------------------------------------------------------------

#include <iostream>
using std::cin;
using std::cout;
using std::istream;
#include <vector>
using std::vector;
#include <iterator>

// Note the use of template partial type that is required here.
template <template <typename T> class Container, typename U>
istream & operator >> (istream & sin, std::back_insert_iterator<Container<U>> & bit)
{
    U u;
    sin >> u;
    *bit = u;

    return sin;
}

// This wrapper is needed to make vector require only one type parameter.
template <typename T>
class simple_vector : public vector<T>
{
};

int main()
{
    simple_vector<int> tree;
    auto treein = std::back_inserter(tree);
    while (!cin.eof())
    {
        cin >> *treein;
    }

    cout << "size:" << tree.size();
    std::cout << "\ndone";
}
