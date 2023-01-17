#pragma once

#define E_FLAG(e, flg) e & flg != 0;

// str - name/identifier of function
#define SINGLEINIT(str) static bool initialized = false; if (initialized) { EXC(std::string("Tried to re-initialize ") + str); } initialized = true;

#define PAD(bytes, index)	char pad_##index[bytes];