#ifndef QUEUE_H
#define QUEUE_H

#include "list/DLinkedList.h"
#include "stacknqueue/IDeck.h"

template<class T>
class Queue: public IDeck<T>{
public:
    class Iterator; //forward declaration
    
protected:
    DLinkedList<T> list;
    void (*deleteUserData)(DLinkedList<T>*);
    bool (*itemEqual)(T& lhs, T& rhs);
    
public:
    Queue(
            void (*deleteUserData)(DLinkedList<T>*)=0, 
            bool (*itemEqual)(T&, T&)=0){
        this->itemEqual = itemEqual;
        this->deleteUserData = deleteUserData;
    }
    void push(T item){
        this->list.add(item); // Add the item to the end of the linked list
    }
    T pop(){
        // if (list.empty()) throw std::out_of_range("Queue is empty");
        // T item = list.get(0);  // Get the first item
        // list.removeAt(0);      // Remove the first item
        // return item;
        if (!list.empty()) {
            return list.removeAt(0); 
        }
        throw std::out_of_range("Queue is empty!");
    }
    T& peek(){
        if (!list.empty()) {
            return list.get(0);  
        }
        throw std::out_of_range("Queue is empty!");
    }
    bool empty(){
        return this->list.empty();
    }
    int size(){
        return this->list.size();       
    }
    void clear(){
        // Cần thêm xử lý deleteUserData nếu có
        if(this->deleteUserData != 0) {
            this->deleteUserData(&this->list);
        }
        this->list.clear();
    }
    bool remove(T item){
        typename DLinkedList<T>::Iterator it = list.begin();
        while (it != list.end()) {
            if (equals(*it, item, itemEqual)) {
                // Sửa lỗi: Không thể truyền &value vì deleteUserData nhận DLinkedList<T>*
                if(this->deleteUserData != 0) {
                    // Tạo một DLinkedList tạm để xử lý
                    DLinkedList<T> tempList;
                    tempList.add(*it);
                    this->deleteUserData(&tempList);
                }
                it.remove();
                return true;
            }
            ++it;
        }
        return false;
    }
    bool contains(T item){
        typename DLinkedList<T>::Iterator it = list.begin();
        while (it != list.end()) {
            if (equals(*it, item, itemEqual)) {
                return true;
            }
            ++it;
        }
        return false;
    }
    string  toString(string (*item2str)(T&)=0 ){
        stringstream os;
        os << "FRONT-TO-REAR: " << list.toString(item2str);
        return os.str();
    }
    void println(string (*item2str)(T&)=0 ){
        cout << toString(item2str) << endl;
    }
    
    ///
    
    Iterator front(){
        return Iterator(this, true);
    }
    Iterator rear(){
        return Iterator(this, false);
    }
    
private:
    static bool equals(T& lhs, T& rhs, bool (*itemEqual)(T&, T& )){
        if(itemEqual == 0) return lhs == rhs;
        else return itemEqual(lhs, rhs);
    }
    
//////////////////////////////////////////////////////////////////////
////////////////////////  INNER CLASSES DEFNITION ////////////////////
//////////////////////////////////////////////////////////////////////

public:
//Iterator: BEGIN
    class Iterator{
    private:
        Queue<T>* queue;
        typename DLinkedList<T>::Iterator listIt;
    public:
        Iterator(Queue<T>* queue=0, bool begin=true){
            this->queue = queue;
            if(begin){
                if(queue != 0) this->listIt = queue->list.begin();
                else this->listIt = 0;
            }
            else{
                if(queue != 0) this->listIt = queue->list.end();
                else this->listIt = 0;
            }
        }
        Iterator& operator=(const Iterator& iterator ){
            this->queue = iterator.queue;
            this->listIt = iterator.listIt;
            return *this;
        }
        
        T& operator*(){
            return *(this->listIt);
        }
        bool operator!=(const Iterator& iterator){
            return this->listIt != iterator.listIt;
        }
        // Prefix ++ overload 
        Iterator& operator++(){
            listIt++;
            return *this; 
        }
        // Postfix ++ overload 
        Iterator operator++(int){
            Iterator iterator = *this; 
            ++*this; 
            return iterator; 
        }
        void remove(void (*removeItem)(T)=0){
            listIt.remove(removeItem);
        }
    };
    //Iterator: END
};


#endif /* QUEUE_H */

