
#include "wazuh_modules/wmodules.h"
#include <stdio.h>

static const char *XML_ENABLED = "enabled";
static const char *XML_TIMEOUT= "timeout";
static const char *XML_SCRIPT = "script";

static short eval_bool(const char *str)
{
    return !str ? OS_INVALID : !strcmp(str, "yes") ? 1 : !strcmp(str, "no") ? 0 : OS_INVALID;
}

// Reading function
int wm_key_request_read(xml_node **nodes, wmodule *module)
{
    unsigned int i;
    wm_krequest_t *key_request;

    os_calloc(1, sizeof(wm_krequest_t), key_request);
    key_request->enabled = 0;
    module->context = &WM_KEY_REQUEST_CONTEXT;
    module->tag = strdup(module->context->name);
    module->data = key_request;

    if (!nodes)
        return 0;

    for(i = 0; nodes[i]; i++)
    {
        if(!nodes[i]->element)
        {
            merror(XML_ELEMNULL);
            return OS_INVALID;
        }
        else if (!strcmp(nodes[i]->element, XML_ENABLED))
        {
            if (key_request->enabled = eval_bool(nodes[i]->content), key_request->enabled == OS_INVALID) {
                merror("Invalid content for tag '%s' at module '%s'.", XML_ENABLED, WM_KEY_REQUEST_CONTEXT.name);
                return OS_INVALID;
            }
        }
        else if(!strcmp(nodes[i]->element, XML_SCRIPT))
        {
            if(key_request->script) {
                free(key_request->script);
            }
            key_request->script = strdup(nodes[i]->content);
        }
        else if(!strcmp(nodes[i]->element, XML_TIMEOUT))
        {
            key_request->timeout = strtoul(nodes[i]->content, NULL, 0);

            if (key_request->timeout == 0 || key_request->timeout == UINT_MAX) {
                merror("Invalid interval at module '%s'", WM_KEY_REQUEST_CONTEXT.name);
                return OS_INVALID;
            }

            mdebug2("Timeout read: %d", key_request->timeout);
        } else {
            mwarn("No such tag <%s> at module '%s'.", nodes[i]->element, WM_KEY_REQUEST_CONTEXT.name);
        }

    }
    return 0;
}
