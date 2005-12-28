// Brazilian description headers

#include "language.h"
#include "expansion.h"

class br_expansion;

class br_language: public language
{
public:
  br_language();
  ~br_language();
  virtual char *post_prepare (char *&text);

private:
  br_expansion* my_expansion;
};

extern br_language *Brazilian;
