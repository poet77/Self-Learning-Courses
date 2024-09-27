#pragma once
#include "Poisson.h"

class CGetgrad :
	public CPoissonEdit
{
public:
	CGetgrad(void);
	~CGetgrad(void);

	int v(QPoint, QPoint, int, QPoint);
};

