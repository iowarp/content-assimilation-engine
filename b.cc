#include <string>
#include <hermes/data_stager/stager_factory.h>

using namespace hermes;

int main(int argc, char **argv) {
  std::string url = "file";
  std::string p = "";
  auto stager = StagerFactory::Get(url, p);
}
