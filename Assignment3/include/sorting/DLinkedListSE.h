#ifndef DLINKEDLISTSE_H
#define DLINKEDLISTSE_H
#include "list/DLinkedList.h"
#include "sorting/ISort.h"

template<class T>
class DLinkedListSE: public DLinkedList<T>{
public:
    
    DLinkedListSE(
            void (*removeData)(DLinkedList<T>*)=0, 
            bool (*itemEQ)(T&, T&)=0 ) : 
            DLinkedList<T>(removeData, itemEQ){
        
    };
    
    DLinkedListSE(const DLinkedList<T>& list){
        this->copyFrom(list);
    }
    
    void sort(int (*comparator)(T&,T&)=0){
        if(this->count <= 1) return;

        // Mảng tạm để lưu các Node* cho quá trình merge
        typename DLinkedList<T>::Node** node_storage_array = new typename DLinkedList<T>::Node*[this->count];
        int array_index = 0;
        
        // Chuyển linked list sang array of pointers
        typename DLinkedList<T>::Node* current_node = this->head->next;
        while(current_node != this->tail) {
            node_storage_array[array_index++] = current_node;
            current_node = current_node->next;
        }

        // Bottom-up merge sort
        typename DLinkedList<T>::Node** address_of_temporary_node = new typename DLinkedList<T>::Node*[this->count];
        for(int width = 1; width < this->count; width = width * 2) {
            for(int i = 0; i < this->count; i = i + 2 * width) {
                // Merge hai phần đã sắp xếp
                int left_array = i;
                int middle_of_array = std::min(i + width, this->count);
                int right_array = std::min(i + 2 * width, this->count);
                int k = left_array;
                int i1 = left_array, i2 = middle_of_array;
                
                // Merge hai mảng con
                while(i1 < middle_of_array && i2 < right_array) {
                    if(compare(node_storage_array[i1]->data, node_storage_array[i2]->data, comparator) <= 0)
                        address_of_temporary_node[k++] = node_storage_array[i1++];
                    else
                        address_of_temporary_node[k++] = node_storage_array[i2++];
                }
                
                // Copy phần còn lại của mảng bên trái
                while(i1 < middle_of_array)
                    address_of_temporary_node[k++] = node_storage_array[i1++];
                    
                // Copy phần còn lại của mảng bên phải
                while(i2 < right_array)
                    address_of_temporary_node[k++] = node_storage_array[i2++];
                    
                // Copy kết quả merge vào mảng gốc
                for(k = left_array; k < right_array; k++)
                    node_storage_array[k] = address_of_temporary_node[k];
            }
        }

        // Xây dựng lại linked list từ mảng đã sắp xếp
        this->head->next = node_storage_array[0];
        node_storage_array[0]->prev = this->head;
        
        for(int i = 0; i < this->count - 1; i++) {
            node_storage_array[i]->next = node_storage_array[i + 1];
            node_storage_array[i + 1]->prev = node_storage_array[i];
        }
        
        node_storage_array[this->count - 1]->next = this->tail;
        this->tail->prev = node_storage_array[this->count - 1];

        // Giải phóng bộ nhớ
        delete[] address_of_temporary_node;
        delete[] node_storage_array;
    };
    
protected:
    static int compare(T& lhs, T& rhs, int (*comparator)(T&,T&)=0){
        if(comparator != 0) return comparator(lhs, rhs);
        else{
            if(lhs < rhs) return -1;
            else if(lhs > rhs) return +1;
            else return 0;
        }
    }
};

#endif /* DLINKEDLISTSE_H */

