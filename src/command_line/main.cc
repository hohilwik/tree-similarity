// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Nikolaus Augsten, and Daniel Kocher.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "node.h"
#include "string_label.h"
#include "unit_cost_model.h"
#include "bracket_notation_parser.h"
#include "apted_tree_index.h"
#include "zhang_shasha_tree_index.h"
#include "touzet_baseline_tree_index.h"
#include "touzet_depth_pruning_tree_index.h"
#include "touzet_depth_pruning_truncated_tree_fix_tree_index.h"
#include "touzet_kr_loop_tree_index.h"
#include "touzet_kr_set_tree_index.h"
#include "tree_indexer.h"

/// Simple command-line tool for executing Tree Edit Distance.
int main(int argc, char** argv) {
    using Label = label::StringLabel;
    using CostModelLD = cost_model::UnitCostModelLD<Label>;
    using LabelDictionary = label::LabelDictionary<Label>;
    using TreeIndexAll = node::TreeIndexAll;

    // sanity check
    printf("Number of parameters: %d\n", argc);


    if(argc<5 || argc>6) {
        std::cerr << "Incorrect number of parameters. Sample usage for string/file: ./ted apted string {x{a}} {x{b}}" << std::endl;
        std::cerr << "Sample usage for linewise: ./ted apted linewise input1_file.txt input2_file.txt results.txt" << std::endl;
        return -1;
    }
    if (argc==5 && !(std::strcmp(argv[2], "string") == 0 || std::strcmp(argv[2], "file") == 0) ) {
        std::cerr << "Incorrect parameters. Sample usage for string/file: ./ted apted string {x{a}} {x{b}}" << std::endl;
        return -1;
    }

    if (argc==6 && std::strcmp(argv[2], "linewise") != 0) {
        std::cerr << "Incorrect parameters. Sample usage for linewise: ./ted apted linewise input1_file.txt input2_file.txt results.txt" << std::endl;
        return -1;
    }

    std::string algorithm_name = argv[1];
    std::string input_format = argv[2];
    std::string source_tree_string;
    std::string dest_tree_string;

    parser::BracketNotationParser<Label> bnp;

    LabelDictionary ld;
    CostModelLD ucm(ld);

    // Initialise TED algorithms.
    ted::ZhangShashaTreeIndex<CostModelLD, TreeIndexAll> zhang_shasha_algorithm(ucm);
    ted::APTEDTreeIndex<CostModelLD, TreeIndexAll> apted_algorithm(ucm);
    ted::TouzetBaselineTreeIndex<CostModelLD, TreeIndexAll> touzet_baseline_algorithm(ucm);
    ted::TouzetDepthPruningTreeIndex<CostModelLD, TreeIndexAll> touzet_depth_pruning_algorithm(ucm);
    ted::TouzetDepthPruningTruncatedTreeFixTreeIndex<CostModelLD, TreeIndexAll> touzet_depth_pruning_truncated_tree_fix_algorithm(ucm);
    ted::TouzetKRLoopTreeIndex<CostModelLD, TreeIndexAll> touzet_kr_loop_algorithm(ucm);
    ted::TouzetKRSetTreeIndex<CostModelLD, TreeIndexAll> touzet_kr_set_algorithm(ucm);

    // Pointer to the chosen TED algorithm.
    ted::TEDAlgorithm<CostModelLD, TreeIndexAll>* ted_algorithm = nullptr;

    // Assign TED algorithm by its name.
    if (algorithm_name == "zhang_shasha") {
        ted_algorithm = &zhang_shasha_algorithm;
    } else if (algorithm_name == "apted") {
        ted_algorithm = &apted_algorithm;
    } else if (algorithm_name == "touzet_baseline") {
        ted_algorithm = &touzet_baseline_algorithm;
    } else if (algorithm_name == "touzet_depth_pruning") {
        ted_algorithm = &touzet_depth_pruning_algorithm;
    } else if (algorithm_name == "touzet_depth_pruning_truncated_tree_fix") {
        ted_algorithm = &touzet_depth_pruning_truncated_tree_fix_algorithm;
    } else if (algorithm_name == "touzet_kr_loop") {
        ted_algorithm = &touzet_kr_loop_algorithm;
    } else if (algorithm_name == "touzet_kr_set") {
        ted_algorithm = &touzet_kr_set_algorithm;
    } else {
        std::cerr << "Error while choosing TED algorithm" << std::endl;
        return -1;
    }

    if (input_format == "string") {
        source_tree_string = argv[3];
        dest_tree_string = argv[4];
    } else if (input_format == "file") {
        std::ifstream tree_file(argv[3]);
        std::getline(tree_file, source_tree_string);
        tree_file.close();

        tree_file = std::ifstream(argv[4]);
        std::getline(tree_file, dest_tree_string);
        tree_file.close();
    } else if (input_format == "linewise") {
        std::ifstream source_file(argv[3]);
        std::ifstream dest_file(argv[4]);
        std::ofstream result_file(argv[5]);

        if (!source_file.is_open() || !dest_file.is_open() || !result_file.is_open()) {
            std::cerr << "Error opening one of the files." << std::endl;
            return -1;
        }

        ////////////

        std::string source_line;
        std::string dest_line;
        while (std::getline(source_file, source_line) && std::getline(dest_file, dest_line)) {
            if (!bnp.validate_input(source_line) || !bnp.validate_input(dest_line)) {
                std::cerr << "Incorrect format of trees. Is the number of opening and closing brackets equal?" << std::endl;
                continue;
            }

            node::TreeIndexAll ti1;
            node::TreeIndexAll ti2;

            node::Node<Label> t1 = bnp.parse_single(source_line);
            node::Node<Label> t2 = bnp.parse_single(dest_line);

            node::index_tree(ti1, t1, ld, ucm);
            node::index_tree(ti2, t2, ld, ucm);

            double ted_distance = ted_algorithm->ted(ti1, ti2);
            result_file << ted_distance << std::endl;
        }

        source_file.close();
        dest_file.close();
        result_file.close();

        std::cout << "Exiting..." << std::endl;
        // exit program, everything is done for this branch
        return 0;
    } else {
        std::cerr << "Incorrect input format. Use string, file, or linewise." << std::endl;
        return -1;
    }

    if (!bnp.validate_input(source_tree_string)) {
        std::cerr << "Incorrect format of source tree. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
    }
    const node::Node<Label> source_tree = bnp.parse_single(source_tree_string);

    if (!bnp.validate_input(dest_tree_string)) {
        std::cerr << "Incorrect format of destination tree. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
    }
    const node::Node<Label> destination_tree = bnp.parse_single(dest_tree_string);

    std::cout << "Size of source tree:" << source_tree.get_tree_size() << std::endl;
    std::cout << "Size of destination tree:" << destination_tree.get_tree_size() << std::endl;

        /////////////

    node::TreeIndexAll ti1;
    node::TreeIndexAll ti2;
    node::index_tree(ti1, source_tree, ld, ucm);
    node::index_tree(ti2, destination_tree, ld, ucm);
    std::cout << "Distance:" << ted_algorithm->ted(ti1, ti2) << std::endl;

    return 0;
}
