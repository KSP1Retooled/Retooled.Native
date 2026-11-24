#include <cstring>
#include <dlfcn.h>

#include "Gpu.h"

namespace retooled {
namespace {
// From GLX_MESA_query_renderer.
constexpr int RendererUnifiedMemoryArchitecture = 0x8188;

struct Display;
using OpenDisplay = Display *(*) (const char *name);
using CloseDisplay = int (*) (Display *display);
using DefaultScreen = int (*) (Display *display);
using GetProcAddress = void *(*) (const unsigned char *name);
using QueryExtensionsString = const char *(*) (Display * display, int screen);
using QueryRendererInteger = int (*) (Display *display, int screen, int renderer, int attribute,
                                      unsigned int *value);

// Closes the libraries and the display connection that the query opened, in reverse order.
struct Opened {
	void *x11 = nullptr;
	void *gl = nullptr;
	Display *display = nullptr;
	CloseDisplay closeDisplay = nullptr;

	~Opened ()
	{
		if (display != nullptr)
			closeDisplay (display);

		if (gl != nullptr)
			dlclose (gl);

		if (x11 != nullptr)
			dlclose (x11);
	}
};

template<typename Function>
Function
Symbol (void *library, const char *name)
{
	return reinterpret_cast<Function> (dlsym (library, name));
}
} // namespace

// Unity's OpenGL context is current only on its render thread, so this asks about the renderer of
// the default X screen through a display connection of its own. Only Mesa's drivers have
// GLX_MESA_query_renderer, and those are the drivers of integrated GPUs on Linux. The plug-in loads
// X11 and GLX itself, so that it doesn't need them on a machine that runs Vulkan.
QueryResult
OpenGLUnifiedMemory ()
{
	Opened opened;
	opened.x11 = dlopen ("libX11.so.6", RTLD_NOW | RTLD_LOCAL);
	if (opened.x11 == nullptr)
		return QueryResult::Unknown;

	opened.gl = dlopen ("libGL.so.1", RTLD_NOW | RTLD_LOCAL);
	if (opened.gl == nullptr)
		return QueryResult::Unknown;

	auto openDisplay = Symbol<OpenDisplay> (opened.x11, "XOpenDisplay");
	auto defaultScreen = Symbol<DefaultScreen> (opened.x11, "XDefaultScreen");
	opened.closeDisplay = Symbol<CloseDisplay> (opened.x11, "XCloseDisplay");
	auto getProcAddress = Symbol<GetProcAddress> (opened.gl, "glXGetProcAddressARB");
	auto queryExtensionsString =
		Symbol<QueryExtensionsString> (opened.gl, "glXQueryExtensionsString");
	if (openDisplay == nullptr || defaultScreen == nullptr || opened.closeDisplay == nullptr
	    || getProcAddress == nullptr || queryExtensionsString == nullptr)
		return QueryResult::Unknown;

	auto queryRendererInteger = reinterpret_cast<QueryRendererInteger> (
		getProcAddress (reinterpret_cast<const unsigned char *> ("glXQueryRendererIntegerMESA")));
	if (queryRendererInteger == nullptr)
		return QueryResult::Unknown;

	// Null when the game runs on Wayland without XWayland.
	opened.display = openDisplay (nullptr);
	if (opened.display == nullptr)
		return QueryResult::Unknown;

	// GLVND picks the driver that serves the screen's extension functions on the first GLX call
	// that names the screen, so this call also lets the renderer query reach Mesa.
	int screen = defaultScreen (opened.display);
	const char *extensions = queryExtensionsString (opened.display, screen);
	if (extensions == nullptr || strstr (extensions, "GLX_MESA_query_renderer") == nullptr)
		return QueryResult::Unknown;

	unsigned int unified = 0;
	if (!queryRendererInteger (opened.display, screen, 0, RendererUnifiedMemoryArchitecture,
	                           &unified))
		return QueryResult::Unknown;

	if (unified == 0)
		return QueryResult::No;

	return QueryResult::Yes;
}
} // namespace retooled
