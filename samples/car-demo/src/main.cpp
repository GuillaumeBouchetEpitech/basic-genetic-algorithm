
#include "application/Application.hpp"
#include "application/defines.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/math/clamp.hpp"

#include <array>
#include <sstream>

#include <cstdlib> // <= EXIT_SUCCESS

#if defined D_WEB_BUILD
#include <emscripten/emscripten.h> // <= EMSCRIPTEN_KEEPALIVE
#endif

namespace {

void
validateInputs(uint32_t inWidth, uint32_t inHeight) {

  if (::isinf(inWidth) || ::isnan(inWidth))
    D_THROW(std::runtime_error, "argument 0 (width) is not a valid number");
  if (::isinf(inHeight) || ::isnan(inHeight))
    D_THROW(std::runtime_error, "argument 1 (height) is not a valid number");

  if (inWidth < 100)
    D_THROW(std::runtime_error, "argument 0 (width) cannot be < 100");
  if (inHeight < 100)
    D_THROW(std::runtime_error, "argument 1 (height) cannot be < 100");
}

} // namespace

#if defined D_NATIVE_BUILD

namespace {

void
processCommandLineArgs(Application::Definition& def, int argc, char** argv) {
  if (argc <= 1) {
    return;
  }

  // array of pointers toward the demo definition arguments
  std::array<uint32_t*, 2> arguments{{
    &def.width,
    &def.height,
  }};

  std::vector<std::string_view> safeArgs;
  safeArgs.reserve(std::size_t(argc - 1));
  for (int ii = 1; ii < argc; ++ii)
    safeArgs.push_back(argv[ii]);

  const std::size_t safeSize = std::min(safeArgs.size(), arguments.size());
  for (std::size_t ii = 0; ii < safeSize; ++ii) {
    std::stringstream sstr;
    sstr << safeArgs.at(ii);

    uint32_t argValue;
    sstr >> argValue;

    *arguments.at(ii) = argValue;
  }
}

} // namespace

int
main(int argc, char** argv) {
  Application::Definition def;
  def.width = 800;
  def.height = 600;

  processCommandLineArgs(def, argc, argv);

  validateInputs(def.width, def.height);

  Application myApp(def);
  myApp.run();

  return EXIT_SUCCESS;
}

#else

namespace /* anonymous */
{
Application* myApplication = nullptr;
};

extern "C" {

EMSCRIPTEN_KEEPALIVE
void
startApplication(uint32_t inWidth, uint32_t inHeight) {
  if (myApplication)
    return;

  Application::Definition def;
  def.width = inWidth;
  def.height = inHeight;

  validateInputs(def.width, def.height);

  myApplication = new Application(def);
}

EMSCRIPTEN_KEEPALIVE
void
updateApplication(uint32_t inDelta, uint32_t inMouseLocked) {
  if (!myApplication)
    return;

  // myApplication->setMouseLockStatus(inMouseLocked != 0);

  myApplication->update(inDelta);
}

EMSCRIPTEN_KEEPALIVE
void
renderApplication() {
  if (!myApplication)
    return;

  myApplication->render();
}
}

#endif
