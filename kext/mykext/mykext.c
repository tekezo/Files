//
//  mykext.c
//  mykext
//
//  Created by Takayama Fumihiko on 6/19/16.
//  Copyright © 2016 Takayama Fumihiko. All rights reserved.
//

#include <mach/mach_types.h>

kern_return_t mykext_start(kmod_info_t * ki, void *d);
kern_return_t mykext_stop(kmod_info_t *ki, void *d);

kern_return_t mykext_start(kmod_info_t * ki, void *d)
{
    return KERN_SUCCESS;
}

kern_return_t mykext_stop(kmod_info_t *ki, void *d)
{
    return KERN_SUCCESS;
}
