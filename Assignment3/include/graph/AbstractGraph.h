
#ifndef ABSTRACTGRAPH_H
#define ABSTRACTGRAPH_H
#include "graph/IGraph.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;


template<class T>
class AbstractGraph: public IGraph<T>{
public:
    class Edge; //forward declaration
    class VertexNode; //forward declaration
    class Iterator; //forward declaration
    
private:
protected:
    //Using the adjacent list technique, so need to store list of nodes (nodeList)
    DLinkedList<VertexNode*> nodeList; 
    
    //Function pointers:
    bool (*vertexEQ)(T&, T&); //to compare two vertices
    string (*vertex2str)(T&); //to obtain string representation of vertices
    
    
    VertexNode* getVertexNode(T& vertex){
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while(it != nodeList.end()){
            VertexNode* node = *it;
            if(vertexEQ(node->vertex, vertex) ) return node;
            it++;
        }
        return 0;
    }
    string vertex2Str(VertexNode& node){
        return vertex2str(node.vertex);
    }
    string edge2Str(Edge& edge){
        stringstream os;
        os << "E("
                << vertex2str(edge.from->vertex)
                << ","
                << vertex2str(edge.to->vertex)
                << ")";
        return os.str();
    }
    
public:
    AbstractGraph(
            bool (*vertexEQ)(T&, T&)=0, 
            string (*vertex2str)(T&)=0){
        
        this->vertexEQ = vertexEQ;
        this->vertex2str = vertex2str;
    }
    virtual ~AbstractGraph(){}
    
    typedef bool (*vertexEQFunc)(T&, T&);
    typedef string (*vertex2strFunc)(T&);
    vertexEQFunc getVertexEQ(){
        return this->vertexEQ;
    }
    vertex2strFunc getVertex2Str(){
        return this->vertex2str;
    }

    
//////////////////////////////////////////////////////////////////////
//////////////////// IMPLEMENTATION of IGraph API ////////////////////
//////////////////////////////////////////////////////////////////////
    /* connect, disconnect, and remove: will be overridden in:
     *  >> UGraphModel
     *  >> DGraphModel
     */
    virtual void connect(T from, T to, float weight=0) =0;
    virtual void disconnect(T from, T to)=0;
    virtual void remove(T vertex)=0;
    
    
    
    /* The following are common methods for UGraphModel and DGraphModel
     */
    virtual void add(T vertex) {
        // Kiểm tra xem đỉnh đã tồn tại chưa
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while (it != nodeList.end()) {
            VertexNode* node = *it;
            if (vertexEQ(node->vertex, vertex)) {
                return; // Đỉnh đã tồn tại, không làm gì cả
            }
            it++;
        }
        
        // Nếu chưa tồn tại, thêm một đỉnh mới vào đồ thị
        VertexNode* newNode = new VertexNode(vertex, vertexEQ, vertex2str);
        nodeList.add(newNode);
    }
    virtual bool contains(T vertex){
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while (it != nodeList.end()) {
            VertexNode* node = *it;
            if (vertexEQ(node->vertex, vertex)) {
                return true; // Tìm thấy đỉnh
            }
            it++;
        }
        return false; // Không tìm thấy đỉnh
    }
    virtual float weight(T from, T to){
        // Tìm đối tượng VertexNode tương ứng với đỉnh `from`
        VertexNode* node_from_node = getVertexNode(from);
        if (node_from_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(from));
        }

        VertexNode* node_to_node = getVertexNode(to);
        if (node_to_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(to));
        }

        // Tìm cạnh nối từ `from` đến `to`
        Edge* edge = node_from_node->getEdge(getVertexNode(to));
        if (edge == nullptr) {
            throw EdgeNotFoundException("E(" + this->vertex2str(from) + "," + this->vertex2str(to) + ")");
        }

        return edge->weight; // Trả về trọng số của cạnh
    }

    virtual DLinkedList<T> getOutwardEdges(T from){
        // Tìm VertexNode tương ứng với đỉnh `from`
        VertexNode* node_from_node = getVertexNode(from);
        if (node_from_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(from));
        }

        // Trả về danh sách các cạnh đi ra từ đỉnh `from`
        return node_from_node->getOutwardEdges();
    }
    
    virtual DLinkedList<T> getInwardEdges(T to){
        // Tìm VertexNode tương ứng với đỉnh `to`
        VertexNode* node_to_node = getVertexNode(to);
        if (node_to_node == nullptr) {
            throw VertexNotFoundException(this->vertex2str(to));
        }

        // Khởi tạo danh sách để lưu các đỉnh có cạnh đi vào `to`
        DLinkedList<T> edges_inward;

        // Duyệt qua tất cả các VertexNode trong nodeList
        typename DLinkedList<VertexNode*>::Iterator nodeIt = nodeList.begin();
        while (nodeIt != nodeList.end()) {
            VertexNode* node_current = *nodeIt;

            // Duyệt qua danh sách các cạnh kề của node_current
            typename DLinkedList<Edge*>::Iterator edge_iterator = node_current->adList.begin();
            while (edge_iterator != node_current->adList.end()) {
                Edge* edge = *edge_iterator;

                // Kiểm tra nếu cạnh có đỉnh đích là `to`
                if (vertexEQ(edge->to->vertex, node_to_node->vertex)) {
                    edges_inward.add(node_current->vertex); // Thêm đỉnh nguồn vào danh sách
                }

                ++edge_iterator;
            }

            ++nodeIt;
        }

        return edges_inward; // Trả về danh sách các đỉnh có cạnh đi vào `to`
    }
    
    virtual int size() {
        return nodeList.size(); // Return the number of vertices in the graph
    }

    virtual bool empty(){
        return nodeList.empty(); // Check if the graph is empty
    }

    virtual void clear(){
        // Duyệt qua từng VertexNode trong danh sách nodeList
        typename DLinkedList<VertexNode*>::Iterator nodeIt = nodeList.begin();
        while (nodeIt != nodeList.end()) {
            VertexNode* node = *nodeIt;

            // Xóa tất cả các cạnh liên kết với đỉnh này
            typename DLinkedList<Edge*>::Iterator edge_iterator = node->adList.begin();
            while (edge_iterator != node->adList.end()) {
                Edge* edge = *edge_iterator;
                delete edge; // Giải phóng bộ nhớ cho từng cạnh
                ++edge_iterator;
            }

            // Xóa danh sách cạnh của VertexNode
            node->adList.clear();

            // Giải phóng bộ nhớ cho VertexNode
            delete node;
            ++nodeIt;
        }

        // Xóa danh sách nodeList (giải phóng toàn bộ đỉnh)
        nodeList.clear();
    }

    virtual int inDegree(T vertex){
        VertexNode* node = this->getVertexNode(vertex);
        if(node == nullptr) throw VertexNotFoundException(this->vertex2str(vertex)); // Use 'vertex' directly
        return node->inDegree(); // Return the in-degree of the vertex
    }

    virtual int outDegree(T vertex){
        VertexNode* node = this->getVertexNode(vertex);
        if(node == nullptr) throw VertexNotFoundException(this->vertex2str(vertex)); // Use 'vertex' directly
        return node->outDegree(); // Return the out-degree of the vertex
    }
    
    virtual DLinkedList<T> vertices(){
        DLinkedList<T> list;
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while(it != nodeList.end()){
            VertexNode* node = *it;
            list.add(node->vertex); // Thêm đỉnh vào danh sách
            ++it;
        }
        return list;
    }

    virtual bool connected(T from, T to){
        VertexNode* node_from_node = this->getVertexNode(from);
        if(node_from_node == nullptr) throw VertexNotFoundException(this->vertex2str(from)); // Use 'from' directly

        VertexNode* node_to_node = this->getVertexNode(to);
        if(node_to_node == nullptr) throw VertexNotFoundException(this->vertex2str(to)); // Use 'to' directly

        Edge* edge = node_from_node->getEdge(node_to_node);
        return edge != nullptr;
    }

    void println(){
        cout << this->toString() << endl;
    }
    virtual string toString(){
        string mark(50, '=');
        stringstream os;
        os << mark << endl;
        os << left << setw(12) << "Vertices:" << endl;
        typename DLinkedList<VertexNode*>::Iterator nodeIt = nodeList.begin();
        while(nodeIt != nodeList.end()){
            VertexNode* node = *nodeIt;
            os << node->toString() << endl;
            nodeIt++;
        }
        string mark2(30, '-');
        os << mark2 << endl;
        
        os << left << setw(12) << "Edges:" << endl;
        
        nodeIt = nodeList.begin();
        while(nodeIt != nodeList.end()){
            VertexNode* node = *nodeIt;
            
            typename DLinkedList<Edge*>::Iterator edge_iterator = node->adList.begin();
            while(edge_iterator != node->adList.end()){
                Edge* edge = *edge_iterator;
                os << edge->toString() << endl;
                
                edge_iterator++;
            }
            
            nodeIt++;
        }
        os << mark << endl;
        return os.str();
    }
    
    /* begin, end: will be used to traverse on graph's vertices
     * example: assume that "graph" is a graph.
     * Code:
     *      AbstractGraph<T>::Iterator it;
     *      for(it = graph.begin(); it != graph.end(); it++){
     *          T vertex = *it; 
     *          //continue to process vertex here!
     *      }
     */
    Iterator begin(){
        return Iterator(this, true);
    }
    Iterator end(){
        return Iterator(this, false);
    }
    

//////////////////////////////////////////////////////////////////////
////////////////////////  INNER CLASSES DEFNITION ////////////////////
//////////////////////////////////////////////////////////////////////
    
public:
//BEGIN of VertexNode    
    class VertexNode{
    private:
        template<class U>
        friend class UGraphModel; //UPDATED: added
        // friend class DGraphModel;
        T vertex;
        int inDegree_, outDegree_;
        DLinkedList<Edge*> adList; 
        friend class Edge;
        friend class AbstractGraph;
        
        bool (*vertexEQ)(T&, T&);
        string (*vertex2str)(T&);
        
    public:
        VertexNode():adList(&DLinkedList<Edge*>::free, &Edge::edgeEQ){}
        VertexNode(T vertex, bool (*vertexEQ)(T&, T&), string (*vertex2str)(T&))
            :adList(&DLinkedList<Edge*>::free, &Edge::edgeEQ){
            this->vertex = vertex;
            this->vertexEQ = vertexEQ;
            this->vertex2str = vertex2str;
            this->outDegree_ = this->inDegree_ = 0;
        }
        T& getVertex(){
            return vertex;
        }
        void connect(VertexNode* to, float weight = 0){
            // Kiểm tra nếu cạnh đã tồn tại thì không tạo mới
            typename DLinkedList<Edge*>::Iterator it = adList.begin();
            while (it != adList.end()) {
                Edge* existingEdge = *it;
                if (existingEdge->to == to) {
                    // Nếu đã có cạnh, thoát mà không thêm mới
                    return;
                }
                ++it;
            }
            
            // Tạo một cạnh mới
            Edge* newEdge = new Edge(this, to, weight);
            adList.add(newEdge); // Thêm cạnh vào danh sách kề
            
            // Cập nhật bậc ra (outDegree) của đỉnh hiện tại
            this->outDegree_++;
            
            // Cập nhật bậc vào (inDegree) của đỉnh đích
            to->inDegree_++;
        }

        DLinkedList<T> getOutwardEdges(){
            DLinkedList<T> edges;
            typename DLinkedList<Edge*>::Iterator it = adList.begin();
            while (it != adList.end()) {
                Edge* edge = *it;
                edges.add(edge->to->vertex); // Lấy đỉnh đích của mỗi cạnh
                ++it;
            }
            return edges;
        }

        Edge* getEdge(VertexNode* to){
            typename DLinkedList<Edge*>::Iterator it = adList.begin();
            while(it != adList.end()){
                if((*it)->to == to){///////////fix
                    return *it;
                }
                ++it;
            }
            return nullptr; // Return nullptr if edge is not found
        }

        bool equals(VertexNode* node){
            if (node == nullptr) return false;
            return vertexEQ(this->vertex, node->vertex); // Compare vertices using vertexEQ
        }

        void removeTo(VertexNode* to){
            Edge* edge = getEdge(to);
            if(edge){
                adList.removeItem(edge, [](Edge* e) { delete e; }); // Use lambda to delete the edge
                this->outDegree_--;
                to->inDegree_--;
            }
        }

        int inDegree(){
            return inDegree_;
        }

        int outDegree(){
            return outDegree_;
        }

        string toString(){
            stringstream os;
            os << "V("
                    << this->vertex << ", "
                    << "in: " << this->inDegree_ << ", "
                    << "out: " << this->outDegree_ << ")";
            return os.str();
        }

        void clear(){
            // Delete all Edge pointers in the adjacency list
            typename DLinkedList<Edge*>::Iterator it = adList.begin();
            while(it != adList.end()){
                delete *it;
                ++it;
            }
            adList.clear();          // Clear the adjacency list
            inDegree_ = outDegree_ = 0; // Reset degrees
        }

        ~VertexNode(){
            clear(); // Call clear to release memory
        }
    };
//END of VertexNode    
    
//BEGIN of Edge
    class Edge{
    private:
        VertexNode* from;
        VertexNode* to;
        float weight;
        friend class VertexNode;
        friend class AbstractGraph;
        
    public:
        Edge(){}
        Edge(VertexNode* from, VertexNode* to, float weight=0){
            this->from = from;
            this->to = to;
            this->weight = weight;
        }
        
        bool equals(Edge* edge){
            if (edge == nullptr) {
                return false;  // Nếu cạnh so sánh là nullptr, trả về false
            }
            return (this->from->vertex == edge->from->vertex) && (this->to->vertex == edge->to->vertex);
        }

        static bool edgeEQ(Edge*& edge1, Edge*& edge2){
            return edge1->equals(edge2);
        }
        string toString(){
            stringstream os;
            os << "E("
                    << this->from->vertex
                    << ","
                    << this->to->vertex
                    << ","
                    << this->weight
                    << ")";
            return os.str();
        }
    };
 //END of Edge
    

//BEGIN of Iterator
public:
    class Iterator{
    private:
        typename DLinkedList<VertexNode*>::Iterator nodeIt;
        
    public:
        Iterator(AbstractGraph<T>* pGraph=0, bool begin=true){
            if(begin) {
                if(pGraph != 0) nodeIt = pGraph->nodeList.begin();
            }
            else{
                if(pGraph != 0) nodeIt = pGraph->nodeList.end();
            }
        }
        Iterator& operator=(const Iterator& iterator){
            this->nodeIt = iterator.nodeIt;
            return *this;
        }
        
        T& operator*(){
            return (*nodeIt)->vertex;
        }
        
        bool operator!=(const Iterator& iterator){
            return nodeIt != iterator.nodeIt;
        }
        // Prefix ++ overload 
        Iterator& operator++(){
            nodeIt++;
            return *this; 
        }
        // Postfix ++ overload 
        Iterator operator++(int){
            Iterator iterator = *this; 
            ++*this; 
            return iterator; 
        }
    };
//END of Iterator
};

#endif /* ABSTRACTGRAPH_H */

