#ifndef __SAMPLE_TESTCASE_H__

#define __SAMPLE_TESTCASE_H__

#include <iostream>
#include <string>
#include <iomanip>
#include "graph/DGraphModel.h"
#include "graph/UGraphModel.h"
#include "graph/TopoSorter.h"

using namespace std;

bool charComparator(char &lhs, char &rhs)
{
    return lhs == rhs;
}
bool strEQ(string &lhs, string &rhs)
{
    return lhs.compare(rhs) == 0;
}
string vertex2str(char &v)
{
    stringstream os;
    os << v;
    return os.str();
}
string vertex2str(string &v)
{
    return v;
}
bool charEQ(char &lhs, char &rhs)
{
    return lhs == rhs;
}
string char2str(char &item)
{
    stringstream os;
    os << item;
    return os.str();
}

/* ******* BEGIN DGraph ******* */
void DGraphDemo1()
{
    DGraphModel<char> model(&charComparator, &vertex2str);
    char vertices[] = {'A', 'B', 'C', 'D'};
    for (int idx = 0; idx < 4; idx++)
    {
        model.add(vertices[idx]);
    }
    model.connect('A', 'B');
    model.connect('B', 'D');
    model.connect('C', 'B');
    model.connect('C', 'D');
    model.println();
}

int hashFunc(char& key, int tablesize) {
    return key % tablesize;
}
void DGraphDemo2()
{
    DGraphModel<char> model(&charComparator, &vertex2str);
    char vertices[] = {'A', 'B', 'C', 'D'};
    for (int idx = 0; idx < 4; idx++)
    {
        model.add(vertices[idx]);
    }
    model.connect('A', 'B');
    model.connect('B', 'D');
    model.connect('C', 'B');
    model.connect('C', 'D');
    model.println();

    TopoSorter<char> sorter(&model, &hashFunc);
    DLinkedList<char> topo = sorter.sort(TopoSorter<char>::BFS);
    cout << left << setw(15) << "Topo-order: " << topo.toString() << endl;
}

void DGraphDemo3()
{
    DGraphModel<char> model(&charComparator, &vertex2str);
    for (int idx = 0; idx < 10; idx++)
    {
        model.add((char)('0' + idx));
    }

    model.connect('0', '1');
    model.connect('0', '5');
    model.connect('1', '7');
    model.connect('3', '2');
    model.connect('3', '4');
    model.connect('3', '7');
    model.connect('3', '8');
    model.connect('4', '8');
    model.connect('6', '0');
    model.connect('6', '1');
    model.connect('6', '2');
    model.connect('8', '2');
    model.connect('8', '7');
    model.connect('9', '4');
    model.println();

    TopoSorter<char> sorter(&model, &hashFunc);
    DLinkedList<char> bfs = sorter.sort(TopoSorter<char>::BFS);
    cout << left << setw(15) << "Topo-order (BFS): " << bfs.toString() << endl;

    DLinkedList<char> dfs = sorter.sort(TopoSorter<char>::DFS);
    cout << left << setw(15) << "Topo-order (DFS): " << dfs.toString() << endl;
}
/* ******* END DGraph ******* */

/* ******* BEGIN UGraph ******* */
void ugraphDemo1()
{
    UGraphModel<string> model(&strEQ, &vertex2str);

    model.add("HCM");
    model.add("LongAn");
    model.add("BinhDuong");
    model.add("DongNai");
    model.add("BinhThuan");
    model.connect("HCM", "LongAn");
    model.connect("HCM", "BinhDuong");
    model.connect("HCM", "DongNai");
    model.connect("DongNai", "BinhThuan");

    model.println();
}

void ugraphDemo2()
{
    UGraphModel<char> model(&charEQ, &char2str);
    model.add('A');
    model.add('B');
    model.add('C');
    model.add('D');
    model.add('E');
    model.add('F');
    model.connect('A', 'B', 6);
    model.connect('A', 'C', 3);

    model.connect('B', 'C', 2);
    model.connect('B', 'D', 5);

    model.connect('C', 'D', 3);
    model.connect('C', 'E', 4);

    model.connect('D', 'E', 2);
    model.connect('D', 'F', 3);

    model.connect('E', 'F', 5);

    model.println();

    model.disconnect('C', 'A');
    model.disconnect('C', 'B');
    model.disconnect('C', 'D');
    model.disconnect('C', 'E');

    model.println();
}
/* ******* END UGraph ******* */

void (*func_ptr[])() = {
    // UGraph
    ugraphDemo1,
    ugraphDemo2,
    // DGraph
    DGraphDemo1,
    DGraphDemo2,
    DGraphDemo3,
};

void run(int func_idx)
{
    func_ptr[func_idx]();
}

void runAll()
{
    for (int idx = 0; idx < sizeof(func_ptr) / sizeof(func_ptr[0]); idx++)
    {
        run(idx);
    }
}



#endif /* __SAMPLE_TESTCASE_H__ */