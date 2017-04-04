#pragma once

#define SAFE_DELETE(x) { delete x; x = nullpptr; }
#define SAFE_DELETE_ARRAY(x) { delete[] x; x = nullptr; }

typedef unsigned char uchar;