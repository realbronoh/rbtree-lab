# RB-tree

## tree 구조체
- root 노드 포인터를 멤버로 가져 트리 전체를 가리킨다

```
typedef struct {
  node_t *root;
} rbtree;

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(sizeof(rbtree), 1);
  return p;
}
```

## node 구조체
- color(red, black), key, parent, left-child, right-child를 멤버로 갖는다

```
typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;
typedef int key_t;

typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;
```

## node insert 함수
1. red 색상을 갖는 새 노드 생성
2. 일반적인 BST(binary-search-tree) 규칙으로 먼저 노드를 추가(루트일 경우 black으로 변경)
3. red-red-conflict(새 노드와 그 부모가 모두 red일 경우, left_rotate(), right_rotate(), recolor() 등의 추가적인 조작 필요


```
node_t *new_RED_node(key_t key);
void recolor(node_t *curr);
void left_rotate(rbtree *t, node_t* x);
void right_rotate(rbtree *t, node_t* x);
void recolor_rotate(rbtree *t, node_t *new_node);

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
```

## node erase 함수
1. 일반적인 BST트리 노드 제거
2. 잘려나간 노드가 black 노드일 경우 fix-up 조작을 통해 rb트리의 특성 보존
3. double-black문제를 해결하기 위해 sibling노드의 색과 그 자식들의 색에 따라 case를 나눔
4. delete할때만 introduction to algorithms 책에 나온 NIL노드를 도입하고 delete완료 후 NIL노드를 트리에서 제거함으로서 마지막 노드들은 NULL포인터를 가리키도록 함

```
// 가상의 NIL 노드(전역변수)
node_t nil_node = {RBTREE_BLACK, 777, NULL, NULL, NULL};
node_t * NIL = &nil_node;

// 첫 3개의 함수는 insert함수 밑에 첨부해서 여기서는 따로 코드 추가하지 않겠습니다
void left_rotate(rbtree *t, node_t* x);
void right_rotate(rbtree *t, node_t* x);
void recolor(node_t *curr);
void rbtree_delete_fixup(rbtree *t, node_t *x);
node_t *rbtree_delete(rbtree *t, node_t *target);

int rbtree_erase(rbtree *t, node_t *p) {
  // exception case: no nodes

  node_t *target = rbtree_delete(t, p);
  free(target);
  return 0;

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
```

## 기타 함수들

### find 함수
1. 루트 노드부터 각 노드의 키 값과 대소비교하여 left-subtree, right-subtree로 갈 것인지 결정
2. 종료조건1: 찾는 값이 없는 경우 끝까지 탐색하여 NULL에 도달 후 경고문과 함께 NULL 반환
3. 종료조건2: target key를 가진 노드의 주소 반환

```
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
```

### min, max 함수
1. min의 경우 트리의 가장 왼쪽에 있는 노드 반환
2. max의 경우 트리의 가장 오른쪽에 있는 노드 반환

```
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
```

### tree print 함수
1. dfs로 트리 탐색(recursion)
2. left-subtree출력 -> 현재 노드 출력 -> right-subtree 출력 순서

```
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
```

### tree 전체 삭제 함수
1. 동적할당된 노드들과 트리 전체를 해제 후 삭제하는 함수
2. dfs로 탐색하면서(recursion) 노드를 빠져나갈 때, 해당 노드 free

```
int delete_rbtree_helper(node_t *curr)

void delete_rbtree(rbtree *t) {
  delete_rbtree_helper(t->root);
  free(t);
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
```
