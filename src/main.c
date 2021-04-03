#undef __cplusplus

#include "hns.h"

int main(int argc, char* argv[]) {
  setbuf(stdout, NULL);

#ifdef CHECK_MEMORY_LEAKS
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  struct HNS hns = {0};
  int hns_error = hns_init(&hns);
  switch (hns_error) {
    case (HNS_SUCCESS):
      break;
    case (HNS_MANA_ERROR):
      fprintf(stderr, "Failed to setup HNS!\n");
      return 1;
    default:
      fprintf(stderr, "Unknown HNS error! Error code: %d\n", hns_error);
      return 2;
  }
  hns_start(&hns);
  hns_delete(&hns);

  return 0;
}
