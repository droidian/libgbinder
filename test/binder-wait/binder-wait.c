/*
 * Copyright (C) 2018-2020 Jolla Ltd.
 * Copyright (C) 2018-2020 Slava Monich <slava.monich@jolla.com>
 * Copyright (C) 2023 Erfan Abdi <erfangplus@gmail.com>
 *
 * You may use this file under the terms of BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the names of the copyright holders nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <gbinder.h>

#include <gutil_log.h>

#define RET_OK          (0)
#define RET_NOTFOUND    (1)
#define RET_INVARG      (2)
#define RET_ERR         (3)

#define DEV_DEFAULT     GBINDER_DEFAULT_HWBINDER

typedef struct app_options {
    char* dev;
    const char* service;
} AppOptions;

typedef struct app {
    const AppOptions* opt;
    GMainLoop* loop;
    GBinderServiceManager* sm;
    gulong wait_id;
    int ret;
} App;

static const char pname[] = "binder-wait";

static
void
app_print_strings(
    char** strv)
{
    if (strv) {
        while (*strv) {
            printf("%s\n", *strv);
            strv++;
        }
    }
}

static
void
app_registration_handler(
    GBinderServiceManager* sm,
    const char* name,
    void* user_data)
{
    App* app = user_data;
    const AppOptions* opt = app->opt;

    GINFO("\"%s\" appeared", name);
    if (!strcmp(name, opt->service)) {
        gbinder_servicemanager_remove_handler(app->sm, app->wait_id);
        app->wait_id = 0;
        app->ret = RET_OK;
        g_main_loop_quit(app->loop);
    }
}

static
void
app_sync(
   App* app)
{
    const AppOptions* opt = app->opt;
    if (opt->service) {
        int status = 0;
        guint8 transport = 0;
        if (!strcmp(GBINDER_DEFAULT_HWBINDER, opt->dev)) {
            transport = gbinder_servicemanager_get_service_transport_sync
                (app->sm, opt->service, &status);
            if (transport == 0) {
                GERR("No such service: %s (%d)", opt->service, status);
                app->ret = RET_NOTFOUND;
                return;
            }
        }

        GBinderRemoteObject* obj = gbinder_servicemanager_get_service_sync
            (app->sm, opt->service, &status);

        if (obj) {
            printf("%s\n", opt->service);
            app->ret = RET_OK;
        } else {
            app->loop = g_main_loop_new(NULL, TRUE);

            GINFO("Waiting for %s", opt->service);
            app->wait_id = gbinder_servicemanager_add_registration_handler(app->sm,
                opt->service, app_registration_handler, app);

            g_main_loop_run(app->loop);
            g_main_loop_unref(app->loop);
            app->loop = NULL;
        }
    } else {
        char** services = gbinder_servicemanager_list_sync(app->sm);

        if (services) {
            app_print_strings(services);
            g_strfreev(services);
            app->ret = RET_OK;
        }
    }
}

static
gboolean
app_log_verbose(
    const gchar* name,
    const gchar* value,
    gpointer data,
    GError** error)
{
    gutil_log_default.level = GLOG_LEVEL_VERBOSE;
    return TRUE;
}

static
gboolean
app_log_quiet(
    const gchar* name,
    const gchar* value,
    gpointer data,
    GError** error)
{
    gutil_log_default.level = GLOG_LEVEL_ERR;
    return TRUE;
}

static
gboolean
app_init(
    AppOptions* opt,
    int argc,
    char* argv[])
{
    gboolean ok = FALSE;
    GOptionEntry entries[] = {
        { "verbose", 'v', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
          app_log_verbose, "Enable verbose output", NULL },
        { "quiet", 'q', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
          app_log_quiet, "Be quiet", NULL },
        { "device", 'd', 0, G_OPTION_ARG_STRING, &opt->dev,
          "Binder device [" DEV_DEFAULT "]", "DEVICE" },
        { NULL }
    };

    GError* error = NULL;
    GOptionContext* options = g_option_context_new("[SERVICE]");

    memset(opt, 0, sizeof(*opt));

    gutil_log_timestamp = FALSE;
    gutil_log_set_type(GLOG_TYPE_STDERR, pname);
    gutil_log_default.level = GLOG_LEVEL_DEFAULT;

    g_option_context_add_main_entries(options, entries, NULL);
    if (g_option_context_parse(options, &argc, &argv, &error)) {
        char* help;

        if (!opt->dev || !opt->dev[0]) opt->dev = g_strdup(DEV_DEFAULT);
        switch (argc) {
        case 2:
            opt->service = argv[1];
            /* no break */
        case 1:
            ok = TRUE;
            break;
        default:
            help = g_option_context_get_help(options, TRUE, NULL);
            fprintf(stderr, "%s", help);
            g_free(help);
            break;
        }
    } else {
        GERR("%s", error->message);
        g_error_free(error);
    }
    g_option_context_free(options);
    return ok;
}

int main(int argc, char* argv[])
{
    App app;
    AppOptions opt;

    memset(&app, 0, sizeof(app));
    app.ret = RET_INVARG;
    app.opt = &opt;
    if (app_init(&opt, argc, argv)) {
        app.sm = gbinder_servicemanager_new(opt.dev);
        if (gbinder_servicemanager_wait(app.sm, -1)) {
            app_sync(&app);
            gbinder_servicemanager_unref(app.sm);
        }
    }
    g_free(opt.dev);
    return app.ret;
}

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
