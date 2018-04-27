template <typename T>
class Quadboost<T>{
  class Node<T>{}
  class Internal<T>: Node<T>{
    double x, y, w, h;
    Node<T> nw, se, sw, se;
    Operation op;
  }
  class Leaf<T>: Node<T>{
    double keyX, keyY;
    T value;
    Move op;
  }
  class Empty<T> : Node<T>{}
  class Operation{}
  class Substitute : Operation{
    Internal parent;
    Node oldChild, newNode;
  }
  class Compress : Operation{
    Internal grandparent, parent;
  }
  class Move : Operation {
    Internal iParent, rParent;
    Node oldIChild, oldRChild, newIChild;
    Operation oldIOp, oldROp;
    bool allFlag;
  }
  class Clean : Operation{}
public:
  Quadboost();
  bool contains(double keyX, keyY);
  bool insert(double keyX, double keyY, )
}
