#include "debug_memory.h"
#include "h264_stream.h"
#include "h265_stream.h"
#include <iomanip>
#include <iostream>
#include <vector>

struct TestSps {
  TestSps(uint8_t *d, size_t s, bool b) : data(d), size(s), is_h264(b) {}
  uint8_t *data;
  size_t size;
  bool is_h264;
};

int main() {
  using std::cerr;
  using std::cout;
  using std::endl;

#if defined(__WIN32__)
  // Send all reports to STDOUT
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
  // Set the debug-heap flag so that freed blocks are kept on the
  // linked list, to catch any inadvertent use of freed memory
  SET_CRT_DEBUG_FIELD(_CRTDBG_DELAY_FREE_MEM_DF);
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtMemState start_state, end_state, diff;

  _CrtMemCheckpoint(&start_state);
#endif // 
  {
    std::vector<TestSps> test_sps;
    uint8_t h264_sps_0[] = {0x67, 0x64, 0x00, 0x0d, 0xac, 0xd9, 0x41, 0x60,
                            0x96, 0x84, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00,
                            0x00, 0x03, 0x00, 0xca, 0x3c, 0x50, 0xa6, 0x58};
    uint8_t h265_sps_0[] = {0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03,
                            0x00, 0x90, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03,
                            0x00, 0x3c, 0xa0, 0x16, 0x20, 0x24, 0x59, 0x66,
                            0x66, 0x92, 0x4c, 0xae, 0x01, 0x00, 0x00, 0x03,
                            0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x19, 0x08};
    TestSps sps0(h264_sps_0, sizeof(h264_sps_0), true);
    TestSps sps1(h265_sps_0, sizeof(h265_sps_0), false);
    test_sps.push_back(sps0);
    test_sps.push_back(sps1);

    for (int i = 0; i < test_sps.size(); i++) {
      TestSps &sps = test_sps[i];
      cout << "sps start: " << std::hex << (size_t)sps.data << std::dec
           << ", size: " << sps.size << endl;
      if (sps.is_h264) {
        h264_stream_t *ctx = h264_new();
        if (read_nal_unit(ctx, sps.data, sps.size) > 0) {
#ifdef _DEBUG
          debug_nal(ctx, ctx->nal);
#endif
          cout << "width: " << ctx->info->width
               << ", height: " << ctx->info->height << endl;
        } else {
          cerr << "parse sps[" << i << "] failed" << endl;
        }
        h264_free(ctx);
      } else {
        h265_stream_t *ctx = h265_new();
        if (h265_read_nal_unit(ctx, sps.data, sps.size) > 0) {
          cout << "width: " << ctx->info->width
               << ", height: " << ctx->info->height << endl;
        } else {
          cerr << "parse sps[" << i << "] failed" << endl;
        }
        h265_free(ctx);
      }
      cout << endl << endl;
    }
  }

#if defined(__WIN32__)
  _CrtMemCheckpoint(&end_state);

  OutputHeading(
      "Dump the changes that occurred between two memory checkpoints");
  if (_CrtMemDifference(&diff, &start_state, &end_state)) {
    _CrtMemDumpStatistics(&diff);
    _CrtMemDumpAllObjectsSince(&diff);
  }
#endif // 

  return 0;
}
