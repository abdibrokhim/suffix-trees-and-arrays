#include <iostream>
#include <algorithm>
#include <cstring>
#include <chrono>

using namespace std;
#define MAX_CHAR 256

struct SuffixTreeNode {
    struct SuffixTreeNode* children[MAX_CHAR];
    struct SuffixTreeNode* suffixLink;
    int start;
    int* end;
    int suffixIndex;
};

typedef struct SuffixTreeNode Node;

char text[100];
Node* root = NULL;
Node* lastNewNode = NULL;
Node* activeNode = NULL;
int activeEdge = -1;
int activeLength = 0;
int remainingSuffixCount = 0;
int leafEnd = -1;
int* rootEnd = NULL;
int* splitEnd = NULL;
int size = -1;

Node* newNode(int start, int* end) {
    Node* node = (Node*)malloc(sizeof(Node));
    for (int i = 0; i < MAX_CHAR; i++)
        node->children[i] = NULL;
    node->suffixLink = root;
    node->start = start;
    node->end = end;
    node->suffixIndex = -1;
    return node;
}

int edgeLength(Node* n) {
    if (n == root)
        return 0;
    return *(n->end) - (n->start) + 1;
}

int walkDown(Node* currNode) {
    if (activeLength >= edgeLength(currNode)) {
        activeEdge += edgeLength(currNode);
        activeLength -= edgeLength(currNode);
        activeNode = currNode;
        return 1;
    }
    return 0;
}

void extendSuffixTree(int pos) {
    leafEnd = pos;
    remainingSuffixCount++;
    lastNewNode = NULL;
    while (remainingSuffixCount > 0) {
        if (activeLength == 0)
            activeEdge = pos;
        if (activeNode->children[text[activeEdge]] == NULL) {
            activeNode->children[text[activeEdge]] = newNode(pos, &leafEnd);
            if (lastNewNode != NULL) {
                lastNewNode->suffixLink = activeNode;
                lastNewNode = NULL;
            }
        } else {
            Node* next = activeNode->children[text[activeEdge]];
            if (walkDown(next))
                continue;
            if (text[next->start + activeLength] == text[pos]) {
                if (lastNewNode != NULL && activeNode != root) {
                    lastNewNode->suffixLink = activeNode;
                    lastNewNode = NULL;
                }
                activeLength++;
                break;
            }
            splitEnd = (int*)malloc(sizeof(int));
            *splitEnd = next->start + activeLength - 1;
            Node* split = newNode(next->start, splitEnd);
            activeNode->children[text[activeEdge]] = split;
            split->children[text[pos]] = newNode(pos, &leafEnd);
            next->start += activeLength;
            split->children[text[next->start]] = next;
            if (lastNewNode != NULL) {
                lastNewNode->suffixLink = split;
            }
            lastNewNode = split;
        }
        remainingSuffixCount--;
        if (activeNode == root && activeLength > 0) {
            activeLength--;
            activeEdge = pos - remainingSuffixCount + 1;
        } else if (activeNode != root) {
            activeNode = activeNode->suffixLink;
        }
    }
}

void setSuffixIndexByDFS(Node* n, int labelHeight) {
    if (n == NULL)
        return;
    if (n->start != -1) {
        // Uncomment the line below to print the suffix tree
        // print(n->start, *(n->end));
    }
    int leaf = 1;
    for (int i = 0; i < MAX_CHAR; i++) {
        if (n->children[i] != NULL) {
            leaf = 0;
            setSuffixIndexByDFS(n->children[i], labelHeight +
            edgeLength(n->children[i]));
        }
    }
    if (leaf == 1) {
        n->suffixIndex = size - labelHeight;
        // Uncomment the line below to print the suffix index
        // printf(" [%d]\n", n->suffixIndex);
    }
}

void freeSuffixTreeByPostOrder(Node* n) {
    if (n == NULL)
        return;
    for (int i = 0; i < MAX_CHAR; i++) {
        if (n->children[i] != NULL) {
            freeSuffixTreeByPostOrder(n->children[i]);
        }
    }
    if (n->suffixIndex == -1)
        free(n->end);
    free(n);
}

void buildSuffixTree() {
    size = strlen(text);
    rootEnd = (int*)malloc(sizeof(int));
    *rootEnd = -1;
    root = newNode(-1, rootEnd);
    activeNode = root;
    for (int i = 0; i < size; i++)
        extendSuffixTree(i);
    int labelHeight = 0;
    setSuffixIndexByDFS(root, labelHeight);
}

int traverseEdge(char* str, int idx, int start, int end) {
    int k = 0;
    for (k = start; k <= end && str[idx] != '\0'; k++, idx++) {
        if (text[k] != str[idx])
            return -1;
    }
    if (str[idx] == '\0')
        return 1;
    return 0;
}

int doTraversal(Node* n, char* str, int idx) {
    if (n == NULL) {
        return -1;
    }
    int res = -1;
    if (n->start != -1) {
        res = traverseEdge(str, idx, n->start, *(n->end));
        if (res != 0)
            return res;
    }
    idx = idx + edgeLength(n);
    if (n->children[str[idx]] != NULL)
        return doTraversal(n->children[str[idx]], str, idx);
    else
        return -1;
}

void checkForSubstring(char* str) {
    int res = doTraversal(root, str, 0);
    if (res == 1)
        printf("Pattern <%s> is a Substring\n", str);
    else
        printf("Pattern <%s> is NOT a Substring\n", str);
}

int main(int argc, char* argv[]) {
    strcpy(text, "THIS IS A TEST TEXT$");

    auto startConstruction = chrono::high_resolution_clock::now();
    buildSuffixTree();
    auto endConstruction = chrono::high_resolution_clock::now();
    auto constructionTime = chrono::duration_cast<chrono::microseconds>
    (endConstruction - startConstruction);

    auto startSearch = chrono::high_resolution_clock::now();
    checkForSubstring("TEST");
    auto endSearch = chrono::high_resolution_clock::now();
    auto searchTime = chrono::duration_cast<chrono::microseconds>
    (endSearch - startSearch);

    printf("Construction Time: %lld microseconds\n", constructionTime.count());
    printf("Substring Search Time: %lld microseconds\n", searchTime.count());

    freeSuffixTreeByPostOrder(root);

    return 0;
}