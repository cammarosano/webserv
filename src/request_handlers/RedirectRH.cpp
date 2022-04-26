#include "RedirectRH.hpp"

RedirectRH::RedirectRH(HttpRequest *request, FdManager &table) : AReqHandler(request, table)
{
    Redirection redirect;

    if (request->vserver->redirected)
        redirect = request->vserver->redirect;
    else
        redirect = request->route->redirect;
    response.status_code = redirect.status_code;
    response.header_fields["location"] = redirect.location;
    response.assemble_header_str();
}

RedirectRH::~RedirectRH()
{
}

int RedirectRH::respond()
{
    if (send_str(response.header_str) == 0)
        return 0;
    return 1;
}
