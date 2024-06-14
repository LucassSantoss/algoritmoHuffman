#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct HuffmanNode {
    char data;
    unsigned freq;
    struct HuffmanNode *left, *right;
} HuffmanNode;

typedef struct ListNode {
    HuffmanNode* huffNode;
    struct ListNode* next;
} ListNode;

#define MAX_CHAR 256
unsigned freqTable[MAX_CHAR];
char* huffmanCodes[MAX_CHAR];
ListNode* huffmanList = NULL; 
HuffmanNode* huffmanTree = NULL; 

// Adiciona a frequência de um caractere na tabela
void addFreq(char c, unsigned freq) {
    freqTable[c] = freq;
}

// Limpa a tabela de frequência e libera a memória dos códigos de Huffman
void clearFreq() {
    for (int i = 0; i < MAX_CHAR; i++) {
        freqTable[i] = 0;
    }
    
    for (int i = 0; i < MAX_CHAR; i++) {
        if (huffmanCodes[i]) {
            free(huffmanCodes[i]);
            huffmanCodes[i] = NULL;
        }
    }
}

// Cria um novo nó da árvore de Huffman
HuffmanNode* createHuffNode(char data, unsigned freq) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    node->data = data;
    node->freq = freq;
    node->left = node->right = NULL;
    return node;
}

// Cria um novo nó da lista encadeada
ListNode* createListNode(HuffmanNode* huffNode) {
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    node->huffNode = huffNode;
    node->next = NULL;
    return node;
}

// Insere um nó na lista encadeada em ordem crescente de frequência
ListNode* insertListNode(ListNode* head, ListNode* node) {
    if (!head || node->huffNode->freq < head->huffNode->freq 
    || (node->huffNode->freq == head->huffNode->freq && node->huffNode->data < head->huffNode->data)) {
        node->next = head;
        return node;
    }
    ListNode* current = head;
    
    while (current->next && (current->next->huffNode->freq < node->huffNode->freq 
    || (current->next->huffNode->freq == node->huffNode->freq && current->next->huffNode->data < node->huffNode->data))) {
        current = current->next;
    }

    node->next = current->next;
    current->next = node;
    return head;
}

// Cria uma lista encadeada de nós de Huffman a partir da tabela de frequência
ListNode* creatLinkedList() {
    ListNode* head = NULL;
    for (int i = 1; i < MAX_CHAR; i++) {
        if (freqTable[i]) {
            head = insertListNode(head, createListNode(createHuffNode((char)i, freqTable[i])));
        }
    }
    return head;
}

// Constrói a árvore de Huffman usando a lista encadeada
HuffmanNode* createHuffmanTree(ListNode* head) {
    while (head && head->next) {
        ListNode *leftNode = head;
        ListNode *rightNode = head->next;
        head = head->next->next;
        
        HuffmanNode* top = createHuffNode('_', leftNode->huffNode->freq + rightNode->huffNode->freq);
        top->left = leftNode->huffNode;
        top->right = rightNode->huffNode;
        
        head = insertListNode(head, createListNode(top));
        
        free(leftNode);
        free(rightNode);
    }
    HuffmanNode* root = head->huffNode;
    free(head);
    return root;
}

// Armazena os códigos de Huffman em um array
void storeCodes(HuffmanNode* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        storeCodes(root->left, arr, top + 1);
    }

    if (root->right) {
        arr[top] = 1;
        storeCodes(root->right, arr, top + 1);
    }

    if (!root->left && !root->right) {
        huffmanCodes[root->data] = (char*)malloc(top + 1);
        for (int i = 0; i < top; ++i) {
            huffmanCodes[root->data][i] = arr[i] + '0';
        }
        huffmanCodes[root->data][top] = '\0';
    }
}

// Calcula e armazena os códigos de Huffman
void calcHuffmanCodes() {
    huffmanList = creatLinkedList();
    huffmanTree = createHuffmanTree(huffmanList); 
    int arr[MAX_CHAR], top = 0;
    storeCodes(huffmanTree, arr, top);
}

// Imprime os códigos de Huffman
void printHuffmanCodes() {
    for (int i = 0; i < MAX_CHAR; i++) {
        if (huffmanCodes[i]) {
            printf("%c com frequencia %u codificado como %s\n", i, freqTable[i], huffmanCodes[i]);
        }
    }
}

// Codifica uma string usando os códigos de Huffman
void encodeString(char* str) {
    char encodedStr[1000] = {0};
    for (int i = 0; str[i]; i++) {
        strcat(encodedStr, huffmanCodes[(unsigned char)str[i]]);
    }

    printf("%s ", encodedStr);

    int originalBits = strlen(str) * 8;
    int encodedBits = strlen(encodedStr);

    int uniqueChars = 0;
    for (int i = 0; i < MAX_CHAR; i++) {
        if (freqTable[i]) uniqueChars++;
    }
    int minBits = uniqueChars > 1 ? (int)ceil(log2(uniqueChars)) : 1;
    int minimalBits = strlen(str) * minBits;

    printf("%.2f%% %.2f%%\n", ((float)encodedBits / originalBits) * 100, ((float)encodedBits / minimalBits) * 100);
}

// Decodifica uma sequência de bits usando a árvore de Huffman
void decodeBits(char* bits) {
    HuffmanNode* curr = huffmanTree;

    for (int i = 0; bits[i]; i++) {
        if (bits[i] == '0') {
            curr = curr->left;
        } else {
            curr = curr->right;
        }

        if (!curr->left && !curr->right) {
            printf("%c", curr->data);
            curr = huffmanTree;
        }
    }
    printf("\n");
}

int main() {
    char option[256];
    while (1) {
        printf("->");
        fgets(option, sizeof(option), stdin);
        
        int len_option = strlen(option);
        option[len_option - 1] = '\0';
       
        if (strncmp(option, "add", 3) == 0) {
            char c;
            unsigned freq;
            sscanf(option + 4, "%c %u", &c, &freq);
            addFreq(c, freq);
        } else if (strcmp(option, "calc") == 0) {
            calcHuffmanCodes();
        } else if (strcmp(option, "print") == 0) {
            printHuffmanCodes();
        } else if (strncmp(option, "cod", 3) == 0) {
            encodeString(option + 4);
        } else if (strncmp(option, "dec", 3) == 0) {
            decodeBits(option + 4);
        } else if (strcmp(option, "clear") == 0) {
            clearFreq();
        } else if (strcmp(option, "exit") == 0) {
            break;
        } else {
            printf("Opção inválida!\n");
        }
    }

    return 0;
}
