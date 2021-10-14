#include <cstdio>

const int N = 4;
const int W = 3;

using weight = int;
using value = double;
using control = bool;

struct Item {
    weight w;
    value v;
};

struct Table {
    weight* w;
    value* v;
    control *c;
};

void init_table(Table* table, int n) {
    // n must be equal to W + 1
    table->w = new weight[n];
    table->v = new value[n];
    table->c = new control[n];
}

void delete_table(Table* table) {
    delete[] table->w;
    delete[] table->v;
    delete[] table->c;
}

void print_table(Table* table, int n) {
    for (int i = 0; i < n; i++) {
        printf("%5d %5.2f %5d\n", table->w[i], table->v[i], table->c[i]);
    }
}

void solve(const Item* items, int n, int W, bool print = false) {
    // items - array of items
    // n - # of items (tables)
    // W - # maximum weight capacity
    // m - # of rows in a table
    int m = W + 1;
    Table tables[n];
    for (int i = 0; i < n; i++) {
        init_table(&tables[i], m);
    }

    for (int i = n - 1; i >= 0; i--) {
        if (i == n - 1) {
            for (int j = 0; j < m; j++) {
                tables[i].w[j] = j;
                bool enough_space = j >= items[i].w;
                if (enough_space) {
                    tables[i].v[j] = items[i].v;
                    tables[i].c[j] = true;
                } else {
                    tables[i].v[j] = 0;
                    tables[i].c[j] = false;
                }
            }
            continue;
        }
        for (int j = 0; j < m; j++) {
            if (i == 0) {
                // skip the first W rows
                j = W;
            }
            tables[i].w[j] = j;
            bool enough_space = j >= items[i].w;
            bool adding_is_more_profitable = (
                    items[i].v + tables[i+1].v[j-items[i].w]
                 >= tables[i+1].v[j]
            );
            if (enough_space && adding_is_more_profitable) {
                tables[i].v[j] = items[i].v + tables[i+1].v[j-items[i].w];
                tables[i].c[j] = true;
            } else {
                tables[i].v[j] = tables[i+1].v[j];
                tables[i].c[j] = false;
            }
        }
    }

    if (print) {
        for (int i = 0; i < n; i++) {
            printf("Table %d (w, v, c)\n", i+1);
            print_table(&tables[i], m);
            puts("");
        }
    }

    value total = tables[0].v[W];
    printf("Knapsack value: %.2f\n", total);
    puts("Items inside:");
    weight ptr = W;
    for (int i = 0; i < n; i++) {
        if (tables[i].c[ptr]) {
            printf("\tItem %d\n", i+1);
            ptr = ptr - items[i].w;
        }
    }

    for (int i = 0; i < n; i++) {
        delete_table(&tables[i]);
    }
}

int main() {
    Item items[N] = {
        {1, 2},
        {1, 3},
        {2, 4},
        {1, 5}
    };
    solve(items, N, W, true);
}
