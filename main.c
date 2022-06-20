#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>

// Kruskal's algorithm implemetation in c
// reference: https://www.programiz.com/dsa/kruskal-algorithm

typedef struct edge
{
    int v;
    int w;
    int weight;
} Edge;

typedef struct graph
{
    Edge *edges;
    size_t size;
    size_t capacity;
} Graph;

typedef struct minimum_spanning_tree
{
    Edge *edges;
    size_t size;
    size_t capacity;
    int sum_of_weights;
} MinimumSpanningTree;

typedef struct Kruskal_data
{
    Graph *graph;
    MinimumSpanningTree *mst;

    int *parent;
    int num_vertices;
} Kruskal;

void Kruskal_init(Kruskal *kruskal, int num_vertices)
{
    kruskal->graph = malloc(sizeof(Graph));
    kruskal->mst = malloc(sizeof(MinimumSpanningTree));

    kruskal->graph->edges = malloc(sizeof(Edge) * num_vertices);
    kruskal->graph->size = 0;
    kruskal->graph->capacity = num_vertices;

    kruskal->mst->edges = malloc(sizeof(Edge) * num_vertices);
    kruskal->mst->size = 0;
    kruskal->mst->capacity = num_vertices;
    kruskal->mst->sum_of_weights = 0;

    kruskal->parent = malloc(sizeof(int) * num_vertices);
    kruskal->num_vertices = num_vertices;

    for (int i = 0; i < num_vertices; i++)
    {
        kruskal->parent[i] = i;
    }
}

void Kruskal_destroy(Kruskal *kruskal)
{
    free(kruskal->graph->edges);
    free(kruskal->graph);

    free(kruskal->mst->edges);
    free(kruskal->mst);

    free(kruskal->parent);
}

void Kruskal_add_edge(Kruskal *kruskal, int v, int w, int weight)
{
    if (kruskal->graph->size == kruskal->graph->capacity)
    {
        kruskal->graph->capacity *= 2 + 1;
        kruskal->graph->edges = realloc(kruskal->graph->edges, sizeof(Edge) * kruskal->graph->capacity);
    }

    kruskal->graph->edges[kruskal->graph->size].v = v;
    kruskal->graph->edges[kruskal->graph->size].w = w;
    kruskal->graph->edges[kruskal->graph->size].weight = weight;

    kruskal->graph->size++;
}

void Kruskal_add_to_tree(Kruskal *kruskal, int index)
{
    if (kruskal->mst->size == kruskal->mst->capacity)
    {
        kruskal->mst->capacity *= 2 + 1;
        kruskal->mst->edges = realloc(kruskal->mst->edges, sizeof(Edge) * kruskal->mst->capacity);
    }

    kruskal->mst->edges[kruskal->mst->size] = kruskal->graph->edges[index];
    kruskal->mst->size++;

    kruskal->mst->sum_of_weights += kruskal->graph->edges[index].weight;
}

int find_parent(Kruskal *kruskal, int v)
{
    if (kruskal->parent[v] == v)
    {
        return v;
    }

    return find_parent(kruskal, kruskal->parent[v]);
    // find parent recursively
}

void Kruskal_union(Kruskal *kruskal, int v, int w)
{
    kruskal->parent[v] = w;
}

int cmp(const void *a, const void *b)
{
    Edge *e1 = (Edge *)a;
    Edge *e2 = (Edge *)b;

    return e1->weight - e2->weight;
}

void Kruskal_print_mst(Kruskal *kruskal)
{
    printf("Minimum Spanning Tree:\n");
    printf("vertex_1 vertex_2 weight:\n");
    for (int i = 0; i < kruskal->mst->size; i++)
    {
        printf("%d %d %d\n", kruskal->mst->edges[i].v, kruskal->mst->edges[i].w, kruskal->mst->edges[i].weight);
    }

    printf("\n");
    printf("total weight of MST: %d\n", kruskal->mst->sum_of_weights);
    printf("\n");
}

void Kruskal_algorithm(Kruskal *kruskal)
{
    // sort edges by weight
    qsort(kruskal->graph->edges, kruskal->graph->size, sizeof(Edge), cmp);

    for (int i = 0; (i < kruskal->graph->size) && (kruskal->mst->size < kruskal->num_vertices - 1); i++)
    {
        int v = kruskal->graph->edges[i].v;
        int w = kruskal->graph->edges[i].w;
        int weight = kruskal->graph->edges[i].weight;

        int v_parent = find_parent(kruskal, v);
        int w_parent = find_parent(kruskal, w);

        if (v_parent != w_parent)
        {
            Kruskal_add_to_tree(kruskal, i);
            Kruskal_union(kruskal, v_parent, w_parent);
        }
    }
}

void generateRandConnectedGraph(Kruskal *kruskal, int num_vertices, int num_edges)
{
    if (num_edges < num_vertices - 1)
    {
        printf("The number of edges must be greater than the number of vertices - 1\n");
        exit(1);
    }

    int *rand_array = (int *)malloc(num_edges * sizeof(int));
    int count = 0;

    rand_array[0] = 0;

    for (int i = 1; i < num_vertices; i++)
    {
        rand_array[i] = i;
        int rand_index = rand() % i;

        int temp = rand_array[i];
        rand_array[i] = rand_array[rand_index];
        rand_array[rand_index] = temp;
    }

    for (int i = num_vertices; i < num_edges + 1; i++)
    {
        rand_array[i] = rand() % num_vertices;
        int rand_index = rand() % i;

        int temp = rand_array[rand_index];
        rand_array[rand_index] = rand_array[i];
        rand_array[i] = temp;
    }

    while (count < num_edges)
    {
        int u = rand_array[count];
        int v = rand_array[count + 1];
        int weight = rand() % 21 - 10;
        Kruskal_add_edge(kruskal, u, v, weight);
        count += 1;
    }

    free(rand_array);

    return;
}

void benchmark(int num_of_vertices, int num_of_edges)
{
    Kruskal kruskal;
    Kruskal_init(&kruskal, num_of_vertices);

    generateRandConnectedGraph(&kruskal, num_of_vertices, num_of_edges);

    clock_t start = clock();
    Kruskal_algorithm(&kruskal);
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("The time spent on Kruskal's algorithm is: %f\n", time_spent);
    printf("vertices: %d, edges: %d\n", num_of_vertices, num_of_edges);
    printf("Total weight of the mst is %d\n", kruskal.mst->sum_of_weights);

    Kruskal_destroy(&kruskal);
}

int main()
{
    srand(time(NULL));

    Kruskal kruskal;
    Kruskal_init(&kruskal, 10);

    Kruskal_add_edge(&kruskal, 0, 1, 4);
    Kruskal_add_edge(&kruskal, 0, 7, 8);
    Kruskal_add_edge(&kruskal, 1, 2, 8);
    Kruskal_add_edge(&kruskal, 1, 7, 11);
    Kruskal_add_edge(&kruskal, 2, 3, 7);
    Kruskal_add_edge(&kruskal, 2, 8, 2);
    Kruskal_add_edge(&kruskal, 2, 5, 4);
    Kruskal_add_edge(&kruskal, 3, 4, 9);
    Kruskal_add_edge(&kruskal, 3, 5, 14);
    Kruskal_add_edge(&kruskal, 4, 5, 10);
    Kruskal_add_edge(&kruskal, 5, 6, 2);
    Kruskal_add_edge(&kruskal, 6, 7, 1);
    Kruskal_add_edge(&kruskal, 6, 8, 6);
    Kruskal_add_edge(&kruskal, 7, 8, 7);
    Kruskal_add_edge(&kruskal, 7, 3, 2);
    Kruskal_add_edge(&kruskal, 8, 9, 4);
    Kruskal_add_edge(&kruskal, 8, 5, 6);
    Kruskal_add_edge(&kruskal, 9, 5, 2);

    Kruskal_algorithm(&kruskal);

    Kruskal_print_mst(&kruskal);

    Kruskal_destroy(&kruskal);

    benchmark(1000, 10000);

    benchmark(1000, 100000);
    benchmark(1000, 1000000);

    benchmark(1000, 1000000);
    benchmark(10000, 1000000);
    benchmark(100000, 1000000);

    return 0;
}
