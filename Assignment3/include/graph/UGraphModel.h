#ifndef UGRAPHMODEL_H
#define UGRAPHMODEL_H

#include "graph/AbstractGraph.h"


//////////////////////////////////////////////////////////////////////
///////////// UGraphModel: Undirected Graph Model ////////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
class UGraphModel : public AbstractGraph<T>
{
private:
public:
    // class UGraphAlgorithm;
    // friend class UGraphAlgorithm;

    UGraphModel(
        bool (*vertexEQ)(T &, T &),
        string (*vertex2str)(T &)) : AbstractGraph<T>(vertexEQ, vertex2str)
    {
    }

    void connect(T from, T to, float weight = 0)
    {
        // Lấy các VertexNode tương ứng với các đỉnh from và to
        typename AbstractGraph<T>::VertexNode* node_from_node = this->getVertexNode(from);
        if (node_from_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(from));  // Ném ngoại lệ nếu không tìm thấy đỉnh from
        }
        
        typename AbstractGraph<T>::VertexNode* node_to_node = this->getVertexNode(to);
        if (node_to_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(to));  // Ném ngoại lệ nếu không tìm thấy đỉnh to
        }
        // Nếu from và to là cùng một đỉnh, thêm cạnh tự vòng
        if (from == to) {
            node_from_node->connect(node_to_node, weight);  // Thêm cạnh tự vòng từ from đến to
        } else {
            // Nếu từ from đến to đã tồn tại, xóa cạnh cũ để đồng bộ trọng số
            typename AbstractGraph<T>::Edge* exist_edge = node_from_node->getEdge(node_to_node);
            if (exist_edge != nullptr) {
                node_from_node->removeTo(node_to_node);
                node_to_node->removeTo(node_from_node);
            }

            // Thêm cạnh từ from đến to và từ to đến from với cùng trọng số
            node_from_node->connect(node_to_node, weight);
            node_to_node->connect(node_from_node, weight);  // Dùng cùng trọng số như cạnh từ `from` đến `to`
        }
    }
    void disconnect(T from, T to)
    {
        // Lấy các VertexNode tương ứng với các đỉnh from và to
        typename AbstractGraph<T>::VertexNode* node_from_node = this->getVertexNode(from);
        if (node_from_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(from));  // Ném ngoại lệ nếu không tìm thấy đỉnh from
        }

        typename AbstractGraph<T>::VertexNode* node_to_node = this->getVertexNode(to);
        if (node_to_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(to));  // Ném ngoại lệ nếu không tìm thấy đỉnh to
        }

        // Nếu from và to là cùng một đỉnh, xóa cạnh tự vòng
        if (from == to) {
            typename AbstractGraph<T>::Edge* edge = node_from_node->getEdge(node_to_node);
            if (edge == nullptr) {
                throw EdgeNotFoundException("E(" + this->vertex2str(from) + "," + this->vertex2str(to) + ")");  // Ném ngoại lệ nếu không có cạnh
            }
            node_from_node->removeTo(node_to_node);  // Xóa cạnh tự vòng
        }
        else {
            // Nếu from và to khác nhau, xóa cả hai cạnh (từ from đến to và ngược lại)
            typename AbstractGraph<T>::Edge* edge1 = node_from_node->getEdge(node_to_node);
            if (edge1 == nullptr) {
                throw EdgeNotFoundException("E(" + this->vertex2str(from) + "," + this->vertex2str(to) + ")");  // Ném ngoại lệ nếu không có cạnh
            }
            node_from_node->removeTo(node_to_node);  // Xóa cạnh từ from đến to

            typename AbstractGraph<T>::Edge* edge2 = node_to_node->getEdge(node_from_node);
            if (edge2 == nullptr) {
                throw EdgeNotFoundException("E(" + this->vertex2str(from) + "," + this->vertex2str(to) + ")");  // Ném ngoại lệ nếu không có cạnh
            }
            node_to_node->removeTo(node_from_node);  // Xóa cạnh từ to đến from
        }
    }
    void remove(T vertex)
    {
        // Lấy VertexNode tương ứng với đỉnh cần xóa
        typename AbstractGraph<T>::VertexNode* remove_this_node = this->getVertexNode(vertex);
        if (remove_this_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(vertex));  // Ném ngoại lệ nếu không tìm thấy đỉnh
        }

        // Duyệt qua tất cả các đỉnh trong đồ thị, xóa các cạnh kết nối đến đỉnh cần xóa
        typename DLinkedList<typename AbstractGraph<T>::VertexNode*>::Iterator it = this->nodeList.begin();
        while (it != this->nodeList.end()) {
            typename AbstractGraph<T>::VertexNode* node_current = *it;
            if (node_current != remove_this_node) {
                // Xóa các cạnh kết nối từ node_current đến remove_this_node
                typename AbstractGraph<T>::Edge* edge = node_current->getEdge(remove_this_node);
                if (edge != nullptr) {
                    node_current->removeTo(remove_this_node);  // Xóa cạnh từ node_current đến remove_this_node
                }
                // Xóa các cạnh từ remove_this_node đến node_current (cạnh từ remove_this_node đến node_current)
                edge = remove_this_node->getEdge(node_current);
                if (edge != nullptr) {
                    remove_this_node->removeTo(node_current);  // Xóa cạnh từ remove_this_node đến node_current
                }
            }
            ++it;
        }

        // Xóa đỉnh khỏi danh sách các đỉnh của đồ thị
        this->nodeList.removeItem(remove_this_node, [](typename AbstractGraph<T>::VertexNode* node) { delete node; });
    }

    static UGraphModel<T> *create(
        T *vertices, int nvertices, Edge<T> *edges, int nedges,
        bool (*vertexEQ)(T &, T &),
        string (*vertex2str)(T &))
    {
        // (a) Khởi tạo đối tượng UGraphModel mới
        UGraphModel<T>* graph = new UGraphModel<T>(vertexEQ, vertex2str);
        
        // (b) Thêm tất cả các đỉnh vào đồ thị
        for (int i = 0; i < nvertices; ++i) {
            graph->add(vertices[i]);  // Thêm từng đỉnh vào đồ thị
        }
        
        // (c) Thêm tất cả các cạnh vào đồ thị
        for (int i = 0; i < nedges; ++i) {
            graph->connect(edges[i].from, edges[i].to, edges[i].weight);  // Thêm cạnh vào đồ thị
        }
        
        // (d) Trả về con trỏ đến đồ thị đã tạo
        return graph;
    }
};

#endif /* UGRAPHMODEL_H */
