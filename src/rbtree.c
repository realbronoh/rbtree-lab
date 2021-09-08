#include "rbtree.h"

#include <malloc.h>
#include <stdio.h>


node_t nil_node = {RBTREE_BLACK, 777, NULL, NULL, NULL};
node_t * NIL = &nil_node;


////////////////////////////////////////////
// implicit declarations
void left_rotate(rbtree *t, node_t* x);
void right_rotate(rbtree *t, node_t* x);
void recolor(node_t *curr);
void recolor_rotate(rbtree *t, node_t *new_node);
node_t *new_RED_node(key_t key);
int insert_node_helper(rbtree *t, node_t *new_node, key_t key);
int print_rbtree(node_t *curr);
void rbtree_delete_fixup(rbtree *t, node_t *x);
int delete_rbtree_helper(node_t *curr);
node_t *rbtree_delete(rbtree *t, node_t *target);

/////////////////////////////////////////////

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(sizeof(rbtree), 1);
  return p;
}

void delete_rbtree(rbtree *t) {
  delete_rbtree_helper(t->root);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {

  // make new "RED"node with given key
  node_t *new_node = new_RED_node(key);

  // 일단 적절한 위치에 new_node 추가
  // empty tree인 경우 함수 탈출
  if (insert_node_helper(t, new_node, key)) return new_node;

  // non-empty cases below: 추가적인 조작 필요(recolor & rotation)
  node_t *curr = new_node;
  node_t *par_node = new_node->parent;

  // handle red-red confilct
  while (par_node->color != RBTREE_BLACK){

    // determine uncle node
    node_t *uncle_node = NULL;
    if (par_node->parent->left == par_node) uncle_node = par_node->parent->right;
    else uncle_node = par_node->parent->left;

    // case1: uncle is NULL or black
    if (uncle_node == NULL || uncle_node->color == RBTREE_BLACK){
      // do rotation and recolor
      recolor_rotate(t, curr);
      break;
    }
    // case2: uncle is red
    else{
      recolor(par_node);
      recolor(uncle_node);
      // 탈출조건
      if (par_node->parent == t->root) break;
      // 반복
      curr = par_node->parent;
      par_node = curr->parent;
      recolor(curr);
    }
  }
  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *curr = t->root;
  //move to chile node
  while (curr != NULL && curr->key != key){
    // search left-subtree
    if (key < curr->key) curr = curr->left;
    // search left-subtree
    else curr = curr->right;
  }
  // end of tree
  if (curr == NULL) printf("cannot find a node with given value %d !!\n", key);
  return curr;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *curr = t->root;
  // move to most-left-outside node
  while (curr != NULL && curr->left != NULL) curr = curr->left;
  return curr;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *curr = t->root;
  // move to most-right-outside node
  while (curr != NULL && curr->right != NULL) curr = curr->right;
  return curr;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // exception case: no nodes

  node_t *target = rbtree_delete(t, p);
  free(target);
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

/////////////////////////////////////////////////////
// my functions

void left_rotate(rbtree *t, node_t* x){
  //        |                 |
  //        x                 y
  //       / \               / \
  //      a   y             x   c
  //         / \           / \ 
  //        b   c         a   b
  
  // b를 x의 right-child로 이동
  node_t *y = x->right;   // y: x의 right-child
  x->right = y->left;
  // b가 NULL인 경우 제외 
  if (y->left != NULL) y->left->parent = x;
  // x의 부모 밑으로 y 연결
  y->parent = x->parent;
  
  if (t->root == x) t->root = y;   // x is root node
  else{     // x is not root node --> x의 기존 위치(left, right)에 y 추가
    if (x->parent->left == x) x->parent->left = y;
    else x->parent->right = y;
  }
  // x가 y의 left-child로 들어감
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t* x){
  //           |             |      
  //           x             y      
  //          / \           / \     
  //         y   c         a   x    
  //        / \               / \   
  //       a   b             b   c  
  
  // b를 x의 left-child로 이동
  node_t *y = x->left;   // y: x의 left-child
  x->left = y->right;
  // b가 NULL인 경우 제외
  if (y->right != NULL) y->right->parent = x;
  // x의 부모 밑으로 y 연결
  y->parent = x->parent;
  
  if (t->root == x) t->root = y;   // x is root node
  else{     // x is not root node --> x의 기존 위치(left, right)에 y 추가
    if (x->parent->left == x) x->parent->left = y;
    else x->parent->right = y;
  }
  // x가 y의 right-child로 들어감
  y->right = x;
  x->parent = y;
}

void recolor(node_t *curr){
  if (curr->color == RBTREE_BLACK) curr->color = RBTREE_RED;
  else curr->color = RBTREE_BLACK;
}

void recolor_rotate(rbtree *t, node_t *new_node){

  // 일단 grand parent node는 recolor 대상
  recolor(new_node->parent->parent);

  // LL, LR, RR, RL의 경우에 따라 회전, recolor을 진행한다
  node_t *par_node = new_node->parent;
  if (par_node->parent->left == par_node){
    // LL case
    if (par_node->left == new_node){
      recolor(par_node);
      right_rotate(t, par_node->parent);
    }
    // LR case
    else{
      recolor(new_node);
      left_rotate(t, par_node);
      right_rotate(t, new_node->parent);
    }
  }
  else{
    // RR case
    if (par_node->right == new_node){
      recolor(par_node);
      left_rotate(t, par_node->parent);
    }
    // RL case
    else{
      recolor(new_node);
      right_rotate(t, par_node);
      left_rotate(t, new_node->parent);
    }
  }
}

// make new node with red color
node_t *new_RED_node(key_t key){
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node->key = key;
  new_node->color = RBTREE_RED;
  new_node->parent = NULL;
  new_node->left = NULL;
  new_node->right = NULL;
  return new_node;
}

// 이진트리원칙에 의해 새 노드 추가
int insert_node_helper(rbtree *t, node_t *new_node, key_t key){
  // case1: empty tree 
  // recolor new node and make it as root node
  if (t->root == NULL){
    // recolor new node and make it as the root of tree
    recolor(new_node);
    t->root = new_node;
    return 1;
  }
  // insert할 위치 결정(binary tree search)
  node_t *curr = t->root;   // root부터 탐색 시작
  node_t *before = NULL;    // curr직전 노드 기록(이 노드 밑에 새 노드 추가)
  while (curr != NULL){
    before = curr;
    if (key < curr->key) curr = curr->left;
    else curr = curr->right;
  }
  // before노드에 new_node 추가
  new_node->parent = before;
  if (key < before->key) before->left = new_node;
  else before->right = new_node;
  return 0;
};


// print (key, color) of all nodes in RB-tree
int print_rbtree(node_t *curr){

  // base case
  if (curr == NULL) return 0;

  // recursive call: dfs
  print_rbtree(curr->left);
  printf("key: %d, color: ", curr->key);
  if (curr->color == RBTREE_BLACK) printf("black\n");
  else printf("red\n");
  print_rbtree(curr->right);
  return 0;
}

// target 노드의 주소값을 받아 트리에서 제거
node_t *rbtree_delete(rbtree *t, node_t *target){

    // exception
    if (target == NULL) return NULL;

    // 1. splice 대상 y 결정

    // target의 자식 노드가 0, 1개인 경우: splice 대상 y는 target 자신
    node_t *y = NULL;
    if (target->left == NULL || target->right == NULL) y = target;
    // target의 자식 노드가 2개인 경우: splice 대상 y는 target 노드의 successor 노드
    else{
      y = target->right;
      while (y->left != NULL) y = y->left;
    }

    // 2. y의 자식 노드 x 결정
    // y가 잘려나가면 그 밑의 노드를 이어주어야 하므로

    node_t *x = NULL;
    if (y->left != NULL) x = y->left;
    else x = y->right;

    // 3. splicing y
    // exception: y가 root일 경우
    if (y->parent == NULL) t->root = x;
    else{
      // exception: x가 NULL인 경우: 가상의 전역변수 NIL노드를 부여
      if (x == NULL) x = NIL;


      // y의 위치에 따라 부모-자식 관계 재형성(x와 y->parent)
      if (y == y->parent->left) y->parent->left = x;
      else y->parent->right = x;

      x->parent = y->parent;
    }

    // target의 자식이 2개인 경우: splicing 대상 y는 target이 아니라 target의 successor
    if (y != target){
      target->key = y->key;
      // target->color = y->color;
    }

    if (y->color == RBTREE_BLACK){
      rbtree_delete_fixup(t, x);
    }
    // else{
    //   // NIL 노드 해제
    //   if (x == NIL){
    //     printf("sdfsdfsdfn");
    //     if (x->parent->left == x) x->parent->left == NULL;
    //     else x->parent->right == NULL;
    //   }
    // }
    if (NIL->parent != NULL){
      if (NIL == NIL->parent->left) NIL->parent->left = NULL;
      else NIL->parent->right = NULL;

      NIL->parent = NULL;
    }
    return y;
}

void rbtree_delete_fixup(rbtree *t, node_t *x){
  // iterate while x is not root and x is black
  while (x != NULL && x->parent != NULL && x->color == RBTREE_BLACK){
    // sibling node (whivh is 'w' in textbook)
    node_t *sibling = NULL;
    // x is left child
    if (x == x->parent->left){
      sibling = x->parent->right;
      // case1: x(BB), w(R) --> p(x)는 자동 B 
      if(sibling->color == RBTREE_RED){
        sibling->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        sibling = x->parent->right;
      }
      // case2: x(BB), w(B), wl(B), wr(B)
      if ((sibling->left == NULL || sibling->left->color == RBTREE_BLACK) 
          && (sibling->right == NULL || sibling->right->color == RBTREE_BLACK)){
            sibling->color = RBTREE_RED;
            x = x->parent;
            // NIL NODE 제거 ///////////////////////
            if (x->left == NIL){
              x->left = NULL;
              NIL->parent = NULL;
            }
      }
      else {
        // case3: x(BB), w(B), wl(R), wr(B)
        if (sibling->right->color == RBTREE_BLACK){
          sibling->left->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          right_rotate(t, sibling);
          sibling = x->parent->right;
        }
        // case4: x(BB), w(B), wr(R)
        sibling->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        sibling->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        // NIL NODE 제거 ///////////////////////
        if (x->parent->left == NIL){
          x->parent->left = NULL;
          NIL->parent = NULL;
        }
        // terminate condition
        x->parent = NULL;
      }
    }
    /////////////////////////////////////////////////
    // x is right child
    else {
      sibling = x->parent->left;
      // case1: x(BB), w(R) --> p(x)는 자동 B 
      if(sibling->color == RBTREE_RED){
        sibling->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        sibling = x->parent->left;
      }
      // case2: x(BB), w(B), wl(B), wr(B)
      if ((sibling->left == NULL || sibling->left->color == RBTREE_BLACK) 
          && (sibling->right == NULL || sibling->right->color == RBTREE_BLACK)){
            sibling->color = RBTREE_RED;
            x = x->parent;
            // NIL NODE 제거 ///////////////////////
            if (x->right == NIL){
              x->right = NULL;
              NIL->parent = NULL;
            }
      }
      else {
        // case3: x(BB), w(B), wl(R), wr(B)
        if (sibling->left->color == RBTREE_BLACK){
          sibling->right->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          left_rotate(t, sibling);
          sibling = x->parent->left;
        }
        // case4: x(BB), w(B), wr(R)
        sibling->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        sibling->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        // NIL NODE 제거 ///////////////////////
        if (x->parent->right == NIL){
          x->parent->right = NULL;
          NIL->parent = NULL;
        }
        // terminate condition
        x->parent = NULL;
      }
    }
  } 
  if (x != NULL) x->color = RBTREE_BLACK;
}

int delete_rbtree_helper(node_t *curr){
  // base case
  if (curr == NULL) return 0;

  // recursive call: dfs
  delete_rbtree_helper(curr->left);
  delete_rbtree_helper(curr->right);
  // free memory escaping 'curr' node
  free(curr);
  return 0;
}