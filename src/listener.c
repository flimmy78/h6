#include <errno.h>
#include "listener.h"

static __inline__ void
listener_finalize(listener_t *l)
{
    // nothing to do ...
}

static __inline__ void
on_listener_fin(obj_t *p)
{
	listener_t *l = (listener_t*)p;

	if (l->ops && l->ops->fin)
	{
		(*l->ops->fin)(l);
	}

	listener_finalize(l);
}


listener_t *
listener_alloc(uint32_t size, listener_ops *ops, void *u)
{
	listener_t *l;
	int32_t err;

	if (size < sizeof(*l))
		return NULL;

	l = (listener_t*)obj_new(size, on_listener_fin, (uint8_t *)__FUNCTION__);
	l->server = NULL;

	if (ops && ops->init)
	{
		err = (*ops->init)(l, u);
		if (err)
		{
			listener_finalize(l);
			obj_free((obj_t *)l);
			return NULL;
		}
	}

	l->ops = ops;
	return l;
}


int32_t
listener_bind(listener_t *l, uint16_t port)
{
	int32_t err = -EINVAL;

	if (l->ops && l->ops->set_port)
	{
		err = (*l->ops->set_port)(l, port);
	}

	return err;
}


int32_t
listener_start(listener_t *l)
{
	int32_t err = -EINVAL;

	if (l->ops && l->ops->run)
	{
		err = (*l->ops->run)(l);
	}

	return err;	
}


client_t *
listener_generate(listener_t *l, void *parm)
{
	client_t *c = NULL;

	if (l->ops && l->ops->new_cli)
	{
		c = (*l->ops->new_cli)(l, parm);
	}
    
	return c;
}


void listener_set_owner(listener_t *l, void *p)
{
	l->server = p;
}


void *listener_get_owner(listener_t *l)
{
	return l->server;
}


//:~ End
