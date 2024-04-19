#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include "cmdline.h"
#include "Common.hpp"
#include "PlainSlp.hpp"
#include "PoSlp.hpp"
#include "ShapedSlp_Status.hpp"
#include "ShapedSlp.hpp"
#include "ShapedSlpV2.hpp"
#include "SelfShapedSlp.hpp"
#include "SelfShapedSlpV2.hpp"
#include "DirectAccessibleGammaCode.hpp"
#include "IncBitLenCode.hpp"
#include "FixedBitLenCode.hpp"
#include "SelectType.hpp"
#include "VlcVec.hpp"

using namespace std;

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

using var_t = uint32_t;
string file_gcx;
template <class SlpT>
void measure(
    std::string in,
    string query_file)
{
  FILE *report_gcx = fopen(file_gcx.c_str(), "a");
  //long long int peak = malloc_count_peak();
  //long long int stack = stack_count_usage(base);
  SlpT slp;
  uint64_t n;
  uint64_t lenExpand;
  auto start = timer::now();
  ifstream fs(in);
  slp.load(fs);
  auto stop = timer::now();
  cout << "time to load (ms): " << duration_cast<milliseconds>(stop - start).count() << endl;
  // slp.printStatus();

  start = timer::now();
  std::ifstream query(query_file);
  vector<pair<uint64_t, uint64_t>> v_query;
  uint64_t l, r;
  query >> n >> lenExpand;
  while (query >> l >> r)
  {
    v_query.push_back(make_pair(l, r));
  }
  stop = timer::now();
  cout << "Time to load query file (ms): " << duration_cast<milliseconds>(stop - start).count() << endl;

  const uint64_t textLen = slp.getLen();
  if (v_query[0].first >= textLen or lenExpand >= textLen or lenExpand >= textLen) {
    cout << "firstPos, lenExpand and jump should be smaller than text length, which is " << textLen << endl;
    exit(1);
  }
  string substr;
  cout << "n = " << n << "Expand size = " << lenExpand << endl;
  substr.resize(lenExpand);
  auto first = std::chrono::high_resolution_clock::now();
  auto total_time = std::chrono::high_resolution_clock::now();
  for (auto p : v_query)
  {
    auto beg = p.first;
    auto t0 = std::chrono::high_resolution_clock::now();
    slp.expandSubstr(beg, lenExpand, substr.data());
    auto t1 = std::chrono::high_resolution_clock::now();
    total_time += t1 - t0;
  }
  std::chrono::duration<double> elapsed = total_time - first;

  cout << "Batch Extraction Total time(s): " << elapsed.count() << endl;
  cout << "Mean time (microseconds): " << elapsed.count() / n * 1e6 << endl;
  fprintf(report_gcx, "0|0|%5.4lf|", elapsed);
  printf("Time inserted into the gcx report: %5.4lf\n", elapsed);
  fclose(report_gcx);
}

int main(int argc, char *argv[])
{
  using Fblc = FixedBitLenCode<>;
  using SelSd = SelectSdvec<>;
  using SelMcl = SelectMcl<>;
  using DagcSd = DirectAccessibleGammaCode<SelSd>;
  using DagcMcl = DirectAccessibleGammaCode<SelMcl>;
  using Vlc64 = VlcVec<sdsl::coder::elias_delta, 64>;
  using Vlc128 = VlcVec<sdsl::coder::elias_delta, 128>;
  using funcs_type = map<string,
                         void (*)(
                             std::string in,
                             string query_file)>;
  funcs_type funcs;

  //// PlainSlp
  funcs.insert(make_pair("PlainSlp_FblcFblc", measure<PlainSlp<var_t, Fblc, Fblc>>));
  funcs.insert(make_pair("PlainSlp_IblcFblc", measure<PlainSlp<var_t, IncBitLenCode, Fblc>>));
  funcs.insert(make_pair("PlainSlp_32Fblc", measure<PlainSlp<var_t, FixedBitLenCode<32>, Fblc>>));

  //// PoSlp: Post-order SLP
  //// Sometimes PoSlp_Sd is better than PoSlp_Iblc
  funcs.insert(make_pair("PoSlp_Iblc", measure<PoSlp<var_t, IncBitLenCode>>));
  funcs.insert(make_pair("PoSlp_Sd", measure<PoSlp<var_t, DagcSd>>));
  // funcs.insert(make_pair("PoSlp_Mcl", measure<PoSlp<var_t, DagcMcl>>));

  //// ShapedSlp: plain implementation of slp encoding that utilizes shape-tree grammar
  //// Since bit length to represent slp element is small, SelMcl is good for them.
  //// For stg and bal element, SelSd is better
  funcs.insert(make_pair("ShapedSlp_SdMclSd_SdMcl", measure<ShapedSlp<var_t, DagcSd, DagcMcl, DagcSd, SelSd, SelMcl>>));
  funcs.insert(make_pair("ShapedSlp_SdSdSd_SdMcl", measure<ShapedSlp<var_t, DagcSd, DagcSd, DagcSd, SelSd, SelMcl>>));

  //// ShapedSlpV2: all vlc vectors are merged into one.
  //// Generally encoding size gets worse than ShapedSlp_SdMclSd_SdMcl because
  //// - Since bit length to represnet stg and bal element is large, DagcSd is a good choice.
  //// - On the other hand, bit size to represent slp element is significantly small, and so SelMcl should be used
  funcs.insert(make_pair("ShapedSlpV2_Sd_SdMcl", measure<ShapedSlpV2<var_t, DagcSd, SelSd, SelMcl>>));
  // funcs.insert(make_pair("ShapedSlpV2_SdSdSd", measure<ShapedSlp<var_t, DagcSd, SelSd, SelSd>>));
  // funcs.insert(make_pair("ShapedSlpV2_SdMclMcl", measure<ShapedSlp<var_t, DagcSd, SelMcl, SelMcl>>));
  // funcs.insert(make_pair("ShapedSlpV2_Vlc128SdSd", measure<ShapedSlp<var_t, Vlc128, SelSd, SelSd>>));

  //// SelfShapedSlp: ShapedSlp that does not use shape-tree grammar
  funcs.insert(make_pair("SelfShapedSlp_SdSd_Sd", measure<SelfShapedSlp<var_t, DagcSd, DagcSd, SelSd>>));
  funcs.insert(make_pair("SelfShapedSlp_SdSd_Mcl", measure<SelfShapedSlp<var_t, DagcSd, DagcSd, SelMcl>>));
  // funcs.insert(make_pair("SelfShapedSlp_MclMcl_Sd", measure<SelfShapedSlp<var_t, DagcMcl, DagcMcl, SelSd>>));
  // funcs.insert(make_pair("SelfShapedSlp_SdMcl_Sd", measure<SelfShapedSlp<var_t, DagcSd, DagcMcl, SelSd>>));

  //// SelfShapedSlpV2:
  //// attempted to asign smaller offsets to frequent variables by giving special seats for hi-frequent ones
  funcs.insert(make_pair("SelfShapedSlpV2_SdSd_Sd", measure<SelfShapedSlpV2<var_t, DagcSd, DagcSd, SelSd>>));
  // funcs.insert(make_pair("SelfShapedSlpV2_SdSd_Mcl", measure<SelfShapedSlpV2<var_t, DagcSd, DagcSd, SelMcl>>));

  string methodList;
  for (auto itr = funcs.begin(); itr != funcs.end(); ++itr)
  {
    methodList += itr->first + ". ";
  }

  cmdline::parser parser;
  parser.add<string>("input", 'i', "input file name in which ShapedSlp data structure is written.", true);
  parser.add<string>("encoding", 'e', "encoding: " + methodList, true);
  parser.add<string>("query_file", 'q', "query file containing the interval of substring to be extracted", true);
  parser.add<string>("file_report_gcx", 'r', "report gcx", true);
  parser.parse_check(argc, argv);
  const string in = parser.get<string>("input");
  const string encoding = parser.get<string>("encoding");
  const string query_file = parser.get<string>("query_file");
  file_gcx = parser.get<string>("file_report_gcx");
  //To GCX
  //clock_t start_gcx, finish_gcx;
  //void* base = stack_count_clear();
  //double duration =0.0;
  //auto start = timer::now();
  if (encoding.compare("All") == 0)
  {
    for (auto itr = funcs.begin(); itr != funcs.end(); ++itr)
    {
      cout << itr->first << ": BEGIN" << std::endl;
      itr->second(in + itr->first, query_file);
      cout << itr->first << ": END" << std::endl;
    }
  }
  else
  {
    auto itr = funcs.find(encoding);
    if (itr != funcs.end())
    {
      cout << itr->first << ": BEGIN" << std::endl;
      itr->second(in, query_file);
      cout << itr->first << ": END" << std::endl;
    }
    else
    {
      cerr << "error: specify a valid encoding name in " + methodList << endl;
      exit(1);
    }
  }
  //auto stop = timer::now();
  //duration = (double)duration_cast<seconds>(stop - start).count();
  //To GCX
  

  // { // correctness check
  //   PoSlp<var_t> poslp;
  //   ShapedSlp<var_t> sslp;
  //   ShapedSlpDagc<var_t> sslpdagc;
  //   {
  //     ifstream fs("temp0");
  //     poslp.load(fs);
  //   }
  //   {
  //     ifstream fs("temp1");
  //     sslp.load(fs);
  //   }
  //   {
  //     ifstream fs("temp2");
  //     sslpdagc.load(fs);
  //   }

  //   cout << "numItr = " << numItr << ", lenExpand = " << lenExpand << ", firstPos = " << firstPos << ", jump = " << jump << endl;
  //   const uint64_t textLen = poslp.getLen();
  //   if (firstPos >= textLen or lenExpand >= textLen or lenExpand >= textLen) {
  //     cout << "firstPos, lenExpand and jump should be smaller than text length, which is " << textLen << endl;
  //     exit(1);
  //   }

  //   string substr0, substr1, substr2;
  //   substr0.resize(lenExpand);
  //   substr1.resize(lenExpand);
  //   substr2.resize(lenExpand);
  //   uint64_t beg = firstPos;
  //   for (uint64_t i = 0; i < numItr; ++i) {
  //     cout << beg << endl;
  //     // substr0[0] = poslp.charAt(beg);
  //     // substr1[0] = sslp.charAt(beg);
  //     // substr2[0] = sslpdagc.charAt(beg);
  //     poslp.expandSubstr(beg, lenExpand, substr0.data());
  //     sslp.expandSubstr(beg, lenExpand, substr1.data());
  //     sslpdagc.expandSubstr(beg, lenExpand, substr2.data());
  //     if ((substr0 != substr1) or (substr1 != substr2) or (substr0 != substr2)) {
  //       cout << "wrong: beg = " << beg << endl;
  //       cout << substr0 << endl;
  //       cout << substr1 << endl;
  //       cout << substr2 << endl;
  //       exit(1);
  //     }
  //     beg += jump;
  //     if (beg > textLen - lenExpand) {
  //       beg -= (textLen - lenExpand);
  //     }
  //   }
  // }

  return 0;
}
