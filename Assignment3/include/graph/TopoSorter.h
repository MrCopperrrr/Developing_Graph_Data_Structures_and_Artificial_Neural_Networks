#ifndef TOPOSORTER_H
#define TOPOSORTER_H
#include "graph/DGraphModel.h"
#include "hash/xMap.h"
#include "list/DLinkedList.h"
#include "sorting/DLinkedListSE.h"
#include "sorting/ISort.h"
#include "stacknqueue/Queue.h"
#include "stacknqueue/Stack.h"
// template <class T, class V>
// using xMap = xMap<T, V>;


template<class T>
class TopoSorter{
public:
    static int DFS;
    static int BFS; 
    
protected:
    DGraphModel<T>* graph;
    int (*hash_code)(T&, int);
    
public:
    TopoSorter(DGraphModel<T>* graph, int (*hash_code)(T&, int)=0){
        this->graph = graph;
        this->hash_code = hash_code;
    }   
    DLinkedList<T> sort(int mode=0, bool sorted=true){
        return mode == BFS ? bfsSort(sorted) : dfsSort(sorted);
    }
    
    DLinkedList<T> bfsSort(bool sorted=true) {
        DLinkedList<T> sort_result;
        xMap<T, int> in_degree_counter = vertex2inDegree(hash_code);
        Queue<T> bfsqueue;
        
        // Lấy danh sách đỉnh theo thứ tự thêm vào ban đầu
        DLinkedList<T> vertices = graph->vertices();
        
        // Thêm các đỉnh có bậc vào = 0 vào bfsqueue
        for(typename DLinkedList<T>::Iterator it = vertices.begin(); 
            it != vertices.end(); ++it) {
            if(in_degree_counter.get(*it) == 0) {
                bfsqueue.push(*it);
            }
        }
        
        while(!bfsqueue.empty()) {
            T current = bfsqueue.pop();
            sort_result.add(current);
            
            // getDests đã trả về các đỉnh theo thứ tự thêm cạnh
            DLinkedList<T> neighbors = getDests(current);
            
            // Xử lý các đỉnh kề theo đúng thứ tự trong danh sách kề
            for(typename DLinkedList<T>::Iterator it = neighbors.begin(); 
                it != neighbors.end(); ++it) {
                int newDegree = in_degree_counter.get(*it) - 1;
                in_degree_counter.put(*it, newDegree);
                
                if(newDegree == 0) {
                    bfsqueue.push(*it); // Thêm vào bfsqueue theo thứ tự thêm cạnh
                }
            }
        }
        
        if(sort_result.size() != vertices.size()) {
            throw std::runtime_error("Graph contains a cycle");
        }
        
        return sort_result;
    }

    DLinkedList<T> dfsSort(bool sorted=true) {
        DLinkedList<T> sort_result;
        xMap<T, int> in_degree_counter = vertex2inDegree(hash_code);
        DLinkedList<T> vertices = graph->vertices();
        
        while(sort_result.size() < vertices.size()) {
            // Tìm tất cả đỉnh có indegree = 0
            DLinkedList<T> zero_indegree;
            Stack<T> temporary_dfs_stack;  // Stack tạm để đảo ngược thứ tự
            
            // Duyệt theo thứ tự ban đầu và đẩy vào stack tạm
            for(typename DLinkedList<T>::Iterator it = vertices.begin(); 
                it != vertices.end(); ++it) {
                if(in_degree_counter.get(*it) == 0) {
                    temporary_dfs_stack.push(*it);  // Đẩy vào stack để đảo ngược thứ tự
                }
            }
            
            // Đẩy từ stack tạm sang zero_indegree để có thứ tự ngược lại
            while(!temporary_dfs_stack.empty()) {
                zero_indegree.add(temporary_dfs_stack.pop());
            }
                
            T current;
            if(sorted) {
                // Nếu sorted=true: sắp xếp theo thứ tự tăng dần
                DLinkedListSE<T> sortedZeroInDegree(zero_indegree);
                sortedZeroInDegree.sort();
                current = *(sortedZeroInDegree.begin());
            } else {
                // Nếu sorted=false: lấy đỉnh đầu tiên từ danh sách đã đảo ngược
                current = *(zero_indegree.begin());
            }
            
            // Thêm vào kết quả và cập nhật đồ thị
            sort_result.add(current);
            in_degree_counter.put(current, -1);
            
            // Cập nhật indegree cho các đỉnh kề
            DLinkedList<T> neighbors = getDests(current);
            for(typename DLinkedList<T>::Iterator nIt = neighbors.begin(); 
                nIt != neighbors.end(); nIt++) {
                int degree = in_degree_counter.get(*nIt);
                if(degree > 0) {
                    in_degree_counter.put(*nIt, degree - 1);
                }
            }
        }
        
        return sort_result;
    }

protected:

    //Helper functions
    xMap<T, int> vertex2inDegree(int (*hash)(T&, int)) {
        xMap<T, int> indegree_table(hash);
        DLinkedList<T> vertices = graph->vertices();
        
        // Khởi tạo tất cả bậc vào là 0
        for(typename DLinkedList<T>::Iterator it = vertices.begin(); it != vertices.end(); ++it) {
            indegree_table.put(*it, graph->getInDegree(*it));
        }
        
        return indegree_table;
    }
    
    xMap<T, int> vertex2outDegree(int (*hash)(T&, int)) {
        xMap<T, int> outdegree_table(hash);
        DLinkedList<T> vertices = graph->vertices();
        
        // Khởi tạo bậc ra cho mỗi đỉnh
        for(typename DLinkedList<T>::Iterator it = vertices.begin(); it != vertices.end(); ++it) {
            outdegree_table.put(*it, graph->getOutDegree(*it));
        }
        
        return outdegree_table;
    }

    DLinkedList<T> listOfZeroInDegrees() {
        DLinkedList<T> zero_indegree_table;
        
        // Lấy map chứa bậc vào của tất cả các đỉnh
        xMap<T, int> indegree_table = vertex2inDegree(hash_code);
        
        // Lấy danh sách tất cả các đỉnh
        DLinkedList<T> vertices = graph->vertices();
        
        // Duyệt qua từng đỉnh và kiểm tra bậc vào
        for(typename DLinkedList<T>::Iterator it = vertices.begin(); it != vertices.end(); ++it) {
            if(indegree_table.get(*it) == 0) {
                zero_indegree_table.add(*it);
            }
        }
        
        return zero_indegree_table;
    }

    DLinkedList<T> getDests(T vertex) {
        return graph->getDestVertices(vertex);
    }

}; //TopoSorter
template<class T>
int TopoSorter<T>::DFS = 0;
template<class T>
int TopoSorter<T>::BFS = 1;

/////////////////////////////End of TopoSorter//////////////////////////////////


#endif /* TOPOSORTER_H */