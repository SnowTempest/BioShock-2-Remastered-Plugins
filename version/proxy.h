#pragma once

#ifndef VERSION_PROXY_H
#define VERSION_PROXY_H

namespace version::proxy {
	void init(HMODULE this_dll);
	void free();
}

#endif