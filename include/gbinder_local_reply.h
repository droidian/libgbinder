/*
 * Copyright (C) 2018-2021 Jolla Ltd.
 * Copyright (C) 2018-2021 Slava Monich <slava.monich@jolla.com>
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

#ifndef GBINDER_LOCAL_REPLY_H
#define GBINDER_LOCAL_REPLY_H

#include "gbinder_types.h"

G_BEGIN_DECLS

GBinderLocalReply*
gbinder_local_reply_ref(
    GBinderLocalReply* reply);

void
gbinder_local_reply_unref(
    GBinderLocalReply* reply);

void
gbinder_local_reply_init_writer(
    GBinderLocalReply* reply,
    GBinderWriter* writer);

void
gbinder_local_reply_cleanup(
    GBinderLocalReply* reply,
    GDestroyNotify destroy,
    gpointer pointer);

GBinderLocalReply*
gbinder_local_reply_append_bool(
    GBinderLocalReply* reply,
    gboolean value); /* since 1.0.3 */

GBinderLocalReply*
gbinder_local_reply_append_int32(
    GBinderLocalReply* reply,
    guint32 value);

GBinderLocalReply*
gbinder_local_reply_append_int64(
    GBinderLocalReply* reply,
    guint64 value);

GBinderLocalReply*
gbinder_local_reply_append_float(
    GBinderLocalReply* reply,
    gfloat value);

GBinderLocalReply*
gbinder_local_reply_append_double(
    GBinderLocalReply* reply,
    gdouble value);

GBinderLocalReply*
gbinder_local_reply_append_string8(
    GBinderLocalReply* reply,
    const char* str);

GBinderLocalReply*
gbinder_local_reply_append_string16(
    GBinderLocalReply* reply,
    const char* utf8);

GBinderLocalReply*
gbinder_local_reply_append_hidl_string(
    GBinderLocalReply* reply,
    const char* str);

GBinderLocalReply*
gbinder_local_reply_append_hidl_string_vec(
    GBinderLocalReply* reply,
    const char* strv[],
    gssize count);

GBinderLocalReply*
gbinder_local_reply_append_local_object(
    GBinderLocalReply* reply,
    GBinderLocalObject* obj);

GBinderLocalReply*
gbinder_local_reply_append_remote_object(
    GBinderLocalReply* reply,
    GBinderRemoteObject* obj);

GBinderLocalReply*
gbinder_local_reply_append_fd(
    GBinderLocalReply* reply,
    int fd); /* Since 1.1.14 */

G_END_DECLS

#endif /* GBINDER_LOCAL_OBJECT_H */

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
