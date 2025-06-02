#ifndef DGRAPHMODEL_H
#define DGRAPHMODEL_H
#include "graph/AbstractGraph.h"

#include "stacknqueue/Queue.h"
#include "stacknqueue/Stack.h"
#include "hash/xMap.h"
#include "sorting/DLinkedListSE.h"

//////////////////////////////////////////////////////////////////////
///////////// GraphModel: Directed Graph Model    ////////////////////
//////////////////////////////////////////////////////////////////////


template<class T>
class DGraphModel: public AbstractGraph<T>{
private:
public:
    DGraphModel(
            bool (*vertexEQ)(T&, T&), 
            string (*vertex2str)(T&) ): 
        AbstractGraph<T>(vertexEQ, vertex2str){
    }
    
    void connect(T from, T to, float weight=0){
        // Lấy các VertexNode tương ứng với các đỉnh from và to
        typename AbstractGraph<T>::VertexNode* node_from_node = this->getVertexNode(from);
        if (node_from_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(from));
        }

        typename AbstractGraph<T>::VertexNode* node_to_node = this->getVertexNode(to);
        if (node_to_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(to));
        }
        // Kiểm tra nếu cạnh đã tồn tại
        typename AbstractGraph<T>::Edge* existingEdge = node_from_node->getEdge(node_to_node);
        if (existingEdge != nullptr) {
            // Nếu cạnh đã tồn tại, xóa cạnh cũ
            node_from_node->removeTo(node_to_node); // Điều này tự động cập nhật số in_degree_counter và out_degree_counter
        }

        // Thêm cạnh mới với trọng số mới
        node_from_node->connect(node_to_node, weight);
    }
    void disconnect(T from, T to){
        // Lấy các VertexNode tương ứng với các đỉnh from và to
        typename AbstractGraph<T>::VertexNode* node_from_node = this->getVertexNode(from);
        if (node_from_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(from));
        }

        typename AbstractGraph<T>::VertexNode* node_to_node = this->getVertexNode(to);
        if (node_to_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(to));
        }

        // Lấy cạnh từ từ đỉnh from đến đỉnh to
        typename AbstractGraph<T>::Edge* edge = node_from_node->getEdge(node_to_node);
        if (edge == nullptr) {
            throw EdgeNotFoundException("E(" + this->vertex2str(from) + "," + this->vertex2str(to) + ")");
        }

        // Xóa cạnh khỏi đồ thị
        node_from_node->removeTo(node_to_node);
    }
    void remove(T vertex){
        // Lấy VertexNode tương ứng với đỉnh cần xóa
        typename AbstractGraph<T>::VertexNode* remove_this_node = this->getVertexNode(vertex);
        if (remove_this_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(vertex));
        }

        // Duyệt qua tất cả các đỉnh trong đồ thị và xóa các cạnh liên kết đến hoặc từ remove_this_node
        typename DLinkedList<typename AbstractGraph<T>::VertexNode*>::Iterator it = this->nodeList.begin();
        while (it != this->nodeList.end()) {
            typename AbstractGraph<T>::VertexNode* node_current = *it;
            if (node_current != remove_this_node) {
                // Xóa cạnh từ node_current đến remove_this_node (nếu có)
                typename AbstractGraph<T>::Edge* edge = node_current->getEdge(remove_this_node);
                if (edge != nullptr) {
                    node_current->removeTo(remove_this_node);
                }
                // Xóa các cạnh từ remove_this_node đến node_current (cạnh từ remove_this_node đến node_current)
                edge = remove_this_node->getEdge(node_current);
                if (edge != nullptr) {
                    remove_this_node->removeTo(node_current);  // Xóa cạnh từ remove_this_node đến node_current
                }
            }
            ++it;
        }

        // Xóa đỉnh khỏi danh sách các đỉnh
        this->nodeList.removeItem(remove_this_node, [](typename AbstractGraph<T>::VertexNode* node) { delete node; });
    }
    //new method
    DLinkedList<T> getDestVertices(T from) {
        DLinkedList<T> result;
        typename AbstractGraph<T>::VertexNode* node_from_node = this->getVertexNode(from);
        if (node_from_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(from));
        }

        // Truy cập tới các vertices trong đồ thị
        DLinkedList<T> vertices = this->vertices();
        
        // Kiểm tra từng đỉnh xem có cạnh đi tới không và thêm vào theo thứ tự kết nối
        for(typename DLinkedList<T>::Iterator it = vertices.begin(); it != vertices.end(); ++it) {
            typename AbstractGraph<T>::VertexNode* node_to_node = this->getVertexNode(*it);
            if(node_from_node->getEdge(node_to_node) != nullptr) {
                result.add(*it);
            }
        }
        return result;
    }

    int getOutDegree(T vertex) {
        typename AbstractGraph<T>::VertexNode* node = this->getVertexNode(vertex);
        if (node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(vertex));
        }
        
        int out_degree_counter = 0;
        typename DLinkedList<typename AbstractGraph<T>::VertexNode*>::Iterator nodeIt;
        for(nodeIt = this->nodeList.begin(); nodeIt != this->nodeList.end(); ++nodeIt) {
            typename AbstractGraph<T>::VertexNode* node_current = *nodeIt;
            if(node->getEdge(node_current) != nullptr) {
                out_degree_counter++;
            }
        }
        return out_degree_counter;
    }

    int getInDegree(T vertex) {
        typename AbstractGraph<T>::VertexNode* targetNode = this->getVertexNode(vertex);
        if (targetNode == nullptr) {
            throw VertexNotFoundException(this->vertex2str(vertex));
        }
        
        int in_degree_counter = 0;
        typename DLinkedList<typename AbstractGraph<T>::VertexNode*>::Iterator nodeIt;
        for(nodeIt = this->nodeList.begin(); nodeIt != this->nodeList.end(); ++nodeIt) {
            typename AbstractGraph<T>::VertexNode* node_current = *nodeIt;
            if(node_current->getEdge(targetNode) != nullptr) {
                in_degree_counter++;
            }
        }
        return in_degree_counter;
    }
    //end new method

    static DGraphModel<T>* create(
            T* vertices, int nvertices, Edge<T>* edges, int nedges,
            bool (*vertexEQ)(T&, T&),
            string (*vertex2str)(T&)){
        // (a) Khởi tạo một đối tượng DGraphModel mới
        DGraphModel<T>* graph = new DGraphModel<T>(vertexEQ, vertex2str);

        // (b) Thêm tất cả các đỉnh vào đồ thị
        for (int i = 0; i < nvertices; ++i) {
            graph->add(vertices[i]);
        }

        // (c) Thêm tất cả các cạnh vào đồ thị
        for (int i = 0; i < nedges; ++i) {
            graph->connect(edges[i].from, edges[i].to, edges[i].weight);
        }

        // (d) Trả về con trỏ tới đồ thị được tạo
        return graph;
    }
};

#endif /* DGRAPHMODEL_H */

