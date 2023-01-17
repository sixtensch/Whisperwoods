#pragma once

#include <string>
#include <vector>

namespace cs
{

    template<typename T_key, typename T_val>
    class ShuffleMap;

    template <typename T, bool UniqueKeys = false>
    class RBTree
    {
        template<typename T_key, typename T_val>
        friend class ShuffleMap;

    public:
        enum class Color { RED, BLACK };

        struct Node
        {
            Node* parent = nullptr;
            Node* leftChild = nullptr;
            Node* rightChild = nullptr;
            Color colour = Color::RED;
            T element = T();
        };



    public:
        RBTree();
        ~RBTree();

        RBTree(RBTree&& rVal);
        RBTree& operator=(RBTree&& rVal);

        RBTree(const RBTree& lVal) = delete;
        RBTree& operator=(const RBTree& lVal) = delete;

        /// Searches while the element != an element in the tree
        bool Add(const T& element);     
        /// Searches until the element == an element in the tree
        bool Delete(const T& element);
        /// Searches while the element != an element in the tree
        bool Find(const T& element);

        const T& Min();
        const T& Max();

        std::vector<T> InOrder();
        std::vector<T> PathFromRoot(const T& element);
        int Height();

        void Clear();



    //public:
    //    enum class Color { RED, BLACK };


        //Node* FindNode(T key);

    private:
        void LeftRotate(Node* node);
        void RightRotate(Node* node);
        void Transplant(Node* target, Node* source);

        void BranchDelete(Node* node);
        void BranchInOrder(Node* node, std::vector<T>& order);
        int BranchGetDepth(Node* node);

        void AddRepair(Node* newNode);
        void DeleteRepair(Node* target);

        Node* root = nullptr;
        Node* nilNode = nullptr;
    };



    // ----



    template <typename T, bool UniqueKeys>
    RBTree<T, UniqueKeys>::RBTree()
    {
        nilNode = new Node();
        nilNode->colour = Color::BLACK;

        root = nilNode;
    }

    template <typename T, bool UniqueKeys>
    RBTree<T, UniqueKeys>::~RBTree()
    {
        if (root == nullptr)
        {
            return;
        }

        if (root != nilNode)
        {
            BranchDelete(root);
            delete root;
        }

        delete nilNode;
    }

    template <typename T, bool UniqueKeys>
    RBTree<T, UniqueKeys>::RBTree(RBTree&& rVal)
    {
        *this = std::move(rVal);
    }

    template <typename T, bool UniqueKeys>
    RBTree<T, UniqueKeys>& RBTree<T, UniqueKeys>::operator=(RBTree&& rVal)
    {
        if (*this == rVal)
        {
            return;
        }

        if (root != nilNode)
        {
            BranchDelete(root);
            delete root;
        }

        delete nilNode;

        root = rVal.root;
        nilNode = rVal.nilNode;

        rVal.root = nullptr;
        rVal.nilNode = nullptr;
    }

    template <typename T, bool UniqueKeys>
    bool RBTree<T, UniqueKeys>::Add(const T& element)
    {
        Node* previous = nullptr;
        Node* current = root;

        bool leftChild = false;

        while (current != nilNode && (!UniqueKeys || current->element != element))
        {
            previous = current;

            if (element > current->element)
            {
                current = current->rightChild;
                leftChild = false;
            }
            else
            {
                current = current->leftChild;
                leftChild = true;
            }
        }

        if (!UniqueKeys && current != nilNode && current->element == element)
        {
            // Found the element, no insertion necessary.
            return false;
        }

        Node* newNode = new Node();
        newNode->element = element;
        newNode->leftChild = nilNode;
        newNode->rightChild = nilNode;

        if (root == nilNode)
        {
            // Fills in the root
            root = newNode;
            root->parent = nilNode;
            root->colour = Color::BLACK;

            return true;
        }

        // Is child of some node

        newNode->parent = previous;

        Node*& childPointer = (leftChild ? previous->leftChild : previous->rightChild);
        childPointer = newNode;

        AddRepair(newNode);

        return true;
    }

    template <typename T, bool UniqueKeys>
    bool RBTree<T, UniqueKeys>::Delete(const T& element)
    {
        Node* z = root;

        while (!(z == nilNode) && !(z->element == element))
        {
            bool larger = element > z->element;

            if (larger)
            {
                z = z->rightChild;
            }
            else
            {
                z = z->leftChild;
            }
        }

        if (z == nilNode)
        {
            // Couldn't find the element.
            return false;
        }

        Node* x = nilNode;
        Node* y = z;
        Color yColor = y->colour;

        if (z->rightChild == nilNode)
        {
            x = z->leftChild;
            Transplant(z, z->leftChild);
        }
        else if (z->leftChild == nilNode)
        {
            x = z->rightChild;
            Transplant(z, z->rightChild);
        }
        else
        {
            y = z->rightChild;
            while (y->leftChild != nilNode)
            {
                y = y->leftChild;
            }

            yColor = y->colour;

            x = y->rightChild;

            if (y->parent == z)
            {
                // Will potentially leave junk in the nilNode variable!
                // Reset at the end of DeleteRepair
                x->parent = y;
            }
            else
            {
                Transplant(y, y->rightChild);
                y->rightChild = z->rightChild;
                y->rightChild->parent = y;
            }

            Transplant(z, y);
            y->leftChild = z->leftChild;
            y->leftChild->parent = y;
            y->colour = z->colour;
        }

        // Make sure to actually delete the data
        delete z;

        if (yColor == Color::BLACK)
        {
            DeleteRepair(x);
        }

        return true;
    }

    template <typename T, bool UniqueKeys>
    bool RBTree<T, UniqueKeys>::Find(const T& element)
    {
        Node* current = root;

        while (current != nilNode && current->element != element)
        {
            if (element > current->element)
            {
                current = current->rightChild;
            }
            else
            {
                current = current->leftChild;
            }
        }

        return current != nilNode;
    }

    template <typename T, bool UniqueKeys>
    const T& RBTree<T, UniqueKeys>::Min()
    {
        if (root == nilNode)
        {
            throw std::out_of_range("Tried to call min() for empty tree.");
        }


        Node* previous = nilNode;
        Node* current = root;

        while (current != nilNode)
        {
            previous = current;
            current = current->leftChild;
        }

        return previous->element;
    }

    template <typename T, bool UniqueKeys>
    const T& RBTree<T, UniqueKeys>::Max()
    {
        if (root == nilNode)
        {
            throw std::out_of_range("Tried to call max() for empty tree.");
        }

        Node* previous = nilNode;
        Node* current = root;

        while (current != nilNode)
        {
            previous = current;
            current = current->rightChild;
        }

        return previous->element;
    }

    template <typename T, bool UniqueKeys>
    std::vector<T> RBTree<T, UniqueKeys>::InOrder()
    {
        std::vector<T> order;

        if (root != nilNode)
        {
            BranchInOrder(root, order); // Recursively fill the vector
        }

        return order;
    }

    template <typename T, bool UniqueKeys>
    int RBTree<T, UniqueKeys>::Height()
    {
        return BranchGetDepth(root) - 1;
    }

    template<typename T, bool UniqueKeys>
    void RBTree<T, UniqueKeys>::Clear()
    {
        if (root != nilNode)
        {
            BranchDelete(root);
            delete root;

            root = nilNode;
        }
    }

    template <typename T, bool UniqueKeys>
    std::vector<T> RBTree<T, UniqueKeys>::PathFromRoot(const T& element)
    {
        std::vector<T> path;

        Node* current = root;

        while (current != nilNode && current->element != element)
        {
            path.push_back(current->element);

            if (element > current->element)
            {
                current = current->rightChild;
            }
            else
            {
                current = current->leftChild;
            }
        }

        if (current == nilNode)
        {
            // The element wasn't found
            path.clear();
            return path;
        }

        path.push_back(current->element);

        return path;
    }

    template <typename T, bool UniqueKeys>
    void RBTree<T, UniqueKeys>::LeftRotate(Node* node)
    {
        if (node->rightChild == nilNode)
        {
            return;
        }

        Node* child = node->rightChild;
        Node* childLeft = child->leftChild;

        child->parent = node->parent;
        child->leftChild = node;

        node->parent = child;
        node->rightChild = childLeft;

        if (root == node)
        {
            root = child;
        }
        else
        {
            if (child->parent->leftChild == node)
            {
                child->parent->leftChild = child;
            }
            else
            {
                child->parent->rightChild = child;
            }
        }

        if (childLeft != nilNode)
        {
            childLeft->parent = node;
        }
    }

    template <typename T, bool UniqueKeys>
    void RBTree<T, UniqueKeys>::RightRotate(Node* node)
    {
        if (node->leftChild == nilNode)
        {
            return;
        }

        Node* child = node->leftChild;
        Node* childRight = child->rightChild;

        child->parent = node->parent;
        child->rightChild = node;

        node->parent = child;
        node->leftChild = childRight;

        if (root == node)
        {
            root = child;
        }
        else
        {
            if (child->parent->leftChild == node)
            {
                child->parent->leftChild = child;
            }
            else
            {
                child->parent->rightChild = child;
            }
        }

        if (childRight != nilNode)
        {
            childRight->parent = node;
        }
    }

    template <typename T, bool UniqueKeys>
    void RBTree<T, UniqueKeys>::Transplant(Node* target, Node* source)
    {
        if (target == root)
        {
            root = source;
        }
        else if (target->parent->leftChild == target)
        {
            target->parent->leftChild = source;
        }
        else
        {
            target->parent->rightChild = source;
        }

        source->parent = target->parent;
    }

    template <typename T, bool UniqueKeys>
    void RBTree<T, UniqueKeys>::BranchDelete(Node* node)
    {
        // Recursively delete
        // All of it
        // It's got to go
        // I'm sorry
        // Blood for the blood god

        if (node->leftChild != nilNode)
        {
            BranchDelete(node->leftChild);
            delete node->leftChild;
        }

        if (node->rightChild != nilNode)
        {
            BranchDelete(node->rightChild);
            delete node->rightChild;
        }
    }

    template <typename T, bool UniqueKeys>
    void RBTree<T, UniqueKeys>::BranchInOrder(Node* node, std::vector<T>& order)
    {
        if (node->leftChild != nilNode)
        {
            BranchInOrder(node->leftChild, order);
        }

        order.push_back(node->element);

        if (node->rightChild != nilNode)
        {
            BranchInOrder(node->rightChild, order);
        }
    }

    template <typename T, bool UniqueKeys>
    int RBTree<T, UniqueKeys>::BranchGetDepth(Node* node)
    {
        if (node == nilNode)
        {
            return 0;
        }

        int lDepth = BranchGetDepth(node->leftChild);
        int rDepth = BranchGetDepth(node->rightChild);

        return 1 + (lDepth > rDepth ? lDepth : rDepth);
    }

    template <typename T, bool UniqueKeys>
    void RBTree<T, UniqueKeys>::AddRepair(Node* newNode)
    {
        if (newNode == root)
        {
            newNode->colour = Color::BLACK;
            return;
        }

        Node* current = newNode;

        while (current->parent->colour == Color::RED)
        {
            // If parent is left child
            if (current->parent == current->parent->parent->leftChild)
            {
                Node* pSibling = current->parent->parent->rightChild; // Right sibling of parent, possibly nilNode

                // CASE 1
                // Let black color "filter down" from grandparent to parents
                if (pSibling->colour == Color::RED)
                {
                    current->parent->parent->colour = Color::RED;
                    current->parent->colour = Color::BLACK;
                    pSibling->colour = Color::BLACK;

                    current = current->parent->parent; // Move cursor up to grandparent

                    continue;
                }

                if (current == current->parent->rightChild)
                {
                    // CASE 2
                    // If right child, move up to parent and rotate left
                    current = current->parent;
                    LeftRotate(current);
                }

                // CASE 3
                // Recolor parent and grandparent, rotate the structure left
                current->parent->colour = Color::BLACK;
                current->parent->parent->colour = Color::RED;
                RightRotate(current->parent->parent);
            }

            // If parent is right child
            // Everything is done in reverse
            else
            {
                Node* pSibling = current->parent->parent->leftChild; // Left sibling of parent, possibly nilNode

                // CASE 1
                // Both parent and its sibling are red
                // Let black color "filter down" from grandparent to parents
                if (pSibling->colour == Color::RED)
                {
                    current->parent->parent->colour = Color::RED;
                    current->parent->colour = Color::BLACK;
                    pSibling->colour = Color::BLACK;

                    current = current->parent->parent; // Move cursor up to grandparent

                    continue;
                }

                if (current == current->parent->leftChild)
                {
                    // CASE 2
                    // If left child, move up to parent and rotate right
                    current = current->parent;
                    RightRotate(current);
                }

                // CASE 3
                // Recolor parent and grandparent, push the structure right
                current->parent->colour = Color::BLACK;
                current->parent->parent->colour = Color::RED;
                LeftRotate(current->parent->parent);
            }
        }

        root->colour = Color::BLACK;
    }

    template <typename T, bool UniqueKeys>
    void RBTree<T, UniqueKeys>::DeleteRepair(Node* target)
    {
        Node* current = target;

        while (current != root && current->colour == Color::BLACK)
        {
            // When current is left child
            if (current == current->parent->leftChild)
            {
                Node* sibling = current->parent->rightChild;

                // CASE 1
                if (sibling->colour == Color::RED)
                {
                    sibling->colour = Color::BLACK;
                    current->parent->colour = Color::RED;

                    LeftRotate(current->parent);

                    sibling = current->parent->rightChild;
                }

                // CASE 2
                if (sibling->leftChild->colour == Color::BLACK && sibling->rightChild->colour == Color::BLACK)
                {
                    sibling->colour = Color::RED;
                    current = current->parent;
                }

                else
                {
                    // CASE 3
                    if (sibling->rightChild->colour == Color::BLACK)
                    {
                        sibling->leftChild->colour = Color::BLACK;
                        sibling->colour = Color::RED;

                        RightRotate(sibling);

                        sibling = current->parent->rightChild;
                    }

                    // CASE 4
                    sibling->colour = current->parent->colour;
                    sibling->rightChild->colour = Color::BLACK;

                    current->parent->colour = Color::BLACK;

                    LeftRotate(current->parent);

                    current = root;
                }
            }

            // When current is right child
            else
            {
                Node* sibling = current->parent->leftChild;

                // CASE 1
                if (sibling->colour == Color::RED)
                {
                    sibling->colour = Color::BLACK;
                    current->parent->colour = Color::RED;

                    RightRotate(current->parent);

                    sibling = current->parent->leftChild;
                }

                // CASE 2
                if (sibling->rightChild->colour == Color::BLACK && sibling->leftChild->colour == Color::BLACK)
                {
                    sibling->colour = Color::RED;
                    current = current->parent;
                }

                else
                {
                    // CASE 3
                    if (sibling->leftChild->colour == Color::BLACK)
                    {
                        sibling->rightChild->colour = Color::BLACK;
                        sibling->colour = Color::RED;

                        LeftRotate(sibling);

                        sibling = current->parent->leftChild;
                    }

                    // CASE 4
                    sibling->colour = current->parent->colour;
                    sibling->leftChild->colour = Color::BLACK;

                    current->parent->colour = Color::BLACK;

                    RightRotate(current->parent);

                    current = root;
                }
            }
        }

        current->colour = Color::BLACK;

        // Reset nilNode to blank state in case it was changed in deleteNode
        nilNode->parent = nullptr;
    }

}