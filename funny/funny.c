/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping. Email: fguangping@gmail.com
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public
 * License version 3 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License version 3 for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License version 3 along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ds.h"
#include "init.h"
#include "read.h"
#include "eval.h"
#include "write.h"

void wait_to_exit(Statement *st)
{
	if (st->type == ARG && st->children->head == st->children->tail
		&& strcmp(((Statement *)(st->children->head->element))->symbol, "exit") == 0)
	{
		exit(0);
	}
}

/***************************** REPL ******************************/

int main(int argc, char **argv)
{
	Statement *st = NULL;
	Object *obj = NULL;
	
	printf("Welcome to FUNNY programming world. Type {exit} to exit.\n");
	
	init();
	
	while (TRUE)
	{
		printf("> ");
		st = read_statement(stdin);
		
		if (empty_statement(st))
		{
			//break;
			printf("can't parse the statement.\n");
			continue;
		}

		wait_to_exit(st);
		
		obj = eval_statement(st, the_global_environment);
		if (obj == NULL)
		{
			printf("can't eval the statement.\n");
			continue;
		}
		
		write(stdout, obj);
		printf("\n");
	}

	return 0;
}
