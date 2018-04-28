template <typename T>
class Quadboost<T>{
public:
  Quadboost();
  bool contains(double keyX, keyY);
  bool insert(double keyX, double keyY, T value);
  bool remove(double keyX, double keyY, T value);
  bool move(double oldKeyX, double oldKeyY, double newKeyX, double NewKeyY);

private:
  //just a wrapper
  bool CAS(Node<T> *node, Node<T> *oldNode, Node<T> *newNode);
  void compress(Stack &path, Internal p);
  bool helpCompress(Compress op);
  bool check(Internal node);

  bool helpMove(Move op);

  bool findCommon(Node &il, Node &rl, Internal &lca, Operation &rOp, Operation &iOp,
  Stack &iPath, Stack &rPath, double oldKeyX, double newKeyX, double newKeyY, il, rl);
  bool continueFindCommon();

  void continueFind(Operation &pOp, Stack &path, Node &I, Internal p);
  void find(Node &l, Operation &pOp, Stack &path, double keyX, keyY);
  bool helpFlag(Internal node, Operation oldOp, Operation newOp);
  void helpSubstitute(Substitute op);
  void help(Operation op);
  bool moved(Node l);
  bool hasChild(Internal parent, Node oldChild);

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

}
