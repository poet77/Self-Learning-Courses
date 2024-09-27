#pragma once
#include "Poisson.h"

class CGetmixgrad :
	public CPoissonEdit
{
public:
	CGetmixgrad(void);
	~CGetmixgrad(void);

	int v(QPoint, QPoint, int, QPoint);
};

