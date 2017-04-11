#pragma once

#define SAFE_DELETE(x) { delete x; x = nullptr; }
#define SAFE_DELETE_ARRAY(x) { delete[] x; x = nullptr; }

#ifdef _DEBUG
#define DBG_ASSERT(x) { assert(x) }
#else
#define DBG_ASSERT(x) 
#endif

typedef unsigned char uchar;