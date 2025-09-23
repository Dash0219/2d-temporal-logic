#include <cassert>
#include <chrono>
#include <iostream>
#include <stack>
#include <string>
#include <sys/resource.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "Formula.h"
#include "ASTNode.h"
#include "MaximalConsistentSet.h"
#include "Cluster.h"
#include "Trace.h"
#include "BottomUpAlgorithm.h"


using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;


/*
grammar:
FMLA := (FMLA binary FMLA) | unary FMLA | prop
binary := /\ | \/ | -> | <>
unary := ~ | G | H | F | P
prop := [a..z0..9_]+
*/

///*
// make the graph 
// formatted for: https://csacademy.com/app/graph_editor/
std::vector<std::vector<bool>> create_and_print_graph(Formula& fmla, bool print_non_reduced_graph) {
    int size_i = fmla.irreflexives.size();
    int size_c = fmla.clusters.size();
    std::vector<std::vector<bool>> g(size_i + size_c, std::vector<bool>(size_i + size_c, false));

    int i = 0;
    for (auto it_i = fmla.irreflexives.begin(); it_i != fmla.irreflexives.end(); ++it_i, ++i) {
        int j = 0;
        for (auto it_j = fmla.irreflexives.begin(); it_j != fmla.irreflexives.end(); ++it_j, ++j) {
            if (**it_i <= **it_j) {
                g[i][j] = true;
                if (print_non_reduced_graph) std::cout << "i" << i << " " << "i" << j << "\n";
            }
        }

        j = 0;
        for (auto it_j = fmla.clusters.begin(); it_j != fmla.clusters.end(); ++it_j, ++j) {
            if (**it_i <= **it_j) {
                g[i][size_i + j] = true;
                if (print_non_reduced_graph) std::cout << "i" << i << " " << "c" << j << "\n";
            }
        }
    }

    i = 0;
    for (auto it_i = fmla.clusters.begin(); it_i != fmla.clusters.end(); ++it_i, ++i) {
        int j = 0;
        for (auto it_j = fmla.irreflexives.begin(); it_j != fmla.irreflexives.end(); ++it_j, ++j) {
            if (**it_i <= **it_j) {
                g[size_i + i][j] = true;
                if (print_non_reduced_graph) std::cout << "c" << i << " " << "i" << j << "\n";
            }
        }

        j = 0;
        for (auto it_j = fmla.clusters.begin(); it_j != fmla.clusters.end(); ++it_j, ++j) {
            if (**it_i <= **it_j) {
                g[size_i + i][size_i + j] = true;
                if (print_non_reduced_graph) std::cout << "c" << i << " " << "c" << j << "\n";
            }
        }
    }

    if (print_non_reduced_graph) std::cout << std::string(32, '=') << "\n";

    // trasitive reduction
    for (int i = 0; i < size_i + size_c; ++i) 
        g[i][i] = false;

    std::vector<std::vector<bool>> original = g;
    for (int i = 0; i < size_i + size_c; ++i) {
        for (int j = 0; j < size_i + size_c; ++j) {
            // if (!g[i][j]) continue;
            for (int k = 0; k < size_i + size_c; ++k) {
                // if (g[j][k]) g[i][k] = false;
                if (original[i][j] && original[j][k]) g[i][k] = false;
            }
        }
    }

    for (int i = 0; i < size_i + size_c; ++i) {
        for (int j = 0; j < size_i + size_c; ++j) {
            if (!g[i][j]) continue;
            std::string node_a;
            std::string node_b;
            (i < size_i) ? node_a = "i" + std::to_string(i) : node_a = "c" + std::to_string(i - size_i);
            (j < size_i) ? node_b = "i" + std::to_string(j) : node_b = "c" + std::to_string(j - size_i);
            std::cout << node_a << " " << node_b << "\n";
        }
    }

    std::cout << std::string(32, '=') << "\n";

    return g;
}
//*/

///*
// use (F(p/\q)/\Pr) as the testcase
void trace_test(Formula& fmla) {
    std::cout << std::boolalpha;

    Trace a, b, c, d, e, f, g;
    a.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 1)]);
    a.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 3)]);
    a.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 3)]);
    b.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 3)]);
    b.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 1)]);
    b.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 2)]);
    c.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 1)]);
    c.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 3)]);
    d.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 1)]);
    d.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 3)]);
    e.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 1)]);
    e.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 2)]);
    f.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 1)]);
    f.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 3)]);
    g.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 3)]);
    g.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 3)]);
    g.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 1)]);
    g.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 2)]);

    a.join(b);
    b.join(a);
    c.join(b);
    d.join(e);
    f.join(g);

    assert(a.sequence.size() == 5);
    assert(b.sequence.size() == 3);
    assert(c.sequence.size() == 5);
    assert(d.sequence.size() == 2);
    assert(e.sequence.size() == 2);
    assert(f.sequence.size() == 5);
    assert(g.sequence.size() == 4);

    assert(a.join(c) == false);
    assert(a == c);
}
//*/

void print_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);

    std::cout << "Max memory used: " << usage.ru_maxrss / 1024 << " MB" << "\n";
    std::cout << std::string(32, '=') << "\n";
}

void print_time(const std::string& label, TimePoint start, TimePoint end) {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << label << ": " << ms << " ms\n";
    std::cout << std::string(32, '=') << "\n";
}

int main() {
    std::vector<std::string> test_inputs = {
        // "~(p->p)",
        // "~(p->(q->p))",
        // "(p\\/r)",
        // "~((p->p)->(q->q))",
        // "(q/\\~(q/\\(p/\\q)))",

        // "~((p0->F~(p1/\\p2))->(Pp3->Pp3))", // formula is not in any mcs or cluster, not SAT

        // "((p0->F~(p1/\\p2))->(Pp3->Pp3))",
        // "(~((p0->~(p1/\\p2))->(p3->p4))/\\~((p5->~(p6/\\p7))->(p8->p9)))",
        // "(~((p0->~(p1/\\p2))->(p3->p4))/\\~((p5->~(p6/\\p7))->(p8->Pp9)))",

        // "(~((p0->~(p1/\\Fp2))->(Fp3->Fp4))/\\~((p2->~(p0/\\Fp1))->(p2->p0)))",
        // "~~~~Fp", 
        // "Fp", 
        // "~P(Fp\\/(Pp\\/p))",
        // "(Fp\\/(Pp\\/p))",
        // "(p/\\F(q->Pp))",
        // "~(p/\\F(q->Pp))",
        // "(p/\\~p)",
        // "(F(p/\\q)/\\Pr)",
        // "(~F~p/\\p)"
        // "(Pp/\\~Fp)",
        // "(~Fp/\\~F~p)",
        // "(Fp/\\Gp)",
        // "(Pp/\\~Pp)",
        // "(Pp\\/~Pp)",
        // "PPPPPPPPPp", // 10
        // "PPPPPPPPPPPPPPp", // 15
        // "HHHHHHHHHHHHHHHHHHHp", // 20

        // 3-2 density axiom
        // "((Fp0/\\(Fp1/\\Fp2))->F((Fp0/\\Fp1)\\/((Fp0/\\Fp2)\\/(Fp1/\\Fp2))))",
        // "~((Fp0/\\(Fp1/\\Fp2))->F((Fp0/\\Fp1)\\/((Fp0/\\Fp2)\\/(Fp1/\\Fp2))))",

        // "~((Fp0/\\Fp1)->F(Fp0\\/Fp1))",

        // "~((Fp0/\\(Fp1/\\(Fp2/\\Fp3)))->F((Fp0/\\(Fp1/\\Fp2))\\/((Fp0/\\(Fp2/\\Fp3))\\/((Fp0/\\(Fp1/\\Fp3))\\/(Fp0/\\(Fp1/\\Fp2))))))",

        "~(G(p->q)->(Gp->Gq))",
        // "~(Gp<>GGp)",
    };

    for (std::string test : test_inputs) {
        std::cout << std::string(32, '=') << "\n";
        std::cout << test << " ";

        auto total_start = Clock::now();

        Formula fmla(test);

        auto setup_start = Clock::now();
        if (fmla.setup_formula()) {
            auto setup_end = Clock::now();

            std::cout << "is a formula" << "\n";
            std::cout << std::string(32, '=') << "\n";
            // fmla.show_tree();
            // fmla.show_propositions();
            // fmla.show_temporal_formulas();
            fmla.show_closure_set();

            // fmla.show_irreflexives();
            // fmla.show_clusters(true);
            // fmla.show_clusters(false);
            // fmla.show_graphs();

            auto algo_start = Clock::now();
            BottomUpAlgorithm algo(fmla);
            if (algo.run(true)) {
                std::cout << "SAT\n";
            } else {
                std::cout << "not SAT\n";
            }
            auto algo_end = Clock::now();

            std::cout << std::string(32, '=') << "\n";

            print_time("setup time", setup_start, setup_end);
            print_time("algorithm time", algo_start, algo_end);
        } else {
            std::cout << "is not a formula" << "\n";
            std::cout << std::string(32, '=') << "\n";
        }
        auto total_end = Clock::now();

        // debugging functions
        // std::vector<std::vector<bool>> g;
        // g = create_and_print_graph(fmla, false);
        // trace_test(fmla);
        print_time("total time", total_start, total_end);
        print_memory_usage();
    }

    return 0;
}


/*
draft paper:

what is the number of distinct boundary maps?
paper says it's O(exp(n))

4 mcs * 4 trace * 2 cluster
O(exp(n)) * O(exp(n)) * O(exp(n)) = O(exp(n))


*/