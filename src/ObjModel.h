/*
 *  ObjModel.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_OBJMODEL
#define _TWS_OBJMODEL

#include <Model.h>

namespace TWS
{
	class ObjModel : public Model {
	public:
		ObjModel();
		~ObjModel();

		bool load(const char *filename);
	};
};

#endif
