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
// These are linux only headers, not available on Windows
//#include <sys/time.h>
//#include <sys/resource.h>
//#include "node.h"
//#include "string_label.h"
//#include "unit_cost_model.h"
//#include "bracket_notation_parser.h"
//#include "lgm_tree_index.h"

#include "../node/node.h"
#include "../label/string_label.h"
#include "../cost_model/unit_cost_model.h"
#include "../parser/bracket_notation_parser.h"
#include "../ted_ub/lgm_tree_index.h"


/// Simple command-line tool for executing Tree Edit Distance.

int main(int argc, char** argv) {

  using Label = label::StringLabel;
  using CostModelLD = cost_model::UnitCostModelLD<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;

  /////// Runtime measurement should be limited to tests, and use portable libraries if available
  //// Runtime measurement variables (rusage).
  //rusage before_rusage;
  //rusage after_rusage;
  //timeval runtime_utime;
  //timeval runtime_stime;
  //unsigned int runtime;

  // Verify parameters.
  if (argc != 6) {
    std::cerr << "Incorrect number of parameters. Sample usage: ./ted string {x{a}} {x{b}} lgm 1" << std::endl;
    return -1;
  }

  // NOTE: Trees passed as command-line arguments must be sorrounded with ''.

  //std::cout << "Source tree: " << argv[2] << std::endl;
  //std::cout << "Destination tree: " << argv[3] << std::endl;

  std::string source_tree_string;
  std::string dest_tree_string;

  parser::BracketNotationParser<Label> bnp;
  // Verify the input format before parsing.

  if (std::strcmp(argv[1], "string") == 0) {
    source_tree_string = argv[2];
    dest_tree_string = argv[3];
  } else if (std::strcmp(argv[1], "file") == 0) {
    // getrusage(RUSAGE_SELF, &before_rusage);
    std::ifstream tree_file(argv[2]);
    //std::string tree_string;
    std::getline(tree_file, source_tree_string);
    tree_file.close();

    tree_file = std::ifstream(argv[3]);
    std::getline(tree_file, dest_tree_string);
    tree_file.close();
  } else {
    std::cerr << "Incorrect input format. Use either string or file." << std::endl;
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

  /*getrusage(RUSAGE_SELF, &after_rusage);
  timersub(&after_rusage.ru_utime, &before_rusage.ru_utime, &runtime_utime);
  timersub(&after_rusage.ru_stime, &before_rusage.ru_stime, &runtime_stime);
  runtime = runtime_utime.tv_usec + runtime_utime.tv_sec * 1000000 + runtime_stime.tv_usec + runtime_stime.tv_sec * 1000000;*/
  //std::cout << runtime << " " << source_tree.get_tree_size() << " " << destination_tree.get_tree_size() << " ";
  std::cout << "Size of source tree:" << source_tree.get_tree_size() << std::endl;
  std::cout << "Size of destination tree:" << destination_tree.get_tree_size() << std::endl;

  if (std::strcmp(argv[4], "lgm") == 0) {
    int k = std::stoi(argv[5]);
    //getrusage(RUSAGE_SELF, &before_rusage);
    LabelDictionary ld;
    CostModelLD ucm(ld);
    ted_ub::LGMTreeIndex<CostModelLD, node::TreeIndexLGM> lgm_algorithm(ucm);
    node::TreeIndexLGM ti1;
    node::TreeIndexLGM ti2;
    node::index_tree(ti1, source_tree, ld, ucm);
    node::index_tree(ti2, destination_tree, ld, ucm);
    lgm_algorithm.init(ti2);
    std::cout << "Distance TED_K:" << lgm_algorithm.ted_k(ti1, ti2, k) << std::endl;
    std::cout << "Distance TED:" << lgm_algorithm.ted(ti1, ti2) << std::endl;
    //getrusage(RUSAGE_SELF, &after_rusage);
    std::cout << "Number of subproblems:" << lgm_algorithm.get_subproblem_count() << std::endl;
    //timersub(&after_rusage.ru_utime, &before_rusage.ru_utime, &runtime_utime);
    //timersub(&after_rusage.ru_stime, &before_rusage.ru_stime, &runtime_stime);
    //runtime = runtime_utime.tv_usec + runtime_utime.tv_sec * 1000000 + runtime_stime.tv_usec + runtime_stime.tv_sec * 1000000;
    //std::cout << " " << runtime << std::endl;
  }

  return 0;
}
