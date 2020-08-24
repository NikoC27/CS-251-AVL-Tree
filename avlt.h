/*avlt.h*/

// Created by Niko Castellana
// Date: 3/14/2020
// Project # 5 
// Threaded AVL tree
//

#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <cmath>

using namespace std;

template<typename KeyT, typename ValueT>
class avlt
{
private:
  struct NODE
  {
    KeyT   Key;
    ValueT Value;
    NODE*  Left;
    NODE*  Right;
    bool   isThreaded; // true => Right is a thread, false => non-threaded
    int    Height;     // height of tree rooted at this node
  };

  NODE* Root;  // pointer to root node of tree (nullptr if empty)
  NODE* Current; // pointer to current node for begin and next functions
  int   Size;  // # of nodes in the tree (0 if empty)
  
  
	/* Traverse through the tree using 
	 * postorder and free the nodes */
	void _clear(NODE* cur)
	{
		if(cur == nullptr) // Tree is empty or tree 
			return;        // has ended so return
		else
		{
			_clear(cur->Left); // Go Left
			
			/* Go Right when it's not threaded */
			if(!cur->isThreaded) 
			{
				_clear(cur->Right);
			}
			
			delete cur; // Free node
		}
	}
	
	
	/* Traverse through the tree using preorder 
	 * and copy the nodes from the original tree */
	void _copy(const NODE* cur)
	{
		if(cur == nullptr) // Tree is empty or tree
			return;        // has ended so return
		else
		{
			/* Insert key and value into the node */
			_insert(cur->Key, cur->Value);
			_copy(cur->Left); // Go Left
			
			/* Go Right when it's not threaded */	
			if(!cur->isThreaded)
			{
				_copy(cur->Right);
			}
			
		}
	}
	
	
	/* Traverse through the tree using 
	 * inorder and print the nodes */
	void _print(NODE* cur, ostream& output)const
	{
		if(cur == nullptr) // Tree is empty or tree
			return;        // has ended so return
		else
		{
			_print(cur->Left, output); // Go Left
			
			/* Print (Key, Value, Height, Thread) when node is threaded
			 * and there is a node right of the current node.*/
			if(cur->isThreaded && cur->Right != nullptr)
			{
				output << "(" << cur->Key << "," << cur->Value << ","
				<< cur->Height << "," << cur->Right->Key << ")"<< endl;
			}
			else
			{
				/* Print (Key, Value, Height) when node is not 
				 * threaded and go right to the next node */
				output << "(" << cur->Key << "," << cur->Value
				       << "," << cur->Height<< ")"<< endl;
				_print(cur->Right, output); // Go Right
			}
				
		}
	}
	
	
	/* This function rotates the tree
	 * to the left when it's broken
	 * and updates the height */
	void _LeftRotate(NODE* Parent, NODE* N)
	{
		NODE* A = N->Left;        // 100   N      
		NODE* R = N->Right;       //  
		NODE* B = R->Left;        //     110   R
		NODE* C = R->Right;       // 
		                          //         120   C
    
		// Make left rotation:
	    R->Left = N;
		N->Right = B;
		
		// Rethread after rotation:
		if(B == nullptr)
		{
			N->Right = R;
			N->isThreaded = true;
		}
		
		// Check if N has a parent: 
		if(Parent == nullptr)
		{
			Root = R;  // No parent, so R becomes root
		}
		else if(Parent->Left == N)
		{
			Parent->Left = R;  // Relink Parent's left pointer
		}
		else
		{
			Parent->Right = R;  // Relink Parent's right pointer
		}
     
		int HA;  // Height of A
		int HB;  // Height of B 
		int HC;  // Height of C
     
		if(A == nullptr)  // No node, make height -1:
		{
			HA = -1;
		}
		else  // Node exists, get height:
		{
			HA = A->Height;
		}
     
		if(B == nullptr)  // No node, make height -1:
		{ 
			HB = -1;
		}
		else  // Node exists, get height:
		{
			HB = B->Height;
		}
     
		if(C == nullptr)  // No node, make height -1:
		{
			HC = -1;
		}
		else  // Node exists, get height:
		{
			HC = C->Height;
		}
     
		 N->Height = 1 + max(HA, HB);        // Update height of N
		 R->Height = 1 + max(HC, N->Height); // Update height of R
     }
	 
	 
	 /* This function rotates the tree
	  * to the right when it's broken
	  * and updates the height */
	 void _RightRotate(NODE* Parent, NODE* N)
	 {
		 NODE* L = N->Left;        //            3   N      
		 NODE* A = L->Left;        //
		 NODE* B = L->Right;       //        2    L
		 NODE* C = N->Right;       //
                                   //    1     A
		                           //
		 
		 // Unlink threading for rotation:
	     if(L->isThreaded)
		 {
			B = nullptr;
		 }
		 if(N->isThreaded)
		 {
			C = nullptr;
		 }
		 
		 // Make right rotation and unthread:
		 L->Right = N;
		 L->isThreaded = false;
		 N->Left = B;
		 
		 // Check if N has a parent:
		 if(Parent == nullptr)
		 {
			 Root = L;  // No parent, so L becomes root
		 }
		 else if(Parent->Left == N)
		 {
			 Parent->Left = L;  // Relink Parent's left pointer
         }
		 else 
		 {
			 Parent->Right = L;  // Relink Parent's right pointer
		 }
		 
		 int HA;  // Height of A
		 int HB;  // Height of B
		 int HC;  // Height of C
      
		 if(A == nullptr)  // No node, make height -1:
		 {
			HA = -1; 
		 }
		 else  // Node exists, get height:
		 {
			HA = A->Height;
		 }
      
		 if(B == nullptr)  // No node, make height -1:
		 {
			HB = -1; 
		 }
		 else  // Node exists, get height:
		 {
			HB = B->Height;
		 }
      
		 if(C == nullptr)  // No node, make height -1:
		 {
			HC = -1; 
		 }
		 else  // Node exists, get height:
		 {
			HC = C->Height;
		 }
      
		 N->Height = 1 + max(HB, HC);        // Update height of N
		 L->Height = 1 + max(HA, N->Height); // Update height of L
	  }
	
	
	/* Returns a pointer to 
	 * the leftmost node */
	NODE* _begin(NODE* cur)
	{
		/* Check for empty tree */
		if(cur == nullptr)
		{
			return nullptr;
		}
		else
		{
			/* Loop until the leftmost node */
			while(cur->Left != nullptr)
			{
				cur = cur->Left; // Go Left
			}
		}
		
		return cur; // Return current node
	}

	void _insert(KeyT key, ValueT value)
	{
		NODE* prev = nullptr; // Previous Node
		NODE* cur = Root; // Current Node
		stack<NODE*> nodes; // Declare stack of nodes to check heights

		/* Search to see if tree already contains key */
		while (cur != nullptr)
		{
			if (key == cur->Key)  // Key already in tree
				return;
		
			nodes.push(cur); // stack so we can return later
		
			if (key < cur->Key)  // Search left
			{
				prev = cur;
				cur = cur->Left;
			}
			else // Search right
			{
				prev = cur;
				if(cur->isThreaded)
				{
					cur = nullptr;
				}
				else // Not Threaded
				{   
					cur = cur->Right; // Move right
				}
			}
		}//while

		/* Key is not in tree, so a new 
		* node is allocated to insert */
		NODE* newNode = new NODE();
	    newNode->Key = key;
	    newNode->Value = value;
        newNode->Left = nullptr;
        newNode->Right = nullptr;
	    newNode->isThreaded = true;
	    newNode->Height = 0;
	  
		//
		// NOTE: cur is null, and prev denotes node where
        // we fell out of the tree.  if prev is null, then
        // the tree is empty and the Root pointer needs 
        // to be updated.
        //

		if(prev == nullptr)
			Root = newNode;
		else if (key < prev->Key)
		{
			prev->Left = newNode;  // Insert new node to the left of the previous
			newNode->Right = prev; // Point new node's right pointer 
								   // to the node on the right
		}
		else
		{   
			newNode->Right = prev->Right;
			prev->isThreaded = false; 
			prev->Right = newNode;        // Insert new node to the right of the previous
		}
         
		Size++; // Update Size
	  
		// Walk back up tree using stack and update heights:
		while (!nodes.empty())
		{
			cur = nodes.top(); // Make current top of stack
		    nodes.pop();
		 
			int hL = (cur->Left == nullptr) ? -1 : cur->Left->Height;
			int hR = (cur->Right == nullptr || cur->isThreaded) ? -1 : cur->Right->Height; // check for threading
			int hCur = 1 + max(hL, hR);

			if (cur->Height == hCur)  // didn't change, so no need to go further:
				break;
			else   // height changed, update and keep going:
				cur->Height = hCur;
		}

	}

public:
  //
  // default constructor:
  //
  // Creates an empty tree.
  //
  avlt()
  {
    Root = nullptr;
    Size = 0;
  }

  //
  // copy constructor
  //
  // NOTE: makes an exact copy of the "other" tree, such that making the
  // copy requires no rotations.
  //
  avlt (const avlt& other)
  {
    Root = nullptr; 
	clear();
	_copy(other.Root);
	this->Size = other.Size;
  }

	//
  // destructor:
  //
  // Called automatically by system when tree is about to be destroyed;
  // this is our last chance to free any resources / memory used by
  // this tree.
  //
  virtual ~avlt()
  {
    clear();
  }

  //
  // operator=
  //
  // Clears "this" tree and then makes a copy of the "other" tree.
  //
  // NOTE: makes an exact copy of the "other" tree, such that making the
  // copy requires no rotations.
  //
  avlt& operator=(const avlt& other)
  {
    clear();
	_copy(other.Root);
	 
    return *this;
  }

  //
  // clear:
  //
  // Clears the contents of the tree, resetting the tree to empty.
  //
  void clear()
  {
    _clear(Root);
	Root = nullptr;
	Size = 0;
  }

  // 
  // size:
  //
  // Returns the # of nodes in the tree, 0 if empty.
  //
  // Time complexity:  O(1) 
  //
  int size() const
  {
    return Size;
  }

  // 
  // height:
  //
  // Returns the height of the tree, -1 if empty.
  //
  // Time complexity:  O(1) 
  //
  int height() const
  {
    if (Root == nullptr) // Empty tree:
      return -1;
    else 
      return Root->Height; // Tree Height
  }

  // 
  // search:
  //
  // Searches the tree for the given key, returning true if found
  // and false if not.  If the key is found, the corresponding value
  // is returned via the reference parameter.
  //
  // Time complexity:  O(lgN) worst-case
  //
  bool search(KeyT key, ValueT& value) const
  {
    NODE* cur = Root; // Current Node

	/* Loop through the tree */
    while (cur != nullptr)
	{
      if (key == cur->Key) 
	  {
		value = cur->Value; // Update value
		return true; // key and value pair found
	  }
		
	  /* Check if key is less than current key */
      if (key < cur->Key)  
      {
        cur = cur->Left; // Move left
      }
	  /* Check if key is greater than current key */
      else
      {
		if(cur->isThreaded)
		{
			cur = nullptr;
		}
		else
		{
			cur = cur->Right; // Move Right
		}
		 
      }
	} 
  
    // key and value pair not found
    return false;
  }

  //
  // range_search
  //
  // Searches the tree for all keys in the range [lower..upper], inclusive.
  // It is assumed that lower <= upper.  The keys are returned in a vector;
  // if no keys are found, then the returned vector is empty.
  //
  // Time complexity: O(lgN + M), where M is the # of keys in the range
  // [lower..upper], inclusive.
  //
  // NOTE: do not simply traverse the entire tree and select the keys
  // that fall within the range.  That would be O(N), and thus invalid.
  // Be smarter, you have the technology.
  //
  vector<KeyT> range_search(KeyT lower, KeyT upper)
  {
    vector<KeyT>  keys;    // Vector of keys
	NODE* cur = Root;      // Current Node
	NODE* prev = nullptr;  // Previous Node
	
	if(lower > upper)  // Invalid bounds return default:
	{
		return keys;
	}
	
	// Searches for the lower key in the tree:
	while(cur != nullptr)
	{
 		prev = cur;  // Update previous node
		
		if(cur->Key == lower)  // lower found:
			break;
		
		if(lower < cur->Key)  // Lower is to the left:
		{
			cur = cur->Left;  
		}
		else  // Lower is to the right:
		{
			if(cur->isThreaded)
				break;
			else
				cur = cur->Right;
		}
			
	} // end while
	
	// Loop until upper is found:
	while(prev != nullptr && prev->Key <= upper)
	{
		// Key in range, push to vector:
		if(lower <= prev->Key)  
			keys.push_back(prev->Key);
		
		if(prev->isThreaded)
			prev = prev->Right;
		else  
			// Get to the right node and retrieve leftmost node:
			prev = _begin(prev->Right);

	}

	return keys;
  }

  //
  // insert
  //
  // Inserts the given key into the tree; if the key has already been insert then
  // the function returns without changing the tree.  Rotations are performed
  // as necessary to keep the tree balanced according to AVL definition.
  //
  // Time complexity:  O(lgN) worst-case
  //
  void insert(KeyT key, ValueT value)
  {
    NODE* prev = nullptr; // Previous Node
    NODE* cur = Root; // Current Node
	stack<NODE*> nodes; // Declare stack of nodes to check heights

    /* Search to see if tree already contains key */
    while (cur != nullptr)
    {
      if (key == cur->Key)  // Key already in tree
        return;
		
	  nodes.push(cur); // stack so we can return later
		
      if (key < cur->Key)  // Search left
      {
        prev = cur;
        cur = cur->Left;
      }
      else // Search right
      {
		  prev = cur;
		  if(cur->isThreaded)
		  {
			  cur = nullptr;
		  }
		  else // Not Threaded
		  {   
			  cur = cur->Right; // Move right
		  }
       }
    }//while

	/* Key is not in tree, so a new 
	 * node is allocated to insert */
      NODE* newNode = new NODE();
	  newNode->Key = key;
	  newNode->Value = value;
      newNode->Left = nullptr;
      newNode->Right = nullptr;
	  newNode->isThreaded = true;
	  newNode->Height = 0;
	  
    //
    // NOTE: cur is null, and prev denotes node where
    // we fell out of the tree.  if prev is null, then
    // the tree is empty and the Root pointer needs 
    // to be updated.
    //

      if(prev == nullptr)
         Root = newNode;
      else if (key < prev->Key)
	  {
		  prev->Left = newNode; // Insert new node to the left of the previous
		  newNode->Right = prev; // Point new node's right pointer 
								 // to the node on the right
	  }
      else
	  {   
		  newNode->Right = prev->Right;
		  prev->isThreaded = false; 
		  prev->Right = newNode; // Insert new node to the right of the previous
	  }
         
      Size++; // Update Size
	  
	 // Walk back up tree using stack and update heights:
	  while (!nodes.empty())
      {
		 NODE* parent;      // Parent of the current node
		 cur = nodes.top(); // Make current top of stack
		 nodes.pop();       // Pop the stack
		 
		 if(nodes.empty()) // Stack empty no parent:
		 {
			 parent = nullptr;
		 }
		 else // Parent found:
		 {
			 parent = nodes.top();
		 }
		 
		 /* Get the left and right heights and of the node and calculate total height */
		 int hL = (cur->Left == nullptr) ? -1 : cur->Left->Height;
		 int hR = (cur->Right == nullptr || cur->isThreaded) ? -1 : cur->Right->Height; 
		 int hCur = 1 + max(hL, hR);

		 if (cur->Height == hCur)  // didn't change, so no need to go further:
			break;
		 else if(abs(hL - hR) <= 1)  // height changed, update and keep going:
			cur->Height = hCur;
		 else  // Height is broken:
         {
			 if(hL > hR)  // Check if cur->Left is leaning:
			 {
				 NODE* L =  cur->Left;  // Get left node of current

				 /* Get the left and right heights of L */
				 int hLL = (L->Left == nullptr) ? -1 : L->Left->Height;
		         int hLR = (L->Right == nullptr || L->isThreaded) ? -1 : L->Right->Height; // check for threading
			
				 if(hLL > hLR) // Case 1
				 {
					_RightRotate(parent,cur);
				 }
				 else // Case 2
				 {
					_LeftRotate(cur,cur->Left);  // Rotate left
					_RightRotate(parent,cur);    // Rotate right
				 }
			 }
			 else if(hR > hL) // Check if cur->Right is leaning:
			 {
				 NODE* R = cur->Right;  // Get right node of current
		
				 /* Get the left and right heights of L */ 
				 int hRR = (R->Right == nullptr || R->isThreaded) ? -1 : R->Right->Height;
		         int hRL = (R->Left == nullptr) ? -1 : R->Left->Height; 
				 
				 if(hRR > hRL) // Case 4
				 {
					 _LeftRotate(parent,cur);  // Rotate left
				 }
				 else // Case 3
				 {
					 _RightRotate(cur,cur->Right);  // Rotate right
					 _LeftRotate(parent,cur);       // Rotate left
				 }
			 }  // end of else if
			
         }  // end of else
			 
      }  // end while
		 
  }  // end of insert
	   
	
  //
  // []
  //
  // Returns the value for the given key; if the key is not found,
  // the default value ValueT{} is returned.
  //
  // Time complexity:  O(lgN) worst-case
  //
  ValueT operator[](KeyT key) const
  {
    ValueT value; // Declare local value to call search and update
	  
    if(search(key,value))  // Key found, return value:
	{
		return value; 
	}
	else  // Key not found, return default:
	{
		return ValueT{ };
	}
  }

  //
  // 
  //
  // Finds the key in the tree, and returns the key to the "right".
  // If the right is threaded, this will be the next inorder key.
  // if the right is not threaded, it will be the key of whatever
  // node is immediately to the right.
  //
  // If no such key exists, or there is no key to the "right", the
  // default key value KeyT{} is returned.
  //
  // Time complexity:  O(lgN) worst-case
  //
  KeyT operator()(KeyT key) const
  {
    NODE* cur = Root;
	
	/* Search through tree */
	while(cur != nullptr)
	{
		/* Key found, return right node's key */
		if(key == cur->Key && cur->Right != nullptr)
		{
			return cur->Right->Key;
		}
		
		if(key < cur->Key)
		{
			cur = cur->Left; // Go Left
		}
		else
		{
			if (cur->isThreaded)
			{
				cur = nullptr;
			}
			else
			{
				cur = cur->Right; // Go Right
			}
			
		}
	}

    return KeyT{ };
  }

  //
  // %
  //
  // Returns the height stored in the node that contains key; if key is
  // not found, -1 is returned.
  //
  // Example:  cout << tree%12345 << endl;
  //
  // Time complexity:  O(lgN) worst-case
  //
  int operator%(KeyT key) const
  {
	NODE* cur = Root;
	
	/* Loop through the tree */
	while (cur != nullptr)
	{
      if (key == cur->Key) 
	  {
		return cur->Height; // Key found return height
	  }
		
	  /* Check if key is less than current key */
      if (key < cur->Key)  
      {
        cur = cur->Left; // Move left
      }
	  /* Check if key is greater than current key */
      else
      {
		if(cur->isThreaded)
		{
			cur = nullptr;
		}
		else
		{
			cur = cur->Right; // Move Right
		}
		 
      }
	} 

    return -1;
  }

  //
  // begin
  //
  // Resets internal state for an inorder traversal.  After the 
  // call to begin(), the internal state denotes the first inorder
  // key; this ensure that first call to next() function returns
  // the first inorder key.
  //
  // Space complexity: O(1)
  // Time complexity:  O(lgN) worst-case
  //
  // Example usage:
  //    tree.begin();
  //    while (tree.next(key))
  //      cout << key << endl;
  //
  void begin()
  {
    Current = _begin(Root); // Initialize Current
							// to leftmost node
  }

  //
  // next
  //
  // Uses the internal state to return the next inorder key, and 
  // then advances the internal state in anticipation of future
  // calls.  If a key is in fact returned (via the reference 
  // parameter), true is also returned.
  //
  // False is returned when the internal state has reached null,
  // meaning no more keys are available.  This is the end of the
  // inorder traversal.
  //
  // Space complexity: O(1)
  // Time complexity:  O(lgN) worst-case
  //
  // Example usage:
  //    tree.begin();
  //    while (tree.next(key))
  //      cout << key << endl;
  //
  bool next(KeyT& key)
  {
    if(Current != nullptr)
	{
	    if(Current->isThreaded)
		{
			key = Current->Key; // Update key
			Current = Current->Right; // Go right
			return true;
		}
		
	    if(!Current->isThreaded)
		{
			key = Current->Key; // Update key
			
			/* Get leftmost node of the
			 * current node's right node */
			Current = _begin(Current->Right);
			return true;
		}
	}
	 
	 return false;
  }

  //
  // dump
  // 
  // Dumps the contents of the tree to the output stream, using a
  // recursive inorder traversal.
  //
  void dump(ostream& output) const
  {
    output << "**************************************************" << endl;
    output << "********************* AVLT ***********************" << endl;

    output << "** size: " << this->size() << endl;
    output << "** height: " << this->height() << endl;

    //
    // inorder traversal, with one output per line: either 
    // (key,value,height) or (key,value,height,THREAD)
    //
    // (key,value,height) if the node is not threaded OR thread==nullptr
    // (key,value,height,THREAD) if the node is threaded and THREAD denotes the next inorder key
    //

    _print(Root, output);

    output << "**************************************************" << endl;
  }
	
};
