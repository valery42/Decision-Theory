#include <cmath>
#include <cstdio>
#include <cstdlib>

using Money = double;

using State = Money;
using Win = Money;
using Control = Money;

const int N = 10; // discretization level
const int R = 9; // at the last stage X must be equal to K/9

struct Stage {
    State* K;
    Win* w;
    Control* X;
};

void init_stage(Stage* s, int n) {
    // n must be equal to N+1
    s->K = new State[n];
    s->w = new Win[n];
    s->X = new Control[n];
}

void delete_stage(Stage* s) {
    delete[] s->K;
    delete[] s->w;
    delete[] s->X;
}

void print_stage(const Stage* s, int n) {
    // n must be equal to N + 1
    for (int i = 0; i < n; i++) {
        printf("%6.2f, %6.2f, %6.2f\n", s->K[i], s->w[i], s->X[i]);
    }
}

Money income1(Money X) {
    return 150 + 0.8*pow(X, 2.0/3);
}

Money income2(Money Y) {
    return 110 + 1.6*pow(Y, 2.0/3);
}

Money income(Money K, Money X) {
    return income1(X) + income2(K-X);
}

Money balance1(Money X, int m) {
    return pow(0.74, m) * X;
}

Money balance2(Money Y, int m) {
    return pow(0.87, m) * Y;
}

Money balance(Money K, Money X, int m) {
    return balance1(X, m) + balance2(K-X, m);
}

void solve(Money K, int m, bool print_tables = false) {
    // K - initial amount of investment
    // m - # of stages
    if (K <= 0) {
        fputs("error: Initial amount of investment 'K' must be greater than zero.\n", stderr);
        exit(EXIT_FAILURE);
    }
    if (m <= 0) {
        fputs("error: Number of stages 'm' must be greater than zero.\n", stderr);
        exit(EXIT_FAILURE);
    }
    Stage stages[m];
    for (int i = 0; i < m; i++) {
        init_stage(&stages[i], N+1);
    }

    for (int i = m - 1; i >= 0; i--) {
        Money k_curr = balance1(K, i);
        Money k_step = (balance2(K, i) - k_curr) / N;
        int j;
        if (i == 0) {
            j = N;
            goto SKIP_LABEL; // skip the first N rows
        }
        for (j = 0; j <= N; j++) {
        SKIP_LABEL:
            stages[i].K[j] = k_curr;
            Money x_curr = 0;
            Money x_step = k_curr / N;
            Money x_best = -1;
            Money w_best = -1;
            for (int k = 0; k <= N; k++) {
                if (i == m - 1) {
                    x_best = k_curr / R;
                    w_best = income(k_curr, x_best);
                } else {
                    Money k_next = balance(k_curr, x_curr, 1);
                    int idx = 0;
                    while (k_next >= stages[i+1].K[idx]) {
                        idx++;
                    }
                    Money w_curr = income(k_curr, x_curr) + stages[i+1].w[idx-1];
                    if (w_curr > w_best) {
                        w_best = w_curr;
                        x_best = x_curr;
                    }
                }
                x_curr += x_step;
            }
            stages[i].w[j] = w_best;
            stages[i].X[j] = x_best;
            k_curr += k_step;
        }
    }

    if (print_tables) {
        for (int i = 0; i < m; i++) {
            printf("Table %d (K, w, X)\n", i+1);
            print_stage(&stages[i], N+1);
            puts("");
        }
    }

    Money total_income = stages[0].w[N];
    Money x = stages[0].X[N];
    Money y = stages[0].K[N] - stages[0].X[N];
    printf("Total income: %.2f\n", total_income);
    printf("\tStage 1: X=%6.2f, Y=%6.2f\n", x, y);
    for (int i = 0; i < m - 1; i++) {
        Money k_next = balance(x+y, x, 1);
        int idx = 0;
        while (k_next >= stages[i+1].K[idx]) {
            idx++;
        }
        x = stages[i+1].X[idx-1];
        y = stages[i+1].K[idx-1] - stages[i+1].X[idx-1];;
        printf("\tStage %d: X=%6.2f, Y=%6.2f\n", i+2, x, y);
    }

    for (int i = 0; i < m; i++) {
        delete_stage(&stages[i]);
    }
}

int main() {
    const Money K = 185;
    const int m = 4;
    solve(K, m, true);
}
